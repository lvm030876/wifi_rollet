#include <ESP8266mDNS.h>
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <WiFiUdp.h>
#include "eepromapi.h"
#include "relay.h"
#include "RCSwitch.h"
#include "webgui.h"

#define LED_PIN     2
#define RESET_PIN   0
#define ALARM_PIN	14
#define RF_PIN		3	//радіомодуль	1
#define debugSerial Serial

int alarmStat;
unsigned long rfCode;
int resetTick = 0;
int udpTick = 0;

EepromClass eepromapi;
IOTconfig customVar;
RellayClass rellay;
Ticker blinker, sensor, stopAll;
ESP8266WebServer HTTP(80);
RCSwitch mySwitch = RCSwitch();
WiFiUDP udp;

void stop_all(String todo);

void tickBlink() {
	int state = digitalRead(LED_PIN);
	digitalWrite(LED_PIN, !state);
}

void hold(const unsigned int &ms) {
	unsigned long m = millis();
	while (millis() - m < ms) {
		yield();
	}
}

void AP_mode_default(){
	blinker.attach(3, tickBlink);
	String nameDev = WiFi.macAddress();
	nameDev.replace(":", "");
	nameDev = "ROLLET_" + nameDev;
	char AP_ssid_default[32]; 
	nameDev.toCharArray(AP_ssid_default, nameDev.length() + 1);
	char AP_pass_default[32] = "12345678";
	WiFi.mode(WIFI_OFF);
	WiFi.softAP(AP_ssid_default, AP_pass_default);
	digitalWrite(LED_PIN, HIGH);
	debugSerial.println("AP started");
}

void STATION_mode(){
	blinker.attach(0.5, tickBlink);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	WiFi.hostname("iot-rollet");
	if (customVar.dhc == 0xaa55) {
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		WiFi.config(customVar.statIp, gateway, subnet);
	}
	WiFi.begin(customVar.STA_ssid, customVar.STA_pass);
	hold(100);
	while (WiFi.status() != WL_CONNECTED){
		delay(250); debugSerial.print(".");
		if(WiFi.status() == WL_NO_SSID_AVAIL){
			// TROCA_REDE();
		}
		if(WiFi.status() ==  WL_CONNECT_FAILED){
			// TROCA_REDE();
		}
	}
	blinker.detach();
	digitalWrite(LED_PIN, HIGH);
	debugSerial.println("\n STA started");
}

void scanwifi_json() {
	int n = WiFi.scanNetworks();
	String outStr = "{\"scan\":[";
	// '{"sacn":[]}'
	if (n > 0){
		// '{"sacn":[{"sidd":"lvm1976","rssi":"66666666"},{"sidd":"roll","rssi":"12345678"}]}' 
		for (int i = 0; i < n; i++){
			outStr += "{\"ssid\":\"";
			outStr += WiFi.SSID(i);
			outStr += "\", ";
			outStr += "\"mac\":\"";
			outStr += WiFi.BSSIDstr(i);
			outStr += "\", ";
			outStr += "\"channel\":";
			outStr += WiFi.channel(i);
			outStr += ", ";
			outStr += "\"pass\":";
			outStr += WiFi.encryptionType(i) != ENC_TYPE_NONE ? "true" : "false";
			outStr += ", ";
			outStr += "\"hidden\":";
			outStr += WiFi.isHidden(i)? "true" : "false";
			outStr += ", ";
			outStr += "\"rssi\":";
			outStr += WiFi.RSSI(i);
			outStr += "}";
			if ((i + 1) != n) outStr += ", ";
		}
	}
	outStr += "]}";
	HTTP.send(200,"application/json",outStr);
	WiFi.scanDelete();
}

void smart_res() {
	debugSerial.println("Reset devise");
	WiFi.disconnect(true);
	eepromapi.eeprom_clr();
	hold(5000);
	ESP.eraseConfig();
	ESP.reset();
}

void stop_all(String todo){
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
	}
}

void sensorTik() {
	if (alarmStat == 0) alarmStat = digitalRead(ALARM_PIN);
	if (digitalRead(RESET_PIN) == 0) {
		if (resetTick++ > 100) smart_res();
	} else resetTick = 0;
	if (mySwitch.available()) {
		rfCode = mySwitch.getReceivedValue();
		if (rfCode > 0) {
			if (rfCode == customVar.rfUp)  stop_all("up");
			if (rfCode == customVar.rfStop)  stop_all("stop");
			if (rfCode == customVar.rfDown)  stop_all("down");
		}
		mySwitch.resetAvailable();
	}
}

void switch_xml() {
	String relayXml = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n<switch>\n\t<mac>"
		+ WiFi.macAddress()
		+ "</mac>\n\t<move>" + rellay.getStatus()
		+ "</move>\n\t<oldMove>" + rellay.getOldStatus()
		+ "</oldMove>\n\t<protTime>" + customVar.protTime
		+ "</protTime>\n\t<alarm>" + alarmStat
		+ "</alarm>\n\t<RSSI>" + WiFi.RSSI()
		+ "</RSSI>\n</switch>";	
	HTTP.send(200,"text/xml",relayXml);
}

