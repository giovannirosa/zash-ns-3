#ifndef CONTEXT
#define CONTEXT

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

using namespace std;

#include "ns3/zash-audit.h"
#include "ns3/zash-configuration.h"
#include "ns3/zash-enums.h"
#include "ns3/zash-models.h"
#include "ns3/zash-utils.h"

namespace ns3 {

class TimePercentage {
public:
  int time;
  float percentage = 0;
  int occurrences = 0;
  TimePercentage(int t);
};

class TimeObject {
public:
  Device *device;
  enums::Enum *userLevel;
  enums::Enum *action;
  int totalOcc = 0;
  vector<TimePercentage *> times = {new TimePercentage(enums::MORNING),
                                    new TimePercentage(enums::AFTERNOON),
                                    new TimePercentage(enums::NIGHT)};
  TimeObject(Device *d, enums::Enum *u, enums::Enum *a);
};

struct compareTObj {
  Request *key;
  compareTObj(Request *r);

  bool operator()(TimeObject *t);
};

struct compareTimes {
  int key;
  compareTimes(int t);

  bool operator()(TimePercentage *t);
};

class ContextComponent {
public:
  ConfigurationComponent *configurationComponent;
  AuditComponent *auditComponent;
  bool isTimeBuilding = true;
  time_t limitDate = (time_t)(-1);
  vector<TimeObject *> timeProbs;
  ContextComponent();
  ContextComponent(ConfigurationComponent *c, AuditComponent *a);

  // static trust calculation based on expected
  // for [DeviceClass x Action] and [UserLevel x Action]
  // from [AccessWay, Localization, Time, Age, Group]
  bool verifyContext(Request *req, time_t currentDate,
                     function<bool(Request *, time_t)> explicitAuthentication);

  // check if markov build time expired
  void checkBuilding(time_t currentDate);

  int calculateTrust(Context *context, User *user);
  void calculateTime(Request *req, time_t currentDate);

  void recalculateProbabilities(TimeObject *timeObj, int time);
};
} // namespace ns3

#endif