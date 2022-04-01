#ifndef CONFIGURATION
#define CONFIGURATION

#include "ns3/zash-models.h"

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
  ConfigurationComponent();
  ConfigurationComponent(int bt, int bi, int bdi, vector<Device *> d,
                         vector<User *> u, vector<Ontology *> o);
};
} // namespace ns3

#endif