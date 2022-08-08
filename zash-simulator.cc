/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// Default Network topology, 29 nodes in a star connected to a router,
// and the router connected to a local server
/*
          n0 n1 n2...
           \ | /
            \|/
       s0---ap0
            /| \
           / | \
          n26 n27 n28
*/
// - Primary traffic goes from the nodes to the local server through the AP
// - The local server responds to the node through the AP

#include "sys/stat.h"
#include "sys/types.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "ns3/applications-module.h"
#include "ns3/bridge-helper.h"
#include "ns3/core-module.h"
#include "ns3/csma-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/zash-activity.h"
#include "ns3/zash-audit.h"
#include "ns3/zash-authorization.h"
#include "ns3/zash-configuration.h"
#include "ns3/zash-context.h"
#include "ns3/zash-data.h"
#include "ns3/zash-device-enforcer.h"
#include "ns3/zash-device.h"
#include "ns3/zash-enums.h"
#include "ns3/zash-models.h"
#include "ns3/zash-notification.h"
#include "ns3/zash-ontology.h"
#include "ns3/zash-server.h"
#include "ns3/zash-utils.h"

#define NUMBER_OF_DEVICES 29
#define ACTIVITY_COL 29
#define DATE_COL 30

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("ZASH");

/**
 * \class StackHelper
 * \brief Helper to set or get some IPv6 information about nodes.
 */
// class StackHelper {
// public:
//   /**
//    * \brief Add an address to a IPv6 node.
//    * \param n node
//    * \param interface interface index
//    * \param address IPv6 address to add
//    */
//   inline void AddAddress(Ptr<Node> &n, uint32_t interface,
//                          Ipv6Address address) {
//     Ptr<Ipv6> ipv6 = n->GetObject<Ipv6>();
//     ipv6->AddAddress(interface, address);
//   };

//   /**
//    * \brief Print the routing table.
//    * \param n the node
//    */
//   inline void PrintRoutingTable(Ptr<Node> &n) {
//     Ptr<Ipv6StaticRouting> routing = 0;
//     Ipv6StaticRoutingHelper routingHelper;
//     Ptr<Ipv6> ipv6 = n->GetObject<Ipv6>();
//     uint32_t nbRoutes = 0;
//     Ipv6RoutingTableEntry route;

//     routing = routingHelper.GetStaticRouting(ipv6);

//     std::cout << "Routing table of " << n << " : " << std::endl;
//     std::cout << "Destination\t"
//               << "Gateway\t"
//               << "Interface\t"
//               << "Prefix to use" << std::endl;

//     nbRoutes = routing->GetNRoutes();
//     for (uint32_t i = 0; i < nbRoutes; i++) {
//       route = routing->GetRoute(i);
//       std::cout << route.GetDest() << "\t" << route.GetGateway() << "\t"
//                 << route.GetInterface() << "\t" << route.GetPrefixToUse()
//                 << "\t" << std::endl;
//     }
//   }
// }

AuditComponent *createAudit() {
  string tracesFolder;
  string simDate = getTimeOfSimulationStart();

  tracesFolder = "zash_traces/";
  errno = 0;
  int dir = mkdir(tracesFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir < 0 && errno != EEXIST)
    cout << "Fail creating directory for traces!" << endl;

  tracesFolder.append(simDate + "/");

  // Creates a directory for specific simulation
  dir = mkdir(tracesFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir == -1)
    cout << "Fail creating sub directory for specific traces!" << dir << endl;

  NS_LOG_INFO(tracesFolder);
  // Audit Module
  AuditComponent *auditModule = new AuditComponent(simDate, tracesFolder);

  string messagesFolder = tracesFolder + "messages/";
  // Creates a directory for messages simulation
  dir = mkdir(messagesFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir == -1)
    cout << "Fail creating sub directory for messages traces!" << dir << endl;

  ostringstream convert;
  convert << tracesFolder.c_str() << "zash_simulation_scenario_"
          << auditModule->simDate << ".txt";
  auditModule->scenarioSimFile = convert.str();

  ostringstream convert2;
  convert2 << tracesFolder.c_str() << "zash_simulation_messages_"
           << auditModule->simDate << ".txt";
  auditModule->messagesSimFile = convert2.str();

  ostringstream convert3;
  convert3 << tracesFolder.c_str() << "zash_simulation_metrics_"
           << auditModule->simDate << ".txt";
  auditModule->metricsSimFile = convert3.str();

  // Save start seed in file
  // fileSim << "Start seed: " << seed << endl << endl;

  return auditModule;
}

