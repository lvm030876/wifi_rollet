#include "RCSwitch.h"

#define SEN_PIN 1	//радіомодуль	1

RCSwitch mySwitch = RCSwitch();

unsigned long rf_loop(){

    static bool s = false;
    if(!s) { // это я сделал, чтобы не писать лишнее в setup()
        s = true;
        mySwitch.enableReceive(SEN_PIN);
    }

    unsigned long rfCode = 0;
	if (mySwitch.available()) {
		rfCode = mySwitch.getReceivedValue();
		mySwitch.resetAvailable();
	}
    return rfCode;
}