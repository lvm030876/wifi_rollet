#ifndef EEPROMCLASS_H
#define EEPROMCLASS_H
#include "EEPROM.h"
#include <ESP8266WiFi.h>

struct IOTconfig{
  int dhc;
  IPAddress statIp;
  int protTime;
  unsigned long rfUp;
  unsigned long rfDown;
  unsigned long rfStop;
  int start;
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

#endif