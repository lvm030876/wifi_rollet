#include "relay.h"

RellayClass::RellayClass() {
    _status = "stop";
    _oldStatus = "stop";
    pinMode(SWITCH_A, OUTPUT);
    pinMode(SWITCH_B, OUTPUT);
    pinMode(SWITCH_C, OUTPUT);
    rellay("stop");
};

RellayClass::~RellayClass() {};

void RellayClass::rellay(String todo) {
    _status = todo;
    if (todo == "up") {
        digitalWrite(SWITCH_A, 0);
        digitalWrite(SWITCH_B, 1);
        _oldStatus = todo;
    }
    if (todo == "stop") {
        digitalWrite(SWITCH_A, 0);
        digitalWrite(SWITCH_B, 0);
    }
    if (todo == "down") {
        digitalWrite(SWITCH_A, 1);
        digitalWrite(SWITCH_B, 1);
        _oldStatus = todo;
    }
};

String RellayClass::getOldStatus(){
    return _oldStatus;
};

String RellayClass::getStatus(){
    return _status;
};