#include "zash-audit.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AuditComponent");

AuditEvent::AuditEvent(time_t t, Request *r) {
  time = t;
  request = r;
}

AuditComponent::AuditComponent(string timeLog, string ft) {
  folderTraces = ft;
  simDate = timeLog;
  // Define and assign log files names
  ostringstream convert;
  convert << folderTraces.c_str() << "zash_device_enforcer_traces_" << simDate
          << ".txt";
  deviceEnforcerFile = convert.str();
}

inline void AuditComponent::printEvents(vector<AuditEvent *> events,
                                        char *currDateStr, string type) {
  ofstream outfile(type + "_" + string(currDateStr) + ".txt");
  for (AuditEvent *event : events) {
    outfile << *event << std::endl;
  }
  outfile.close();
}

void AuditComponent::appendFile(string file, string msg) {
  ofstream stream;
  stream.open(file, ios::out | ios::app);
  stream << msg;
  stream.close();
}

void AuditComponent::deviceEnforcerCallback(string path, Ipv6Address sourceIp,
                                            Ipv6Address destinyIP,
                                            string message) {
  double timeReceived = Simulator::Now().GetSeconds();
  ostringstream msg;
  msg << timeReceived << "\t" << sourceIp << "\t" << destinyIP << "\t"
      << message << endl;

  // // Save all emergency messages sent in only one file
  // appendFile(deviceEnforcerStream, deviceEnforcerFile, msg.str());

  ostringstream convert;
  convert << folderTraces.c_str() << "messages/messages_" << sourceIp << ".txt";

  // Save received messages individually by IP address
  appendFile(convert.str(), msg.str());
}

void AuditComponent::outputMetrics() {
  ofstream fileSimRec;
  fileSimRec.open(metricsSimFile);
  fileSimRec << "**** ZASH file ****" << endl << endl;
  fileSimRec << "Date: " << simDate << endl << endl;

  fileSimRec << "Number of admin users (|AU|) = " << adminNumber << endl;
  fileSimRec << "Number of critical devices (|CD|) = " << criticalNumber
             << endl;
  fileSimRec << "Privacy Risk (PR) = " << privacyRisk << endl << endl;

  fileSimRec << "Access Control Response Time (ACRT) = " << accessControlRT << endl << endl;

  // Close scenario simulation configuration file
  fileSimRec << "**** End of ZASH file ****" << endl;
  fileSimRec.close();
}

} // namespace ns3