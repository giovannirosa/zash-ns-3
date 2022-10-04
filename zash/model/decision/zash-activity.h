#ifndef ACTIVITY
#define ACTIVITY

#include <ctime>
#include <iostream>

using namespace std;

#include "ns3/zash-audit.h"
#include "ns3/zash-configuration.h"
#include "ns3/zash-data.h"
#include "ns3/zash-markov.h"
#include "ns3/zash-models.h"
#include "ns3/zash-utils.h"

namespace ns3 {

class ActivityComponent
{
public:
  DataComponent *dataComponent;
  ConfigurationComponent *configurationComponent;
  AuditComponent *auditComponent;
  MarkovChain *markovChain = new MarkovChain ();
  bool isMarkovBuilding = true;
  time_t limitDate = (time_t) (-1);
  ActivityComponent ();
  ActivityComponent (DataComponent *d, ConfigurationComponent *c, AuditComponent *a);

  bool verifyActivity (Request *req, function<bool (Request *)> explicitAuthentication);

  // check if markov build time expired
  void checkBuilding (time_t currentDate);
  void resetMarkov (time_t currentDate);
};
} // namespace ns3

#endif
