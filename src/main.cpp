#include "config.h"

EepromClass eepromapi;
RellayClass rellay;
Ticker blinker, sensor, stopAll;
ESP8266WebServer HTTP(80);
WiFiUDP udp;

void setup() {
	debugSerial.begin(115200);
	debugSerial.println("\n start");
	pinMode(LED_PIN, OUTPUT);
	blinker.attach(0.6, tickBlink);
	pinMode(RESET_PIN, INPUT);
	pinMode(ALARM_1, INPUT_PULLUP);
	pinMode(ALARM_2, INPUT_PULLUP);
	eepromapi.eeprom_init();
	sensor.attach(0.1, sensorTik);
	WiFiManager wifiManager;
	wifiManager.setTimeout(180);
	wifiManager.setAPCallback([](WiFiManager *myWiFiManager){blinker.attach(0.5, tickBlink);});
	if (!wifiManager.autoConnect("iot_home")) {
		debugSerial.println("failed to connect and hit timeout");
		ESP.reset();
	}
	IOTconfig customVar = eepromapi.eeprom_get();
	if (customVar.dhc == 0xaa55) {
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		WiFi.config(customVar.statIp, gateway, subnet);
	}
	if (MDNS.begin("iot_home", WiFi.localIP())) {
		debugSerial.println("MDNS responder started");
	}
	udp.begin(8266);
	blinker.detach();
	digitalWrite(LED_PIN, HIGH);
	startServer();
}

void loop() {
	HTTP.handleClient();
	MDNS.update();
	int packetSize = udp.parsePacket();
	if(packetSize) {
		char packetBuffer[10];
		udp.read(packetBuffer, 10);
		String udpRead = packetBuffer;
		if (udpRead.indexOf("Who") != -1){
			char IP[] = "xxx.xxx.xxx.xxx";
			WiFi.localIP().toString().toCharArray(IP, 16);
			IPAddress broadcastIp = WiFi.localIP();
			broadcastIp[3] = 255;
			udp.beginPacket(broadcastIp, 8266);
			udp.write("rollet: ");
			udp.write(IP);
			udp.endPacket();
		}
	}
}

void sensorTik() {
	if (alarmStat1 == 0) alarmStat1 = digitalRead(ALARM_1);
	if (alarmStat2 == 0) alarmStat2 = digitalRead(ALARM_2);
	if (digitalRead(RESET_PIN) == 0) {
		if (resetTick++ > 100) smart_res();
	} else resetTick = 0;
	rfCode = rf_loop();
	if (rfCode > 0) {
		IOTconfig customVar = eepromapi.eeprom_get();
		if (rfCode == customVar.rfUp)  stop_all("up");
		if (rfCode == customVar.rfStop)  stop_all("stop");
		if (rfCode == customVar.rfDown)  stop_all("down");
	}
}

void tickBlink() {
	int state = digitalRead(LED_PIN);
	digitalWrite(LED_PIN, !state);
}

void smart_res() {
	debugSerial.println("Reset devise");
	WiFi.disconnect(true);
	eepromapi.eeprom_clr();
	delay(5000);
	ESP.eraseConfig();
	ESP.reset();
}

void switch_xml() {
	IOTconfig customVar = eepromapi.eeprom_get();
	String swStat = rellay.getStatus();
	String swOldStat = rellay.getOldStatus();
	String relayXml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<switch>\n\t<mac>"
		+ WiFi.macAddress()
		+ "</mac>\n\t<move>" + swStat
		+ "</move>\n\t<oldMove>" + swOldStat
		+ "</oldMove>\n\t<protTime>" + customVar.protTime
		+ "</protTime>\n\t<alarm1>" + alarmStat1
		+ "</alarm1>\n\t<alarm2>" + alarmStat2
		+ "</alarm2>\n\t<RSSI>" + WiFi.RSSI()
		+ "</RSSI>\n</switch>";	
	HTTP.send(200,"text/xml",relayXml);
}

void switch_json() {
	IOTconfig customVar = eepromapi.eeprom_get();
	String swStat = rellay.getStatus();
	String swOldStat = rellay.getOldStatus();
	String relayJson = "{\"switch\": {\"mac\":\""
		+ WiFi.macAddress()
		+ "\",\"move\":\"" + swStat
		+ "\",\"oldMove\":\"" + swOldStat
		+ "\",\"protTime\":" + customVar.protTime
		+ ",\"alarm1\":" + alarmStat1
		+ ",\"alarm2\":" + alarmStat2
		+ ",\"RSSI\":" + WiFi.RSSI()
		+ "}}";	
	HTTP.send(200,"application/json",relayJson);
}

void rf_json() {
	IOTconfig customVar = eepromapi.eeprom_get();
	String rfJson = "{\"rf\": {\"rfCode\":"
		+ String(rfCode, DEC)
		+ ",\"rfUp\":" + customVar.rfUp
		+ ",\"rfDown\":" + customVar.rfDown
		+ ",\"rfStop\":" + customVar.rfStop
		+ "}}";	
	HTTP.send(200,"application/json",rfJson);
}

void reset_alarm() {
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	alarmStat1 = 0;
	alarmStat2 = 0;
}

void rf_xml() {
	IOTconfig customVar = eepromapi.eeprom_get();
	String relayXml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<rf>\n\t<rfCode>"
		+ String(rfCode, DEC)
		+ "</rfCode>\n\t<rfUp>" + customVar.rfUp
		+ "</rfUp>\n\t<rfDown>" + customVar.rfDown
		+ "</rfDown>\n\t<rfStop>" + customVar.rfStop
		+ "</rfStop>\n</rf>";
	HTTP.send(200,"text/xml",relayXml);
}

void stop_all(String todo){
	IOTconfig customVar = eepromapi.eeprom_get();
	rellay.rellay(todo);
	if (todo == "stop") stopAll.detach();
	else if (customVar.protTime > 0) stopAll.attach(customVar.protTime, [](){stop_all("stop");});
}

void switch_web() {
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	if (HTTP.arg("rollet") != "") {
		String todo = HTTP.arg("rollet");
		if (todo == "up") stop_all("up");
		if (todo == "stop") stop_all("stop");
		if (todo == "down") stop_all("down");
		rellay.rellay(HTTP.arg("rollet"));
	}
}

void mem_set() {
	int t;
	boolean flag = false;
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	IOTconfig customVar = eepromapi.eeprom_get();
	if (HTTP.arg("ipStat") != "") {
		flag = true;
		IPAddress ip;
		ip.fromString(HTTP.arg("ipStat"));
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		delay(1000);
		WiFi.config(ip, gateway, subnet);
		customVar.statIp = ip;
		customVar.dhc = 0xaa55;
	}
	if (HTTP.arg("protTime") != "") {
		t = HTTP.arg("protTime").toInt();
		if ((t >= 0) && (t <= 30*60)) {
			flag = true;
			customVar.protTime = t;
			}
	}
	if (HTTP.arg("rfUp") != "") {
		t =  HTTP.arg("rfUp").toInt();
		customVar.rfUp = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfStop") != "") {
		t =  HTTP.arg("rfStop").toInt();
		customVar.rfStop = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (HTTP.arg("rfDown") != "") {
		t =  HTTP.arg("rfDown").toInt();
		customVar.rfDown = (t < 0)? 0:(t > 0)? t: rfCode;
		flag = true;
	}
	if (flag) eepromapi.eeprom_set(customVar);
}