#define SWITCH_A 12
#define SWITCH_B 13

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

RellayClass::RellayClass() {
    _status = "stop";
    _oldStatus = "stop";
    pinMode(SWITCH_A, OUTPUT);
    pinMode(SWITCH_B, OUTPUT);
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