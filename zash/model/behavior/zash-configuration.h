#ifndef CONFIGURATION
#define CONFIGURATION

#include "ns3/log.h"
#include "ns3/zash-audit.h"
#include "ns3/zash-models.h"
#include <algorithm>

using namespace std;

namespace ns3 {

class ConfigurationComponent {
public:
  int blockThreshold;
  int blockInterval;
  int buildInterval;
  vector<Device *> devices;
  vector<User *> users;
  vector<Ontology *> ontologies;
  AuditComponent *auditModule;
  ConfigurationComponent();
  ConfigurationComponent(int bt, int bi, int bdi, vector<Device *> d,
                         vector<User *> u, vector<Ontology *> o,
                         AuditComponent *a);

  bool addDevice(Device *d);

  bool remDevice(Device *d);
};
} // namespace ns3

#endif