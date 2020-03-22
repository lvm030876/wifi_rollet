#include "Arduino.h"
#include "eepromclass.h"

void EepromClass::eeprom_init() {
	EEPROM.begin(512);
	eeprom_load();
	if (_customVar.start != 0xaa55) {
		_customVar.protTime = 0;
		_customVar.rfUp = 0;
		_customVar.rfStop = 0;
		_customVar.rfDown = 0;
		_customVar.start = 0xaa55;
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