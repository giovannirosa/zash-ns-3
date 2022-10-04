#ifndef CONFIGURATION
#define CONFIGURATION

#include "ns3/log.h"
#include "ns3/zash-audit.h"
#include "ns3/zash-models.h"
#include <algorithm>

using namespace std;

namespace ns3 {

class ConfigurationComponent
{
public:
  int blockThreshold;
  int blockInterval;
  int buildInterval;
  double markovThreshold;
  vector<Device *> devices;
  vector<User *> users;
  vector<Ontology *> ontologies;
  AuditComponent *auditModule;
  enums::Properties *props;
  bool isBuilding;
  ConfigurationComponent ();
  ConfigurationComponent (int bt, int bi, int bdi, double m, vector<Device *> d, vector<User *> u,
                          vector<Ontology *> o, AuditComponent *a, enums::Properties *p);

  bool addDevice (Device *d);

  bool remDevice (Device *d);
};
} // namespace ns3

#endif
