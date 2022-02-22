#include <ctime>
#include <iostream>

using namespace std;

#include "audit.h"
#include "configuration.h"
#include "enums_zash.h"
#include "models_zash.h"

class TimePercentage {
   public:
    int time;
    float percentage = 0;
    int occurrences = 0;
    TimePercentage(int t) {
        time = t;
    }
};

class TimeObject {
   public:
    Device device;
    enums::Enum userLevel;
    enums::Enum action;
    int totalOcc = 0;
    vector<TimePercentage> times = {
        TimePercentage(enums::MORNING),
        TimePercentage(enums::AFTERNOON),
        TimePercentage(enums::NIGHT)};
    TimeObject(Device d, enums::Enum u, enums::Enum a) {
        device = d;
        userLevel = u;
        action = a;
    }
};

class ContextComponent {
   public:
    ConfigurationComponent configurationComponent;
    AuditComponent auditComponent;
    bool isTimeBuilding = true;
    time_t limitDate = NULL;
    vector<TimeObject *> timeProbs;
    ContextComponent(ConfigurationComponent c, AuditComponent a) {
        configurationComponent = c;
        auditComponent = a;
        for (Device device : c.devices) {
            for (const pair<const char *const, enums::Enum *> ul : enums::UserLevel) {
                for (const pair<const char *const, enums::Enum *> act : enums::Action) {
                    timeProbs.push_back(new TimeObject(device, *ul.second, *act.second));
                }
            }
        }
    }

    
};