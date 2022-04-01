#ifndef NOTIFICATION
#define NOTIFICATION

#include "ns3/zash-configuration.h"
#include "ns3/zash-models.h"

using namespace std;

namespace ns3 {

class NotificationComponent {
public:
  ConfigurationComponent *configurationComponent;
  NotificationComponent();
  NotificationComponent(ConfigurationComponent *c);

  void alertUsers(User *blockedUser);
};
} // namespace ns3

#endif