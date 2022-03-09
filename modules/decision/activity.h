#ifndef ACTIVITY
#define ACTIVITY

#include <ctime>
#include <iostream>

using namespace std;

#include "../audit/audit.h"
#include "../behavior/configuration.h"
#include "../collection/data.h"
#include "../models/markov.h"
#include "../models/models_zash.h"
#include "../models/utils.h"

#define PROB_THRESHOLD 0.1

class ActivityComponent {
   public:
    DataComponent *dataComponent;
    ConfigurationComponent *configurationComponent;
    AuditComponent *auditComponent;
    MarkovChain *markovChain = new MarkovChain();
    bool isMarkovBuilding = true;
    time_t limitDate = (time_t)(-1);
    ActivityComponent() {}
    ActivityComponent(DataComponent *d, ConfigurationComponent *c, AuditComponent *a) {
        dataComponent = d;
        configurationComponent = c;
        auditComponent = a;
    }

    bool verifyActivity(Request *req, time_t currentDate, function<bool(Request*, time_t)> explicitAuthentication) {
        cout << "Activity Component" << endl;
        checkBuilding(currentDate);
        cout << "aqui" << endl;
        vector<int> lastState = dataComponent->lastState;
        vector<int> currentState = dataComponent->currentState;
        if (!isMarkovBuilding) {
            cout << "Verify activities" << endl;
            cout << "From: " << vecToStr(lastState) << endl;
            cout << "To: " << vecToStr(currentState) << endl;
            float prob = markovChain->getProbability(currentState, lastState);
            cout << "Probability = " << prob << endl;
            if (prob < PROB_THRESHOLD) {
                auditComponent->activityFail.push_back(new AuditEvent(currentDate, req));
                cout << "Activity is NOT valid! Requires proof of identity!" << endl;
                if (!explicitAuthentication(req, currentDate)) {
                    return false;
                }
            }
            cout << "Activity is valid!" << endl;
        } else {
            cout << "Markov Chain is still building" << endl;
        }
        markovChain->buildTransition(currentState, lastState);
        return true;
    }

    // check if markov build time expired
    void checkBuilding(time_t currentDate) {
        if (difftime(limitDate, (time_t)(-1)) == 0) {
            limitDate = currentDate + configurationComponent->buildInterval * 24 * 60 * 60;  // add days
        } else if (isMarkovBuilding && difftime(currentDate, limitDate) > 0) {
            isMarkovBuilding = false;
            cout << "Markov Chain stopped building transition matrix at" << formatTime(currentDate) << endl;
        }
    }
};

#endif