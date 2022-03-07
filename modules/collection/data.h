#ifndef DATA
#define DATA

using namespace std;

#include "../models/models_zash.h"

class DataComponent {
   public:
    vector<int> lastState;
    vector<int> currentState;

    void updateCurrentState(Request req) {
        currentState = lastState;
        int currentDeviceState = currentState[req.device->id - 1];
        currentState[req.device->id - 1] = currentDeviceState ? 0 : 1;
    }

    void updateLastState() {
        delete &lastState;
        lastState = currentState;
    }
};

#endif