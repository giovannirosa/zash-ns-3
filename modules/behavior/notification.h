using namespace std;

#include "configuration.h"
#include "models_zash.h"

class NotificationComponent {
   public:
    ConfigurationComponent configurationComponent;
    NotificationComponent(ConfigurationComponent c) {
        configurationComponent = c;
    }

    void alertUsers(User blockedUser) {
        for (User user : configurationComponent.users) {
            cout << "User " << user.id << " received message: 'User " << blockedUser.id << " is blocked!'";
        }
    }
};