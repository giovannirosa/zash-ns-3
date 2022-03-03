#ifndef NOTIFICATION
#define NOTIFICATION

using namespace std;

#include "configuration.h"
#include "../models/models_zash.h"

class NotificationComponent {
   public:
    ConfigurationComponent configurationComponent;
    NotificationComponent() {}
    NotificationComponent(ConfigurationComponent c) {
        configurationComponent = c;
    }

    void alertUsers(User blockedUser) {
        for (User *user : configurationComponent.users) {
            cout << "User " << user->id << " received message: 'User " << blockedUser.id << " is blocked!'";
        }
    }
};

#endif