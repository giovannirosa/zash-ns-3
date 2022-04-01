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

#define PROB_THRESHOLD 0.1

namespace ns3 {

class ActivityComponent {
public:
  DataComponent *dataComponent;
  ConfigurationComponent *configurationComponent;
  AuditComponent *auditComponent;
  MarkovChain *markovChain = new MarkovChain();
  bool isMarkovBuilding = true;
  time_t limitDate = (time_t)(-1);
  ActivityComponent();
  ActivityComponent(DataComponent *d, ConfigurationComponent *c,
                    AuditComponent *a);

  bool verifyActivity(Request *req, time_t currentDate,
                      function<bool(Request *, time_t)> explicitAuthentication);

  // check if markov build time expired
  void checkBuilding(time_t currentDate);
};
} // namespace ns3

#endif