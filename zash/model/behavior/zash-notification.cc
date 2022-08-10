#include "zash-notification.h"

namespace ns3 {

NotificationComponent::NotificationComponent() {}
NotificationComponent::NotificationComponent(ConfigurationComponent *c, AuditComponent *a) {
  configurationComponent = c;
  auditComponent = a;
}

void NotificationComponent::alertUsers(User *blockedUser) {
  for (User *user : configurationComponent->users) {
    *auditComponent->zashOutput << "User " << user->id << " received message: 'User "
         << blockedUser->id << " is blocked!'";
  }
}

} // namespace ns3