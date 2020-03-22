#ifndef RELAY_H
#define RELAY_H
#include "Arduino.h"

#define SWITCH_A 12
#define SWITCH_B 13
#define SWITCH_C 14

class RellayClass {
private:
    String _status;
    String _oldStatus;
public:
    RellayClass();
    ~RellayClass();
    void rellay(String todo);
    String getStatus();
    String getOldStatus();
    void setProtTime(int a);
};
#endif