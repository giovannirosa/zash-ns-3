#ifndef AUDIT
#define AUDIT

#include <ctime>
#include <fstream>
#include <iostream>

#include "ns3/address-utils.h"
#include "ns3/address.h"
#include "ns3/simulator.h"
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

  // Metrics
  int adminNumber = 0;
  int criticalNumber = 0;
  int privacyRisk;

  double accessControlRT = 0.0;

  string simDate;

  string folderTraces;
  string deviceEnforcerFile;
  string scenarioSimFile;
  string messagesSimFile;
  string metricsSimFile;

  // Create a string stream to store simulation scenario data
  stringstream fileSim;
  // Create a string stream to store simulation messages data
  stringstream fileMsgs;

  AuditComponent(string timeLog, string folderTraces);

  /*
   * Print events
   * \param file name of the file
   * \param msg message to append
   */
  void printEvents(vector<AuditEvent *> events, char *currDateStr, string type);

  /*
   * Append file with given message
   * \param file name of the file
   * \param msg message to append
   */
  void appendFile(string file, string msg);

  /*
   * Device Enforcer callback to store messages exchanged between nodes and
   * server
   * \param path path of callback
   * \param sourceIp IP address from node
   * \param destinyIP IP address from server
   * \param message message exchanged
   */
  void deviceEnforcerCallback(string path, Ipv6Address sourceIp,
                              Ipv6Address destinyIP, string message);

  void outputMetrics();
};
} // namespace ns3

#endif