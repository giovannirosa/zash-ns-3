#ifndef AUDIT
#define AUDIT

#include <ctime>
#include <iostream>
#include <fstream>

#include "ns3/zash-models.h"
#include "ns3/zash-utils.h"

using namespace std;

namespace ns3 {

class AuditEvent {
public:
  time_t time;
  Request *request;
  AuditEvent(time_t t, Request *r);

  friend ostream &operator<<(ostream &out, AuditEvent const &a) {
    printFormattedTime(a.time, out);
    out << " - " << *a.request << endl;
    return out;
  }
};

class AuditComponent {
public:
  vector<AuditEvent *> ontologyFail;
  vector<AuditEvent *> contextFail;
  vector<AuditEvent *> activityFail;
  vector<AuditEvent *> blocks;
  vector<AuditEvent *> attacks;
  vector<AuditEvent *> validProofs;
  vector<AuditEvent *> invalidProofs;
  int reqNumber = 0;
  int reqGranted = 0;
  int reqDenied = 0;
  AuditComponent();

  void printEvents(vector<AuditEvent *> events, char *currDateStr, string type);
};
} // namespace ns3

#endif