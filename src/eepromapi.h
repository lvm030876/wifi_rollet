#include "EEPROM.h"
#include <ESP8266WiFi.h>

struct IOTconfig{
  int start;
  int wifimode;
  char STA_ssid[32];
  char STA_pass[32];
  int dhc;
  IPAddress statIp;
  int protTime;
  unsigned long rfUp;
  unsigned long rfDown;
  unsigned long rfStop;
};

class EepromClass {
  public:
    void eeprom_init();
    void eeprom_clr();
    void eeprom_set(IOTconfig &obj);
    IOTconfig eeprom_get();
  private:
    IOTconfig _customVar;
    int _len = sizeof(_customVar);
    void eeprom_save();
    void eeprom_load();
};

void EepromClass::eeprom_init() {
	EEPROM.begin(512);
	eeprom_load();
	if (_customVar.start != _len) {
		_customVar.wifimode = 0;
		_customVar.dhc = 0;
    // String str = "lvm1976";
    // str.toCharArray(_customVar.STA_ssid, str.length() + 1);
    // str = "66666666";
    // str.toCharArray(_customVar.STA_pass, str.length() + 1);
		_customVar.protTime = 30;
		_customVar.rfUp = 0;
		_customVar.rfStop = 0;
		_customVar.rfDown = 0;
		_customVar.start = _len;
		eeprom_save();
	}
}

void EepromClass::eeprom_clr() {
	for (int i = 0; i < _len; i++) EEPROM.write(i, 0);
    EEPROM.commit();
}

void EepromClass::eeprom_save() {
	for (int i = 0; i < _len; i++) EEPROM.write(i, *((char*)&_customVar+i));
    EEPROM.commit();
}

void EepromClass::eeprom_load() {
	char date[_len];
	for (int i = 0; i < _len; i++) date[i] = EEPROM.read(i);
	memcpy((char*)&_customVar, date, _len);
    EEPROM.commit();
}

void EepromClass::eeprom_set(IOTconfig &obj){
	memcpy((char*)&_customVar, (char*)&obj, _len);
	eeprom_save();
}

IOTconfig EepromClass::eeprom_get() {
	return _customVar;
}