#ifndef config_h
#define config_h
#include "Arduino.h"
#include <ESP8266mDNS.h>
#include "Ticker.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include "WiFiManager.h"
#include "RCSwitch.h"
#include "eepromclass.h"
#include "relay.h"
#include "index.h"
#include "ipstat.h"
#include "rfstat.h"
#include <WiFiUdp.h>


// #define PROGMEM   ICACHE_RODATA_ATTR
#define LED_PIN 2
#define RESET_PIN 0
#define ALARM_1 5	//датчик руху
#define ALARM_2 4	//RS пульт	4
#define SEN_PIN 1	//радіомодуль	1
#define debugSerial Serial1

void startServer();
void tickBlink();
void smart_res();
void switch_xml();
void switch_json();
void rf_xml();
void rf_json();
void reset_alarm();
void switch_web();
void mem_set();
void sensorTik();
void stop_all(String todo);

int alarmStat1, alarmStat2;
unsigned long rfCode;
int resetTick = 0;  // 
int udpTick = 0;  // 

const char* upgradeIndex = R"=====(<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>)=====";

#endif