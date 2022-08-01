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

void AuditComponent::deviceEnforcerCallback(string path, Address sourceIp,
                                            Address destinyIP, string message) {
  stringstream sourceIpStr;
  stringstream destinyIpStr;
  if (InetSocketAddress::IsMatchingType(sourceIp)) {
    sourceIpStr << InetSocketAddress::ConvertFrom(sourceIp).GetIpv4();
    destinyIpStr << InetSocketAddress::ConvertFrom(destinyIP).GetIpv4();
  } else {
    sourceIpStr << Inet6SocketAddress::ConvertFrom(sourceIp).GetIpv6();
    destinyIpStr << Inet6SocketAddress::ConvertFrom(destinyIP).GetIpv6();
  }
  double timeReceived = Simulator::Now().GetSeconds();
  ostringstream msg;
  msg << timeReceived << "\t" << sourceIpStr.str() << "\t" << destinyIpStr.str() << "\t"
      << message << endl;

  // // Save all emergency messages sent in only one file
  // appendFile(deviceEnforcerStream, deviceEnforcerFile, msg.str());

  ostringstream convert;
  convert << folderTraces.c_str() << "messages/messages_" << sourceIpStr.str() << ".txt";

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

  fileSimRec << "Access Control Response Time (ACRT) = " << accessControlRT
             << " ms" << endl
             << endl;

  fileSimRec << "Access Control Distance (ACD) = " << accessControlDistance
             << " hops" << endl
             << endl;

  fileSimRec << "Spatial and Temporal Locality (STL) = "
             << spatialTemporalLocality << endl
             << endl;

  fileSimRec << "Number of user levels (|UL|) = " << userLevelNumber << endl;
  fileSimRec << "Number of device classes (|DC|) = " << deviceClassNumber
             << endl;
  fileSimRec << "Number of actions (|A|) = " << actionNumber << endl;
  fileSimRec << "Resources Isolation (RI) = " << resourceIsolation << endl
             << endl;

  fileSimRec << "Maximum number of devices (Dmax) = " << maxDeviceNumber
             << endl;
  fileSimRec << "Minimum number of devices (Dmin) = " << minDeviceNumber
             << endl;
  fileSimRec << "Device Extensibility (DE) = " << deviceExtensibility << endl
             << endl;

  fileSimRec << "Number of requests of requested access way (|RAC|) = "
             << requestedAccessWayNumber << endl;
  fileSimRec << "Number of requests of home access way (|HAC|) = "
             << homeAccessWayNumber << endl;
  fileSimRec << "Number of requests of personal access way (|PAC|) = "
             << personalAccessWayNumber << endl;
  fileSimRec << "Number of requests without intermediaries (|NI|) = "
             << reqWithNoIntermediaryNumber << endl;
  fileSimRec << "Number of requests with intermediaries (|I|) = "
             << reqWithIntermediaryNumber << endl;
  fileSimRec << "Access Control Enforcement (ACE) = "
             << accessControlEnforcement << endl
             << endl;

  // Close scenario simulation configuration file
  fileSimRec << "**** End of ZASH file ****" << endl;
  fileSimRec.close();
}

void AuditComponent::storeRequestMetrics(Request *req) {
  if (req->context->accessWay == enums::AccessWay.at("REQUESTED")) {
    ++requestedAccessWayNumber;
    ++reqWithNoIntermediaryNumber;
  } else if (req->context->accessWay == enums::AccessWay.at("HOUSE")) {
    ++homeAccessWayNumber;
    ++reqWithIntermediaryNumber;
  } else {
    ++personalAccessWayNumber;
    ++reqWithIntermediaryNumber;
  }

  accessControlEnforcement =
      reqWithNoIntermediaryNumber / reqWithIntermediaryNumber;
}

} // namespace ns3