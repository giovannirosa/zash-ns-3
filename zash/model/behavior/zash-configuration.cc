#include "zash-configuration.h"

namespace ns3 {

ConfigurationComponent::ConfigurationComponent() {}
ConfigurationComponent::ConfigurationComponent(int bt, int bi, int bdi,
                                               vector<Device *> d,
                                               vector<User *> u,
                                               vector<Ontology *> o) {
  blockThreshold = bt;
  blockInterval = bi;
  buildInterval = bdi;
  devices = d;
  users = u;
  ontologies = o;
}
} // namespace ns3