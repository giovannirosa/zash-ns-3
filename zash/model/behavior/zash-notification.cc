#include "zash-notification.h"

namespace ns3 {

NotificationComponent::NotificationComponent() {}
NotificationComponent::NotificationComponent(ConfigurationComponent *c) {
  configurationComponent = c;
}

void NotificationComponent::alertUsers(User *blockedUser) {
  for (User *user : configurationComponent->users) {
    cout << "User " << user->id << " received message: 'User "
         << blockedUser->id << " is blocked!'";
  }
}

} // namespace ns3