DeviceComponent *buildServerStructure(AuditComponent *auditModule) {
  vector<User *> users = {
      new User(1, enums::UserLevel.at("ADMIN"), enums::Age.at("ADULT")),
      new User(2, enums::UserLevel.at("ADULT"), enums::Age.at("ADULT")),
      new User(3, enums::UserLevel.at("CHILD"), enums::Age.at("TEEN")),
      new User(4, enums::UserLevel.at("CHILD"), enums::Age.at("KID")),
      new User(5, enums::UserLevel.at("VISITOR"), enums::Age.at("ADULT"))};

  auditModule->fileSim << "Users of simulation are " << users.size() << ":"
                       << endl;
  for (User *user : users) {
    auditModule->fileSim << *user << endl;
    if (user->userLevel == enums::UserLevel.at("ADMIN")) {
      ++auditModule->adminNumber;
    }
  }

  vector<Device *> devices = {
      new Device(1, "Wardrobe", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, true), // wardrobe
      new Device(2, "TV", enums::DeviceClass.at("NONCRITICAL"),
                 enums::LIVINGROOM, true), // tv
      new Device(3, "Oven", enums::DeviceClass.at("CRITICAL"), enums::KITCHEN,
                 true), // oven
      new Device(4, "Office Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::OFFICE, true), // officeLight
      new Device(5, "Office Door Lock", enums::DeviceClass.at("CRITICAL"),
                 enums::OFFICE, true), // officeDoorLock
      new Device(6, "Office Door", enums::DeviceClass.at("NONCRITICAL"),
                 enums::OFFICE, true), // officeDoor
      new Device(7, "Office Carpet", enums::DeviceClass.at("NONCRITICAL"),
                 enums::OFFICE, false), // officeCarp
      new Device(8, "Office", enums::DeviceClass.at("NONCRITICAL"),
                 enums::OFFICE, false), // office
      new Device(9, "Main Door Lock", enums::DeviceClass.at("CRITICAL"),
                 enums::HOUSE, true), // mainDoorLock
      new Device(10, "Main Door", enums::DeviceClass.at("NONCRITICAL"),
                 enums::HOUSE, true), // mainDoor
      new Device(11, "Living Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::LIVINGROOM, true), // livingLight
      new Device(12, "Living Carpet", enums::DeviceClass.at("NONCRITICAL"),
                 enums::LIVINGROOM, false), // livingCarp
      new Device(13, "Kitchen Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::KITCHEN, true), // kitchenLight
      new Device(14, "Kitchen Door Lock", enums::DeviceClass.at("CRITICAL"),
                 enums::KITCHEN, true), // kitchenDoorLock
      new Device(15, "Kitchen Door", enums::DeviceClass.at("NONCRITICAL"),
                 enums::KITCHEN, true), // kitchenDoor
      new Device(16, "Kitchen Carpet", enums::DeviceClass.at("NONCRITICAL"),
                 enums::KITCHEN, false), // kitchenCarp
      new Device(17, "Hallway Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::HOUSE, true), // hallwayLight
      new Device(18, "Fridge", enums::DeviceClass.at("CRITICAL"),
                 enums::KITCHEN, true), // fridge
      new Device(19, "Couch", enums::DeviceClass.at("NONCRITICAL"),
                 enums::LIVINGROOM, false), // couch
      new Device(20, "Bedroom Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, true), // bedroomLight
      new Device(21, "Bedroom Door Lock", enums::DeviceClass.at("CRITICAL"),
                 enums::BEDROOM, true), // bedroomDoorLock
      new Device(22, "Bedroom Door", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, true), // bedroomDoor
      new Device(23, "Bedroom Carpet", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, false), // bedroomCarp
      new Device(24, "Bed Table Lamp", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, true), // bedTableLamp
      new Device(25, "Bed", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BEDROOM, false), // bed
      new Device(26, "Bathroom Light", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BATHROOM, true), // bathroomLight
      new Device(27, "Bathroom Door Lock", enums::DeviceClass.at("CRITICAL"),
                 enums::BATHROOM, true), // bathroomDoorLock
      new Device(28, "Bathroom Door", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BATHROOM, true), // bathroomDoor
      new Device(29, "Bathroom Carpet", enums::DeviceClass.at("NONCRITICAL"),
                 enums::BATHROOM, false) // bathroomCarp
  };

  auditModule->fileSim << "Devices of simulation are " << devices.size() << ":"
                       << endl;
  for (Device *device : devices) {
    auditModule->fileSim << *device << endl;
    if (device->deviceClass == enums::DeviceClass.at("CRITICAL")) {
      ++auditModule->criticalNumber;
    }
  }

  auditModule->minDeviceNumber = devices.size();
  auditModule->maxDeviceNumber = devices.size();
  auditModule->deviceExtensibility = 0;

  auditModule->privacyRisk =
      auditModule->adminNumber * auditModule->criticalNumber;

  vector<enums::Enum *> visitorCriticalCap = {};
  Ontology *visitorCritical =
      new Ontology(enums::UserLevel.at("VISITOR"),
                   enums::DeviceClass.at("CRITICAL"), visitorCriticalCap);

  vector<enums::Enum *> childCriticalCap = {enums::Action.at("VIEW")};
  childCriticalCap.insert(childCriticalCap.end(), visitorCriticalCap.begin(),
                          visitorCriticalCap.end());
  Ontology *childCritical =
      new Ontology(enums::UserLevel.at("CHILD"),
                   enums::DeviceClass.at("CRITICAL"), childCriticalCap);

  vector<enums::Enum *> adultCriticalCap = {enums::Action.at("CONTROL")};
  adultCriticalCap.insert(adultCriticalCap.end(), childCriticalCap.begin(),
                          childCriticalCap.end());
  Ontology *adultCritical =
      new Ontology(enums::UserLevel.at("ADULT"),
                   enums::DeviceClass.at("CRITICAL"), adultCriticalCap);

  vector<enums::Enum *> adminCriticalCap = {enums::Action.at("MANAGE")};
  adminCriticalCap.insert(adminCriticalCap.end(), adultCriticalCap.begin(),
                          adultCriticalCap.end());
  Ontology *adminCritical =
      new Ontology(enums::UserLevel.at("ADMIN"),
                   enums::DeviceClass.at("CRITICAL"), adminCriticalCap);

  vector<enums::Enum *> visitorNonCriticalCap = {enums::Action.at("VIEW"),
                                                 enums::Action.at("CONTROL")};
  Ontology *visitorNonCritical =
      new Ontology(enums::UserLevel.at("VISITOR"),
                   enums::DeviceClass.at("NONCRITICAL"), visitorNonCriticalCap);

  vector<enums::Enum *> childNonCriticalCap = {};
  childNonCriticalCap.insert(childNonCriticalCap.end(),
                             visitorNonCriticalCap.begin(),
                             visitorNonCriticalCap.end());
  Ontology *childNonCritical =
      new Ontology(enums::UserLevel.at("CHILD"),
                   enums::DeviceClass.at("NONCRITICAL"), childNonCriticalCap);

  vector<enums::Enum *> adultNonCriticalCap = {enums::Action.at("MANAGE")};
  adultNonCriticalCap.insert(adultNonCriticalCap.end(),
                             childNonCriticalCap.begin(),
                             childNonCriticalCap.end());
  Ontology *adultNonCritical =
      new Ontology(enums::UserLevel.at("ADULT"),
                   enums::DeviceClass.at("NONCRITICAL"), adultNonCriticalCap);

  vector<enums::Enum *> adminNonCriticalCap = {};
  adminNonCriticalCap.insert(adminNonCriticalCap.end(),
                             adultNonCriticalCap.begin(),
                             adultNonCriticalCap.end());
  Ontology *adminNonCritical =
      new Ontology(enums::UserLevel.at("ADMIN"),
                   enums::DeviceClass.at("NONCRITICAL"), adminNonCriticalCap);

  vector<Ontology *> ontologies = {
      visitorCritical,    childCritical,    adultCritical,    adminCritical,
      visitorNonCritical, childNonCritical, adultNonCritical, adminNonCritical};

  auditModule->fileSim << "Ontologies of simulation are " << ontologies.size()
                       << ":" << endl;
  for (Ontology *ontology : ontologies) {
    auditModule->fileSim << *ontology << endl;
  }

  auditModule->userLevelNumber = enums::UserLevel.size();
  auditModule->deviceClassNumber = enums::DeviceClass.size();
  auditModule->actionNumber = enums::Action.size();

  auditModule->resourceIsolation = auditModule->userLevelNumber *
                                   auditModule->deviceClassNumber *
                                   auditModule->actionNumber;

  // Behavior Module
  int blockThreshold = 3;
  int blockInterval = 24;
  int buildInterval = 32;
  ConfigurationComponent *configurationComponent =
      new ConfigurationComponent(blockThreshold, blockInterval, buildInterval,
                                 devices, users, ontologies, auditModule);
  auditModule->fileSim << "Other configuration parameters of simulation are:"
                       << endl;
  auditModule->fileSim << "Block Threshold: " << blockThreshold << endl;
  auditModule->fileSim << "Block Interval: " << blockInterval << endl;
  auditModule->fileSim << "Build Interval: " << buildInterval << endl;
  NotificationComponent *notificationComponent =
      new NotificationComponent(configurationComponent);

  // Collection Module
  DataComponent *dataComponent = new DataComponent();

  // Decision Module
  OntologyComponent *ontologyComponent =
      new OntologyComponent(configurationComponent, auditModule);
  ContextComponent *contextComponent =
      new ContextComponent(configurationComponent, auditModule);
  ActivityComponent *activityComponent =
      new ActivityComponent(dataComponent, configurationComponent, auditModule);
  AuthorizationComponent *authorizationComponent = new AuthorizationComponent(
      configurationComponent, ontologyComponent, contextComponent,
      activityComponent, notificationComponent, auditModule);

  DeviceComponent *deviceComponent =
      new DeviceComponent(authorizationComponent, dataComponent, auditModule);

  return deviceComponent;
};

void appsConfiguration(Ipv6InterfaceContainer serverApInterface,
                       DeviceComponent *deviceComponent, double start,
                       double stop, NodeContainer serverNode,
                       NodeContainer staNodes,
                       Ipv6InterfaceContainer staInterface, string dataRate,
                       vector<Device *> devices, AuditComponent *auditModule) {
  // Create a server to receive these packets
  // Start at 0s
  // Stop at final
  Address serverAddress(
      Inet6SocketAddress(serverApInterface.GetAddress(0, 0), 50000));
  Ptr<ZashServer> ZashServerApp = CreateObject<ZashServer>();
  ZashServerApp->SetAttribute("Protocol",
                              TypeIdValue(TcpSocketFactory::GetTypeId()));
  ZashServerApp->SetAttribute("Local", AddressValue(serverAddress));
  ZashServerApp->SetDeviceComponent(deviceComponent);
  ZashServerApp->SetStartTime(Seconds(start));
  ZashServerApp->SetStopTime(Seconds(stop));
  serverNode.Get(0)->AddApplication(ZashServerApp);

  // Start at 5s
  // Each start 0.2s apart
  // Stop at final
  double startDevice = start + 5.0;
  for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
    if (i == 7) {
      continue;
    }

    Address nodeAddress(
        Inet6SocketAddress(staInterface.GetAddress(i, 0), 50000));
    Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp =
        CreateObject<ZashDeviceEnforcer>();
    ZashDeviceEnforcerApp->SetAttribute(
        "Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
    ZashDeviceEnforcerApp->SetAttribute("Local", AddressValue(nodeAddress));
    ZashDeviceEnforcerApp->SetAttribute("Remote", AddressValue(serverAddress));
    ZashDeviceEnforcerApp->SetAttribute("DataRate",
                                        DataRateValue(DataRate(dataRate)));
    ZashDeviceEnforcerApp->SetDevice(devices[i]);
    ZashDeviceEnforcerApp->SetAuditModule(auditModule);
    ZashDeviceEnforcerApp->SetStartTime(Seconds(startDevice));
    ZashDeviceEnforcerApp->SetStopTime(Seconds(stop));
    startDevice += 0.2;

    Ptr<Node> node = staNodes.Get(i);
    node->AddApplication(ZashDeviceEnforcerApp);
    NS_LOG_INFO("Installed device " << i);
  }
}

void scheduleMessages(NodeContainer staNodes, vector<Device *> devices,
                      DataComponent *dataComponent, AuditComponent *auditModule,
                      int startDay, int endDay) {
  int user = 0;
  string accessWay = "PERSONAL";
  string localization = "INTERNAL";
  string group = "ALONE";
  string action = "CONTROL";
  auditModule->fileSim << "Devices interaction have the following properties: "
                       << endl;
  auditModule->fileSim << "User: " << user << endl;
  auditModule->fileSim << "Access Way: " << accessWay << endl;
  auditModule->fileSim << "Localization: " << localization << endl;
  auditModule->fileSim << "Group: " << group << endl;
  auditModule->fileSim << "Action: " << action << endl;

  int idReq = 0;

  NS_LOG_INFO("Opening dataset...");
  string dataset = "data/d6_2m_0tm.csv";
  auditModule->fileMsgs << "Dataset is: " << dataset << endl;
  CsvReader csv(dataset);
  vector<int> lastState;
  time_t firstDate = (time_t)(-1);
  int msgCount = 0;
  int dayCount = 1;
  time_t lastDate;
  while (csv.FetchNextRow()) {
    // NS_LOG_INFO("Processing row " << csv.RowNumber() << "...");
    auditModule->fileMsgs << "Processing row " << csv.RowNumber() << "..."
                          << endl;
    // Ignore blank lines and header
    if (csv.RowNumber() == 1 || csv.IsBlankRow()) {
      continue;
    }

    // Expecting vector
    vector<int> currentState(NUMBER_OF_DEVICES);
    bool ok = true;
    for (size_t i = 0; i < currentState.size(); ++i) {
      ok |= csv.GetValue(i, currentState[i]);
    }
    if (!ok) {
      // Handle error, then
      NS_LOG_ERROR("Error parsing csv file, row number: " + csv.RowNumber());
      continue;
    }

    string timeStr;
    csv.GetValue(DATE_COL, timeStr);
    time_t currentDate = strToTime(timeStr.c_str());

    if (difftime(firstDate, (time_t)(-1)) == 0) {
      firstDate = currentDate - 10;
    } else if (extractDay(currentDate) != extractDay(lastDate)) {
      ++dayCount;
    }

    string actStr;
    csv.GetValue(ACTIVITY_COL, actStr);

    // NS_LOG_INFO(vecToStr(currentState));
    auditModule->fileMsgs << vecToStr(currentState) << endl;

    if (currentState == lastState) {
      continue;
    }

    if (lastState.size() > 0 && dayCount >= startDay && dayCount <= endDay) {
      vector<uint32_t> changes;

      for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i) {
        if (lastState[i] != currentState[i]) {
          changes.push_back(i);
        }
      }

      double diff = difftime(currentDate, firstDate);
      for (uint32_t change : changes) {
        if (change > staNodes.GetN() - 1 || change == 7) {
          continue;
        }
        // NS_LOG_INFO(formatTime(currentDate) + " - " + actStr);
        auditModule->fileMsgs << formatTime(currentDate) << " - " << actStr
                              << endl;

        string request = "[" + to_string(++idReq) + "," + to_string(change) +
                         "," + to_string(user) + "," + accessWay + "," +
                         localization + "," + group + "," + action + "," +
                         formatTime(currentDate) + "]";
        // NS_LOG_INFO("Device enforcer scheduled with message = " + request);
        auditModule->fileMsgs
            << "Device enforcer scheduled with message = " << request << endl;
        Ptr<Node> node = staNodes.Get(change);
        Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp =
            DynamicCast<ZashDeviceEnforcer>(node->GetApplication(0));
        Simulator::Schedule(Seconds(diff), &ZashDeviceEnforcer::StartSending,
                            ZashDeviceEnforcerApp, request);
        msgCount++;
        // NS_LOG_INFO(devices[change]->name << " will change at " << diff
        //                                   << " seconds");
        auditModule->fileMsgs << devices[change]->name << " will change at "
                              << diff << " seconds" << endl;
        diff += 0.2;
      }
    } else {
      dataComponent->lastState = currentState;
    }
    lastState = currentState;
    lastDate = currentDate;
  }

  auditModule->fileMsgs << "Count of messages = " << msgCount << endl;
  auditModule->fileMsgs << "Count of days = " << dayCount << endl;
};

int main(int argc, char *argv[]) {
  //----------------------------------------------------------------------------------
  // Simulation logs
  //----------------------------------------------------------------------------------
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  LogComponentEnable("ZASH", LOG_LEVEL_ALL);
  LogComponentEnable("ZashDeviceEnforcer", LOG_LEVEL_ALL);
  LogComponentEnable("ZashServer", LOG_LEVEL_ALL);
  // LogComponentEnable("ArpL3Protocol", LOG_LEVEL_INFO);
  LogComponentEnable("AuditComponent", LOG_LEVEL_INFO);

  //----------------------------------------------------------------------------------
  // Simulation variables
  //----------------------------------------------------------------------------------
  // Set up some default values for the simulation.
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents",
                     BooleanValue(true));
  double start = 0.0;
  // double stop = 86400.0;
  double stop = 200.0;
  uint32_t N = NUMBER_OF_DEVICES; // number of nodes in the star
  uint32_t payloadSize = 1448;    /* Transport layer payload size in bytes. */
  string dataRate = "100Mbps";    /* Application layer datarate. */
  string phyRate = "HtMcs7";      /* Physical layer bitrate. */
  double simulationTime = stop;   /* Simulation time in seconds. */
  bool pcapTracing = false;       /* PCAP Tracing is enabled or not. */
  uint32_t startDay = 1;          /* Start day of the simulation. */
  uint32_t endDay = 2;            /* End day of the simulation. */

  // Allow the user to override any of the defaults and the above
  // Config::SetDefault()s at run-time, via command-line arguments
  CommandLine cmd(__FILE__);
  cmd.AddValue("nNodes", "Number of nodes to place in the star", N);
  cmd.AddValue("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue("dataRate", "Application data ate", dataRate);
  cmd.AddValue("phyRate", "Physical layer bitrate", phyRate);
  cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue("pcap", "Enable/disable PCAP Tracing", pcapTracing);
  cmd.AddValue("start", "Start day of the simulation", startDay);
  cmd.AddValue("end", "End day of the simulation", endDay);
  cmd.Parse(argc, argv);

  if (startDay < 1) {
    cout << "ZASH - Error: start must be greater than 0" << endl;
    return 1;
  }
  if (startDay > endDay) {
    cout << "ZASH - Error: start must be greater than end" << endl;
    return 1;
  }

  // Config::SetDefault("ns3::TcpL4Protocol::SocketType",
  //                    TypeIdValue(TypeId::LookupByName("ns3::TcpNewReno")));

  /* Configure TCP Options */
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

  // Config::SetDefault("ns3::ArpCache::DeadTimeout",
  //                    TimeValue(MilliSeconds(500)));
  // Config::SetDefault("ns3::ArpCache::WaitReplyTimeout",
  //                    TimeValue(MilliSeconds(200)));
  // Config::SetDefault("ns3::ArpCache::MaxRetries", UintegerValue(10));
  // Config::SetDefault("ns3::ArpCache::PendingQueueSize",
  //                    UintegerValue(NUMBER_OF_DEVICES));

  //----------------------------------------------------------------------------------
  // Create a directory for traces (zash_traces) inside ns3 directory
  //----------------------------------------------------------------------------------

  AuditComponent *auditModule = createAudit();

  //----------------------------------------------------------------------------------
  // Topology configuration
  //----------------------------------------------------------------------------------

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  WifiStandard wifiStandard = WIFI_STANDARD_80211n_2_4GHZ;
  wifiHelper.SetStandard(wifiStandard);
  Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss",
                     DoubleValue(40.046));
  auditModule->fileSim << "Wifi standard is " << wifiStandard << endl;

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  // wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  // wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
  // "Frequency",
  //                                DoubleValue(5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel(wifiChannel.Create());
  // Set MIMO capabilities
  // wifiPhy.Set("Antennas", UintegerValue(4));
  // wifiPhy.Set("MaxSupportedTxSpatialStreams", UintegerValue(4));
  // wifiPhy.Set("MaxSupportedRxSpatialStreams", UintegerValue(4));
  // wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
  // wifiHelper.SetRemoteStationManager("ns3::AarfWifiManager");
  // wifiHelper.SetRemoteStationManager("ns3::IdealWifiManager");
  wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                                     StringValue(phyRate), "ControlMode",
                                     StringValue("ErpOfdmRate24Mbps"));

  // Here, we will create N nodes in a star.
  NS_LOG_INFO("Create nodes.");
  uint32_t nServers = 1;
  uint32_t nAps = 1;
  NodeContainer serverNode;
  serverNode.Create(nServers);
  NodeContainer apNode;
  apNode.Create(nAps);
  NodeContainer staNodes;
  staNodes.Create(N);
  NodeContainer allNodes = NodeContainer(serverNode, apNode, staNodes);
  auditModule->fileSim << "Number of servers is " << nServers << endl;
  auditModule->fileSim << "Number of APs is " << nAps << endl;
  auditModule->fileSim << "Number of STAs is " << N << endl;

  NodeContainer serverAp = NodeContainer(serverNode.Get(0), apNode.Get(0));
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma.SetChannelAttribute("Delay", StringValue("1ms"));
  NetDeviceContainer serverApDevice = csma.Install(serverAp);

  /* Configure AP */
  NS_LOG_INFO("Configure AP");
  Ssid ssid = Ssid("network");
  wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid), "BeaconInterval",
                  TimeValue(MicroSeconds(4096000)), "BsrLifetime",
                  TimeValue(MilliSeconds(800)));

  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install(wifiPhy, wifiMac, apNode);

  /* Configure STA */
  NS_LOG_INFO("Configure STA");
  wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid),
                  "WaitBeaconTimeout", TimeValue(MilliSeconds(4800)),
                  "AssocRequestTimeout", TimeValue(Seconds(20)));

  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install(wifiPhy, wifiMac, staNodes);

  Config::Set("/NodeList//DeviceList//$ns3::WifiNetDevice/HtConfiguration/"
              "ShortGuardIntervalSupported",
              BooleanValue(true));

  NS_LOG_INFO("Configure Bridge");
  BridgeHelper bridge;
  NetDeviceContainer bridgeDev;
  bridgeDev =
      bridge.Install(apNode.Get(0), NetDeviceContainer(apDevice.Get(0),
                                                       serverApDevice.Get(1)));

  /* Mobility model */
  NS_LOG_INFO("Configure mobility");
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc =
      CreateObject<ListPositionAllocator>();
  positionAlloc->Add("data/positions.csv");

  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(allNodes);

  // Install network stacks on the nodes
  InternetStackHelper internet;
  internet.Install(allNodes);

  // Later, we add IP addresses.
  NS_LOG_INFO("Assign IP Addresses.");
  Ipv6AddressHelper ipv6;
  ipv6.SetBase(Ipv6Address("2001:1::"), Ipv6Prefix(64));
  Ipv6InterfaceContainer serverApInterface = ipv6.Assign(serverApDevice);
  serverApInterface.SetForwarding(1, true);
  serverApInterface.SetDefaultRouteInAllNodes(1);
  auditModule->fileSim << "Server IP is " << serverApInterface.GetAddress(0, 0)
                       << endl;
  Ipv6InterfaceContainer apInterface = ipv6.Assign(bridgeDev);
  apInterface.SetForwarding(0, true);
  apInterface.SetDefaultRouteInAllNodes(0);
  auditModule->fileSim << "AP IP is " << apInterface.GetAddress(0, 0) << endl;
  Ipv6InterfaceContainer staInterface = ipv6.Assign(staDevices);
  staInterface.SetDefaultRouteInAllNodes(apInterface.GetAddress(0, 0));
  auditModule->fileSim << "STA IP start with " << staInterface.GetAddress(0, 0)
                       << endl;

  // Turn on global static routing
  // Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Ptr<Node> n0 = apNode.Get(0);

  // StackHelper stackHelper;
  // stackHelper.PrintRoutingTable(n0);

  //----------------------------------------------------------------------------------
  // ZASH Application Logic
  //----------------------------------------------------------------------------------

  DeviceComponent *deviceComponent = buildServerStructure(auditModule);

  // NS_LOG_INFO(deviceComponent);

  vector<Device *> devices =
      deviceComponent->authorizationComponent->configurationComponent->devices;
  DataComponent *dataComponent = deviceComponent->dataComponent;

  //----------------------------------------------------------------------------------
  // Applications configuration
  //----------------------------------------------------------------------------------

  appsConfiguration(serverApInterface, deviceComponent, start, stop, serverNode,
                    staNodes, staInterface, dataRate, devices, auditModule);

  //----------------------------------------------------------------------------------
  // Schedule messages from dataset
  //----------------------------------------------------------------------------------

  scheduleMessages(staNodes, devices, dataComponent, auditModule, startDay,
                   endDay);

  //----------------------------------------------------------------------------------
  // Output configuration
  //----------------------------------------------------------------------------------

  createFile(auditModule->scenarioSimFile, auditModule->simDate,
             auditModule->fileSim.str());
  createFile(auditModule->messagesSimFile, auditModule->simDate,
             auditModule->fileMsgs.str());

  // Callback Trace to Collect Messages in Device Enforcer Application
  for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
    if (i == 7) {
      continue;
    }
    Ptr<Node> node = staNodes.Get(i);
    Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp =
        DynamicCast<ZashDeviceEnforcer>(node->GetApplication(0));
    ostringstream paramTest;
    paramTest << "/NodeList/" << (ZashDeviceEnforcerApp->z_device->id)
              << "/ApplicationList/*/$ns3::ZashDeviceEnforcer/Traces";
    ZashDeviceEnforcerApp->m_traces.Connect(
        MakeCallback(&AuditComponent::deviceEnforcerCallback, auditModule),
        paramTest.str());
    // Config::Connect(
    //     paramTest.str(),
    //     MakeCallback(&AuditComponent::deviceEnforcerCallback,
    //     auditComponent));
  }

  /* Enable Traces */
  if (pcapTracing) {
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.EnablePcap("AccessPoint", apDevice);
    wifiPhy.EnablePcap("Station", staDevices);
    csma.EnablePcap("Server", serverApDevice);
  }

  // configure tracing
  // AsciiTraceHelper ascii;
  // mobility.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));
  // csma.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));

  /* Stop Simulation */
  Simulator::Stop(Seconds(simulationTime + 1));

  // 40mx28m
  AnimationInterface anim("animation.xml");
  anim.SetBackgroundImage(
      "/home/grosa/Dev/ns-allinone-3.35/ns-3.35/data/home-design.png", 0, 0,
      0.07, 0.07, 1.0);
  for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
    Ptr<Node> node = staNodes.Get(i);
    anim.UpdateNodeDescription(node, devices[i]->name); // Optional
    anim.UpdateNodeColor(node, 255, 0, 0);              // Optional
    anim.UpdateNodeSize(node->GetId(), 0.8, 0.8);
  }
  for (uint32_t i = 0; i < apNode.GetN(); ++i) {
    Ptr<Node> node = apNode.Get(i);
    anim.UpdateNodeDescription(node, "AP"); // Optional
    anim.UpdateNodeColor(node, 0, 255, 0);  // Optional
    anim.UpdateNodeSize(node->GetId(), 0.8, 0.8);
  }
  for (uint32_t i = 0; i < serverNode.GetN(); ++i) {
    Ptr<Node> node = serverNode.Get(i);
    anim.UpdateNodeDescription(node, "Local Server"); // Optional
    anim.UpdateNodeColor(node, 0, 0, 255);            // Optional
    anim.UpdateNodeSize(node->GetId(), 1.2, 1.2);
  }
  anim.EnablePacketMetadata();
  anim.EnableIpv4RouteTracking("zash.txt", Seconds(0), Seconds(200),
                               Seconds(5));

  // Flow monitor
  // Ptr<FlowMonitor> flowMonitor;
  // FlowMonitorHelper flowHelper;
  // flowMonitor = flowHelper.InstallAll();

  // Trace routing tables
  // Ipv4GlobalRoutingHelper g;
  // Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(
  //     "dynamic-global-routing.routes", std::ios::out);
  // g.PrintRoutingTableAllAt(Seconds(12), routingStream);

  //----------------------------------------------------------------------------------
  // Call simulation
  //----------------------------------------------------------------------------------

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  // flowMonitor->SerializeToXmlFile("flow.xml", true, true);

  auditModule->outputMetrics();

  return 0;
}
