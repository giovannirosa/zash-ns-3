#ifndef AUDIT
#define AUDIT

#include <ctime>
#include <fstream>
#include <iostream>

#include "ns3/address-utils.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
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
    out << formatTime(a.time) << " - " << *a.request << endl;
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

  int userLevelNumber = 0;
  int deviceClassNumber = 0;
  int actionNumber = 0;
  int resourceIsolation;

  uint32_t minDeviceNumber = 0;
  uint32_t maxDeviceNumber = 0;
  uint32_t deviceExtensibility;

  int requestedAccessWayNumber = 0;
  int homeAccessWayNumber = 0;
  int personalAccessWayNumber = 0;
  int reqWithNoIntermediaryNumber = 0;
  int reqWithIntermediaryNumber = 0;
  double accessControlEnforcement;

  int accessControlDistance = 2;
  double spatialTemporalLocality;

  string simDate;

  string folderTraces;
  string deviceEnforcerFile;
  string scenarioSimFile;
  string messagesSimFile;
  string metricsSimFile;
  string execSimFile;
  string logSimFile;

  // Create a string stream to store simulation scenario data
  stringstream fileSim;
  // Create a string stream to store simulation messages data
  stringstream fileMsgs;
  // Create a string stream to store simulated executions
  stringstream fileExec;
  // Create a string stream to store main simulation log
  stringstream fileLog;
  // Create a string stream to store zash output
  stringstream *zashOutput = &fileLog;

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
  void deviceEnforcerCallback(string path, Address sourceIp, Address destinyIP,
                              string message);

  void outputMetrics();

  void storeRequestMetrics(Request *req);
};
} // namespace ns3

#endif