void switch_json() {
	String relayJson = "{\"switch\": {\"mac\":\""
		+ WiFi.macAddress()
		+ "\",\"move\":\"" + rellay.getStatus()
		+ "\",\"oldMove\":\"" + rellay.getOldStatus()
		+ "\",\"protTime\":" + customVar.protTime
		+ ",\"alarm\":" + alarmStat
		+ ",\"RSSI\":" + WiFi.RSSI()
		+ "}}";	
	HTTP.send(200,"application/json",relayJson);
}

void rf_json() {
	String rfJson = "{\"rf\": {\"rfNew\":"
		+ String(rfCode, DEC)
		+ ",\"rfUp\":" + customVar.rfUp
		+ ",\"rfDown\":" + customVar.rfDown
		+ ",\"rfStop\":" + customVar.rfStop
		+ "}}";	
	HTTP.send(200,"application/json",rfJson);
}

void reset_alarm() {
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	alarmStat = 0;
}

void mem_set() {
	int t;
	boolean flag = false;
	boolean rst = false;
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	if (HTTP.arg("ipStat") != "") {
		flag = true;
		IPAddress ip;
		ip.fromString(HTTP.arg("ipStat"));
		customVar.statIp = ip;
		customVar.dhc = 0xaa55;
		rst = true;
	}
	if (HTTP.arg("ssid") != "") {
		flag = true;
		String str = HTTP.arg("ssid");
    	str.toCharArray(customVar.STA_ssid, str.length() + 1);
		customVar.wifimode = 1;
		rst = true;
	}
	if (HTTP.arg("pass") != "") {
		flag = true;
		String str = HTTP.arg("pass");
    	str.toCharArray(customVar.STA_pass, str.length() + 1);
		customVar.wifimode = 1;
		rst = true;
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
	if (rst) {
		hold(5000);
		ESP.restart();
		hold(500);
	}
}

void startServer() {
    HTTP.on("/", HTTP_GET, [](){HTTP.send(200, "text/html", homeIndex);});
    HTTP.on("/style.css", HTTP_GET, [](){HTTP.send(200, "text/css", style);});
	HTTP.on("/switch", HTTP_GET, switch_web);
	HTTP.on("/switch.xml", HTTP_GET, switch_xml);
	HTTP.on("/switch.json", HTTP_GET, switch_json);
	HTTP.on("/resalarm", HTTP_GET, reset_alarm);
    HTTP.on("/rf.htm", HTTP_GET, [](){HTTP.send(200, "text/html", rfIndex);});
	HTTP.on("/rf.json", HTTP_GET, rf_json);
    HTTP.on("/ip.htm", HTTP_GET, [](){HTTP.send(200, "text/html", ipIndex);});
	HTTP.on("/scanwifi.json", HTTP_GET, scanwifi_json);
    HTTP.on("/help", HTTP_GET, [](){HTTP.send(200, "text/html", helpIndex);});
	HTTP.on("/mem", HTTP_GET, mem_set);
	HTTP.on("/default", HTTP_GET, [](){
		HTTP.send(200, "text/plain", "OK");
		hold(5000);
		smart_res();
		});
	HTTP.on("/reboot", HTTP_GET, [](){
		HTTP.send(200, "text/plain", "OK");
		hold(5000);
		ESP.restart();
		hold(500);
		});
    HTTP.on("/upgrade", HTTP_GET, [](){HTTP.send(200, "text/html", upgradeIndex);});
    HTTP.on("/update", HTTP_POST, [](){
		HTTP.sendHeader("Connection", "close");
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		hold(5000);
		ESP.restart();
		hold(500);
    },[](){
		HTTPUpload& upload = HTTP.upload();
		if(upload.status == UPLOAD_FILE_START){
			debugSerial.setDebugOutput(true);
			WiFiUDP::stopAll();
			debugSerial.printf("Update: %s\n", upload.filename.c_str());
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			if(!Update.begin(maxSketchSpace)){
				Update.printError(debugSerial);
			}
		} else if(upload.status == UPLOAD_FILE_WRITE){
			if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
				Update.printError(debugSerial);
			}
		} else if(upload.status == UPLOAD_FILE_END){
			if(Update.end(true)){
				debugSerial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
			} else {
				Update.printError(debugSerial);
			}
			debugSerial.setDebugOutput(false);
		}
		yield();
    });
	HTTP.begin();
	debugSerial.println("server started");
}

void setup() {
	debugSerial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
	debugSerial.println("\n start");
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
	pinMode(RESET_PIN, INPUT);
	pinMode(ALARM_PIN, INPUT_PULLUP);
	mySwitch.enableReceive(RF_PIN);
	eepromapi.eeprom_init();
	sensor.attach(0.1, sensorTik);
	customVar = eepromapi.eeprom_get();
	if (customVar.wifimode == 0) AP_mode_default();
	else STATION_mode();
	if (MDNS.begin("iot-rollet", WiFi.localIP())) {
		debugSerial.println("\n MDNS responder started, name is http://iot-rollet.local/");
	}
	udp.begin(8266);
	startServer();
	MDNS.addService("http", "tcp", 80);
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