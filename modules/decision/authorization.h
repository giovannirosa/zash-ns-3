#ifndef AUTHORIZATION
#define AUTHORIZATION

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

using namespace std;

#include "../audit/audit.h"
#include "../behavior/configuration.h"
#include "../behavior/notification.h"
#include "../models/enums_zash.h"
#include "../models/models_zash.h"
#include "activity.h"
#include "context.h"
#include "ontology.h"

class AuthorizationComponent {
   public:
    ConfigurationComponent *configurationComponent;
    OntologyComponent *ontologyComponent;
    ContextComponent *contextComponent;
    ActivityComponent *activityComponent;
    NotificationComponent *notificationComponent;
    AuditComponent *auditComponent;
    AuthorizationComponent() {}
    AuthorizationComponent(ConfigurationComponent *c,
                           OntologyComponent *o,
                           ContextComponent *ctx,
                           ActivityComponent *a,
                           NotificationComponent *n,
                           AuditComponent *adt) {
        configurationComponent = c;
        ontologyComponent = o;
        contextComponent = ctx;
        activityComponent = a;
        notificationComponent = n;
        auditComponent = adt;
    }

    bool authorizeRequest(Request *req, time_t currentDate, function<bool(Request *, time_t)> explicitAuthentication) {
        cout << "Authorization Component" << endl;
        cout << "Processing Request: " << req->id << endl;
        checkUsers(currentDate);
        if (req->user->blocked) {
            ++auditComponent->reqDenied;
            cout << "USER IS BLOCKED - Request is NOT authorized!" << endl;
            return false;
        }
        if (!ontologyComponent->verifyOntology(req, currentDate) ||
            !contextComponent->verifyContext(req, currentDate, explicitAuthentication) ||
            !activityComponent->verifyActivity(req, currentDate, explicitAuthentication)) {
            // cout << "aqui2" << endl;
            req->user->rejected.push_back(currentDate);
            cout << "User have now " << req->user->rejected.size() << " rejected requests!" << endl;
            if (req->user->rejected.size() > configurationComponent->blockThreshold) {
                auditComponent->blocks.push_back(new AuditEvent(currentDate, req));
                req->user->blocked = true;
                cout << "User " << req->user->id << " is blocked!" << endl;
                notificationComponent->alertUsers(req->user);
            }
            ++auditComponent->reqDenied;
            cout << "Request is NOT authorized!" << endl;
            return false;
        }
        // cout << "aqui" << endl;
        ++auditComponent->reqGranted;
        cout << "Request is authorized!" << endl;
        return true;
    }

    void checkUsers(time_t currentDate) {
        for (User *user : configurationComponent->users) {
            remove_if(user->rejected.begin(), user->rejected.end(), [this, currentDate](time_t time) { return difftime(currentDate, time) < configurationComponent->blockInterval; });
        }
    }
};

#endif