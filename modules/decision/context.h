#ifndef CONTEXT
#define CONTEXT

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

using namespace std;

#include "../audit/audit.h"
#include "../behavior/configuration.h"
#include "../models/enums_zash.h"
#include "../models/models_zash.h"
#include "../models/utils.h"

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
    Device *device;
    enums::Enum *userLevel;
    enums::Enum *action;
    int totalOcc = 0;
    vector<TimePercentage *> times = {
        new TimePercentage(enums::MORNING),
        new TimePercentage(enums::AFTERNOON),
        new TimePercentage(enums::NIGHT)};
    TimeObject(Device *d, enums::Enum *u, enums::Enum *a) {
        device = d;
        userLevel = u;
        action = a;
    }
};

struct compareTObj {
    Request *key;
    compareTObj(Request *r) : key(r) {}

    bool operator()(TimeObject *t) {
        return t->device->id == key->device->id &&
               t->userLevel->value == key->user->userLevel->value &&
               t->action->value == key->action->value;
    }
};

struct compareTimes {
    int key;
    compareTimes(int t) : key(t) {}

    bool operator()(TimePercentage *t) {
        return t->time == key;
    }
};

class ContextComponent {
   public:
    ConfigurationComponent *configurationComponent;
    AuditComponent *auditComponent;
    bool isTimeBuilding = true;
    time_t limitDate = (time_t)(-1);
    vector<TimeObject *> timeProbs;
    ContextComponent() {}
    ContextComponent(ConfigurationComponent *c, AuditComponent *a) {
        configurationComponent = c;
        auditComponent = a;
        for (Device *device : c->devices) {
            for (const pair<const char *const, enums::Enum *> ul : enums::UserLevel) {
                for (const pair<const char *const, enums::Enum *> act : enums::Action) {
                    timeProbs.push_back(new TimeObject(device, ul.second, act.second));
                }
            }
        }
    }

    // static trust calculation based on expected
    // for [DeviceClass x Action] and [UserLevel x Action]
    // from [AccessWay, Localization, Time, Age, Group]
    bool verifyContext(Request *req, time_t currentDate, function<bool(Request *, time_t)> explicitAuthentication) {
        cout << "Context Component" << endl;
        calculateTime(req, currentDate);
        checkBuilding(currentDate);
        if (isTimeBuilding) {
            cout << "Time probability is still building" << endl;
            req->context->time = enums::TimeClass.at("COMMON");
        }
        cout << "Verify context with " << req->user->id << " in ";
        printFormattedTime(currentDate);
        cout << endl;
        int expectedDevice = req->device->deviceClass->weight + req->action->weight;
        int expectedUser = req->user->userLevel->weight + req->action->weight;
        int expected = min(max(expectedDevice, expectedUser), 100);
        int calculated = min(calculateTrust(req->context, req->user), 100);
        cout << "Trust level is " << calculated << " and expected is " << expected << endl;
        if (calculated < expected) {
            auditComponent->contextFail.push_back(new AuditEvent(currentDate, req));
            cout << "Trust level is BELOW expected! Requires proof of identity!" << endl;
            if (!explicitAuthentication(req, currentDate)) {
                return false;
            }
        }
        cout << "Trust level is ABOVE expected!" << endl;
        return true;
    }

    // check if markov build time expired
    void checkBuilding(time_t currentDate) {
        if (difftime(limitDate, (time_t)(-1)) == 0) {
            limitDate = currentDate + configurationComponent->buildInterval * 24 * 60 * 60;  // add days
            cout << "Current date = ";
            printFormattedTime(currentDate);
            cout << endl;
            cout << "Limit date   = ";
            printFormattedTime(limitDate);
            cout << endl;
        } else if (isTimeBuilding && difftime(currentDate, limitDate) > 0) {
            isTimeBuilding = false;
            cout << "Time context stopped building probabilities at ";
            printFormattedTime(currentDate);
            cout << endl;
        }
    }

    int calculateTrust(Context *context, User *user) {
        return context->accessWay->weight +
               context->localization->weight +
               context->time->weight +
               user->age->weight +
               context->group->weight;
    }

    void calculateTime(Request *req, time_t currentDate) {
        int time = 0;
        int hour = extractHour(currentDate);
        if (hour >= 6 && hour < 12) {
            time = enums::MORNING;
        } else if (hour >= 12 && hour < 18) {
            time = enums::AFTERNOON;
        } else if ((hour >= 18 && hour <= 23) || (hour >= 0 && hour < 6)) {
            time = enums::NIGHT;
        }

        auto it = find_if(timeProbs.begin(), timeProbs.end(), compareTObj(req));
        if (it != timeProbs.end()) {
            cout << "Found timeObj" << endl;
            it[0]->totalOcc++;
            TimeObject *timeObj = it[0];

            recalculateProbabilities(timeObj, time);

            auto it2 = find_if(timeObj->times.begin(), timeObj->times.end(), compareTimes(time));
            if (it2 != timeObj->times.end()) {
                cout << "Found timePct" << endl;
                TimePercentage *timePct = it2[0];
                if (timePct->percentage < 0.3) {
                    req->context->time = enums::TimeClass.at("COMMON");
                } else {
                    req->context->time = enums::TimeClass.at("UNCOMMON");
                }
            }
        }
    }

    void recalculateProbabilities(TimeObject *timeObj, int time) {
        ++timeObj->totalOcc;
        for (TimePercentage *timePct : timeObj->times) {
            if (timePct->time == time) {
                ++timePct->occurrences;
            }
            timePct->percentage = (float)timePct->occurrences / timeObj->totalOcc;
        }
    }
};

#endif