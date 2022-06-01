#include <ESP8266mDNS.h>
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include "WiFiManager.h"
#include <WiFiUdp.h>
#include "eepromapi.h"
#include "relay.h"
#include "RCSwitch.h"
#include "INDEXhtm.h"
#include "STYLEcss.h"
#include "IPhtm.h"
#include "RFhtm.h"
#include "HELPhtm.h"

#define LED_PIN     2
#define RESET_PIN   0
#define ALARM_1     5
#define ALARM_2     4
#define SEN_PIN		1	//радіомодуль	1
#define debugSerial Serial1

int alarmStat1, alarmStat2;
unsigned long rfCode;
int resetTick = 0;
int udpTick = 0;

const char* upgradeIndex = R"=====(<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>)=====";

EepromClass eepromapi;
RellayClass rellay;
Ticker blinker, sensor, stopAll;
ESP8266WebServer HTTP(80);
RCSwitch mySwitch = RCSwitch();
WiFiUDP udp;

void sensorTik(void);
void tickBlink(void);
void smart_res(void);
void startServer(void);
void stop_all(String todo);

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
	// int AP_channel_default  = 7;
	// IPAddress AP_ip_default (10,1,1,1);
	// IPAddress AP_gtw_default(10,1,1,1);
	// IPAddress AP_net_default(255,0,0,0);

	WiFi.mode(WIFI_OFF);
	WiFi.softAP(AP_ssid_default, AP_pass_default);
	// WiFi.softAP(AP_ssid_default, AP_pass_default, AP_channel_default);
	// WiFi.setPhyMode(WIFI_PHY_MODE_11N);
	// WiFi.softAPConfig(AP_ip_default, AP_gtw_default, AP_net_default);
	digitalWrite(LED_PIN, HIGH);
}

void STATION_mode(IOTconfig customVar){
	blinker.attach(0.5, tickBlink);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	// WiFi.config (STA_IP, STA_DNS, STA_GTW, STA_NET);
	// String STA_host = "rollet_" + WiFi.macAddress();
	// WiFi.hostname(STA_host);
	WiFi.begin(customVar.STA_ssid, customVar.STA_pass);
	hold(100);
	while (WiFi.status() != WL_CONNECTED){
		debugSerial.print(".");
		if(WiFi.status() == WL_NO_SSID_AVAIL){
			// TROCA_REDE();
		}
		if(WiFi.status() ==  WL_CONNECT_FAILED){
			// TROCA_REDE();
		}
	}

	if (customVar.dhc == 0xaa55) {
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		WiFi.config(customVar.statIp, gateway, subnet);
	}
	if (MDNS.begin("iot_home", WiFi.localIP())) {
		debugSerial.println("MDNS responder started");
	}
	blinker.detach();
	digitalWrite(LED_PIN, HIGH);
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

void setup() {
	debugSerial.begin(115200);
	debugSerial.println("\n start");
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
	pinMode(RESET_PIN, INPUT);
	pinMode(ALARM_1, INPUT_PULLUP);
	pinMode(ALARM_2, INPUT_PULLUP);
	mySwitch.enableReceive(SEN_PIN);
	eepromapi.eeprom_init();
	sensor.attach(0.1, sensorTik);
	IOTconfig customVar = eepromapi.eeprom_get();
	if (customVar.wifimode == 0) AP_mode_default();
	else STATION_mode(customVar);
	udp.begin(8266);
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
	if (mySwitch.available()) {
		rfCode = mySwitch.getReceivedValue();
		if (rfCode > 0) {
			IOTconfig customVar = eepromapi.eeprom_get();
			if (rfCode == customVar.rfUp)  stop_all("up");
			if (rfCode == customVar.rfStop)  stop_all("stop");
			if (rfCode == customVar.rfDown)  stop_all("down");
		}
		mySwitch.resetAvailable();
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
	hold(5000);
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
	alarmStat1 = 0;
	alarmStat2 = 0;
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
	boolean rst = false;
	HTTP.send(200,"application/json","{\"status\":\"ok\"}");
	IOTconfig customVar = eepromapi.eeprom_get();
	if (HTTP.arg("ipStat") != "") {
		flag = true;
		IPAddress ip;
		ip.fromString(HTTP.arg("ipStat"));
		IPAddress gateway = WiFi.gatewayIP();
		IPAddress subnet = WiFi.subnetMask();
		hold(1000);
		WiFi.config(ip, gateway, subnet);
		customVar.statIp = ip;
		customVar.dhc = 0xaa55;
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
	HTTP.on("/resalarm", HTTP_GET, reset_alarm);
	HTTP.on("/switch.xml", HTTP_GET, switch_xml);
	HTTP.on("/switch.json", HTTP_GET, switch_json);
	HTTP.on("/scanwifi.json", HTTP_GET, scanwifi_json);
	HTTP.on("/rf.json", HTTP_GET, rf_json);
    HTTP.on("/rf.htm", HTTP_GET, [](){HTTP.send(200, "text/html", rfIndex);});
    HTTP.on("/ip.htm", HTTP_GET, [](){HTTP.send(200, "text/html", ipIndex);});
    HTTP.on("/help", HTTP_GET, [](){HTTP.send(200, "text/html", helpIndex);});
	HTTP.on("/mem", HTTP_GET, mem_set);
	HTTP.on("/default", HTTP_GET, [](){
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		hold(5000);
		smart_res();
		});
	HTTP.on("/reboot", HTTP_GET, [](){
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		hold(5000);
		ESP.restart();
		hold(500);
		});
    HTTP.on("/upgrade", HTTP_GET, [](){HTTP.send(200, "text/html", upgradeIndex);});
    HTTP.on("/update", HTTP_POST, [](){
		HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		hold(5000);
		ESP.restart();
		hold(500);
    },[](){
		HTTPUpload& upload = HTTP.upload();
		if(upload.status == UPLOAD_FILE_START){
			debugSerial.setDebugOutput(true);
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
}