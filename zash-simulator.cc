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

NS_LOG_COMPONENT_DEFINE ("ZASH");

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

//     cout << "Routing table of " << n << " : " << endl;
//     cout << "Destination\t"
//               << "Gateway\t"
//               << "Interface\t"
//               << "Prefix to use" << endl;

//     nbRoutes = routing->GetNRoutes();
//     for (uint32_t i = 0; i < nbRoutes; i++) {
//       route = routing->GetRoute(i);
//       cout << route.GetDest() << "\t" << route.GetGateway() << "\t"
//                 << route.GetInterface() << "\t" << route.GetPrefixToUse()
//                 << "\t" << endl;
//     }
//   }
// }

AuditComponent *
createAudit ()
{
  string tracesFolder;
  string simDate = getTimeOfSimulationStart ();

  tracesFolder = "zash_traces/";
  errno = 0;
  int dir = mkdir (tracesFolder.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir < 0 && errno != EEXIST)
    cout << "Fail creating directory for traces!" << endl;

  tracesFolder.append (simDate + "/");

  // Creates a directory for specific simulation
  dir = mkdir (tracesFolder.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir == -1)
    cout << "Fail creating sub directory for specific traces!" << dir << endl;

  // auditModule->fileLog << tracesFolder < endl;
  // Audit Module
  AuditComponent *auditModule = new AuditComponent (simDate, tracesFolder);

  string messagesFolder = tracesFolder + "messages/";
  // Creates a directory for messages simulation
  dir = mkdir (messagesFolder.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (dir == -1)
    cout << "Fail creating sub directory for messages traces!" << dir << endl;

  ostringstream convert;
  convert << tracesFolder.c_str () << "zash_simulation_scenario_" << auditModule->simDate << ".txt";
  auditModule->scenarioSimFile = convert.str ();

  ostringstream convert2;
  convert2 << tracesFolder.c_str () << "zash_simulation_messages_" << auditModule->simDate
           << ".txt";
  auditModule->messagesSimFile = convert2.str ();

  ostringstream convert3;
  convert3 << tracesFolder.c_str () << "zash_simulation_metrics_" << auditModule->simDate << ".txt";
  auditModule->metricsSimFile = convert3.str ();

  ostringstream convert4;
  convert4 << tracesFolder.c_str () << "zash_simulation_simulated_" << auditModule->simDate
           << ".txt";
  auditModule->execSimFile = convert4.str ();

  ostringstream convert5;
  convert5 << tracesFolder.c_str () << "zash_log_" << auditModule->simDate << ".txt";
  auditModule->logSimFile = convert5.str ();

  // Save start seed in file
  // fileSim << "Start seed: " << seed << endl << endl;

  return auditModule;
}

void
printEnums (enums::Properties *props, AuditComponent *auditModule)
{
  auditModule->fileSim << "Enums are:" << endl << endl;
  auditModule->fileSim << "Action:" << endl;
  for (auto const &a : props->Action)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "UserLevel:" << endl;
  for (auto const &a : props->UserLevel)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "DeviceClass:" << endl;
  for (auto const &a : props->DeviceClass)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "AccessWay:" << endl;
  for (auto const &a : props->AccessWay)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "Localization:" << endl;
  for (auto const &a : props->Localization)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "TimeClass:" << endl;
  for (auto const &a : props->TimeClass)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "Age:" << endl;
  for (auto const &a : props->Age)
    {
      auditModule->fileSim << *a.second << endl;
    }

  auditModule->fileSim << endl << "Group:" << endl;
  for (auto const &a : props->Group)
    {
      auditModule->fileSim << *a.second << endl;
    }
}

enums::Properties *
buildEnums (AuditComponent *auditModule, string mode)
{
  string enumsConfig;
  if (mode == "H")
    {
      enumsConfig = "data/enums_hard.csv";
    }
  else if (mode == "S")
    {
      enumsConfig = "data/enums_soft.csv";
    }
  auditModule->fileSim << "Enums config file is: " << enumsConfig << endl;
  CsvReader csv (enumsConfig);
  map<string, enums::Enum *> Action;
  map<string, enums::Enum *> UserLevel;
  map<string, enums::Enum *> DeviceClass;
  map<string, enums::Enum *> AccessWay;
  map<string, enums::Enum *> Localization;
  map<string, enums::Enum *> TimeClass;
  map<string, enums::Enum *> Age;
  map<string, enums::Enum *> Group;
  vector<map<string, enums::Enum *>> props = {Action,       UserLevel, DeviceClass, AccessWay,
                                              Localization, TimeClass, Age,         Group};
  int index = 0;
  while (csv.FetchNextRow ())
    {
      // cout << "Processing row " << csv.RowNumber () << "..." << endl;
      // Ignore blank lines and header
      // cout << csv.ColumnCount () << endl;
      if (csv.ColumnCount () == 0)
        {
          ++index;
          continue;
        }

      bool ok = true;
      string k;
      int v;
      int w;
      ok |= csv.GetValue (0, k);
      ok |= csv.GetValue (1, v);
      ok |= csv.GetValue (2, w);
      if (!ok)
        {
          // Handle error, then
          NS_LOG_ERROR ("Error parsing csv file, row number: " + csv.RowNumber ());
          continue;
        }

      // pair<string, enums::Enum *> p = new
      // cout << "inserting " << enums::Enum (k, v, w) << " into " << index << endl;
      props[index].insert ({k, new enums::Enum (k, v, w)});

      // props[index].at (k)->value = v;
      // props[index].at (k)->weight = w;
    }
  enums::Properties *propsObj = new enums::Properties (props[0], props[1], props[2], props[3],
                                                       props[4], props[5], props[6], props[7]);
  return propsObj;
}

DeviceComponent *
buildServerStructure (AuditComponent *auditModule, enums::Properties *props)
{
  vector<User *> users = {new User (1, props->UserLevel.at ("ADMIN"), props->Age.at ("ADULT")),
                          new User (2, props->UserLevel.at ("ADULT"), props->Age.at ("ADULT")),
                          new User (3, props->UserLevel.at ("CHILD"), props->Age.at ("TEEN")),
                          new User (4, props->UserLevel.at ("CHILD"), props->Age.at ("KID")),
                          new User (5, props->UserLevel.at ("VISITOR"), props->Age.at ("ADULT"))};

  auditModule->fileSim << "Users of simulation are " << users.size () << ":" << endl;
  for (User *user : users)
    {
      auditModule->fileSim << *user << endl;
      if (user->userLevel == props->UserLevel.at ("ADMIN"))
        {
          ++auditModule->adminNumber;
        }
    }

  vector<Device *> devices = {
      new Device (1, "Wardrobe", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM,
                  true), // wardrobe
      new Device (2, "TV", props->DeviceClass.at ("NONCRITICAL"), enums::LIVINGROOM, true), // tv
      new Device (3, "Oven", props->DeviceClass.at ("CRITICAL"), enums::KITCHEN,
                  true), // oven
      new Device (4, "Office Light", props->DeviceClass.at ("NONCRITICAL"), enums::OFFICE,
                  true), // officeLight
      new Device (5, "Office Door Lock", props->DeviceClass.at ("CRITICAL"), enums::OFFICE,
                  true), // officeDoorLock
      new Device (6, "Office Door", props->DeviceClass.at ("NONCRITICAL"), enums::OFFICE,
                  true), // officeDoor
      new Device (7, "Office Carpet", props->DeviceClass.at ("NONCRITICAL"), enums::OFFICE,
                  false), // officeCarp
      new Device (8, "Office", props->DeviceClass.at ("NONCRITICAL"), enums::OFFICE,
                  false), // office
      new Device (9, "Main Door Lock", props->DeviceClass.at ("CRITICAL"), enums::HOUSE,
                  true), // mainDoorLock
      new Device (10, "Main Door", props->DeviceClass.at ("NONCRITICAL"), enums::HOUSE,
                  true), // mainDoor
      new Device (11, "Living Light", props->DeviceClass.at ("NONCRITICAL"), enums::LIVINGROOM,
                  true), // livingLight
      new Device (12, "Living Carpet", props->DeviceClass.at ("NONCRITICAL"), enums::LIVINGROOM,
                  false), // livingCarp
      new Device (13, "Kitchen Light", props->DeviceClass.at ("NONCRITICAL"), enums::KITCHEN,
                  true), // kitchenLight
      new Device (14, "Kitchen Door Lock", props->DeviceClass.at ("CRITICAL"), enums::KITCHEN,
                  true), // kitchenDoorLock
      new Device (15, "Kitchen Door", props->DeviceClass.at ("NONCRITICAL"), enums::KITCHEN,
                  true), // kitchenDoor
      new Device (16, "Kitchen Carpet", props->DeviceClass.at ("NONCRITICAL"), enums::KITCHEN,
                  false), // kitchenCarp
      new Device (17, "Hallway Light", props->DeviceClass.at ("NONCRITICAL"), enums::HOUSE,
                  true), // hallwayLight
      new Device (18, "Fridge", props->DeviceClass.at ("CRITICAL"), enums::KITCHEN, true), // fridge
      new Device (19, "Couch", props->DeviceClass.at ("NONCRITICAL"), enums::LIVINGROOM,
                  false), // couch
      new Device (20, "Bedroom Light", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM,
                  true), // bedroomLight
      new Device (21, "Bedroom Door Lock", props->DeviceClass.at ("CRITICAL"), enums::BEDROOM,
                  true), // bedroomDoorLock
      new Device (22, "Bedroom Door", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM,
                  true), // bedroomDoor
      new Device (23, "Bedroom Carpet", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM,
                  false), // bedroomCarp
      new Device (24, "Bed Table Lamp", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM,
                  true), // bedTableLamp
      new Device (25, "Bed", props->DeviceClass.at ("NONCRITICAL"), enums::BEDROOM, false), // bed
      new Device (26, "Bathroom Light", props->DeviceClass.at ("NONCRITICAL"), enums::BATHROOM,
                  true), // bathroomLight
      new Device (27, "Bathroom Door Lock", props->DeviceClass.at ("CRITICAL"), enums::BATHROOM,
                  true), // bathroomDoorLock
      new Device (28, "Bathroom Door", props->DeviceClass.at ("NONCRITICAL"), enums::BATHROOM,
                  true), // bathroomDoor
      new Device (29, "Bathroom Carpet", props->DeviceClass.at ("NONCRITICAL"), enums::BATHROOM,
                  false) // bathroomCarp
  };

  auditModule->fileSim << "Devices of simulation are " << devices.size () << ":" << endl;
  for (Device *device : devices)
    {
      auditModule->fileSim << *device << endl;
      if (device->deviceClass == props->DeviceClass.at ("CRITICAL"))
        {
          ++auditModule->criticalNumber;
        }
    }

  auditModule->minDeviceNumber = devices.size ();
  auditModule->maxDeviceNumber = devices.size ();
  auditModule->deviceExtensibility = 0;

  auditModule->privacyRisk = auditModule->adminNumber * auditModule->criticalNumber;

  vector<enums::Enum *> visitorCriticalCap = {};
  Ontology *visitorCritical = new Ontology (props->UserLevel.at ("VISITOR"),
                                            props->DeviceClass.at ("CRITICAL"), visitorCriticalCap);

  vector<enums::Enum *> childCriticalCap = {props->Action.at ("VIEW")};
  childCriticalCap.insert (childCriticalCap.end (), visitorCriticalCap.begin (),
                           visitorCriticalCap.end ());
  Ontology *childCritical = new Ontology (props->UserLevel.at ("CHILD"),
                                          props->DeviceClass.at ("CRITICAL"), childCriticalCap);

  vector<enums::Enum *> adultCriticalCap = {props->Action.at ("CONTROL")};
  adultCriticalCap.insert (adultCriticalCap.end (), childCriticalCap.begin (),
                           childCriticalCap.end ());
  Ontology *adultCritical = new Ontology (props->UserLevel.at ("ADULT"),
                                          props->DeviceClass.at ("CRITICAL"), adultCriticalCap);

  vector<enums::Enum *> adminCriticalCap = {props->Action.at ("MANAGE")};
  adminCriticalCap.insert (adminCriticalCap.end (), adultCriticalCap.begin (),
                           adultCriticalCap.end ());
  Ontology *adminCritical = new Ontology (props->UserLevel.at ("ADMIN"),
                                          props->DeviceClass.at ("CRITICAL"), adminCriticalCap);

  vector<enums::Enum *> visitorNonCriticalCap = {props->Action.at ("VIEW"),
                                                 props->Action.at ("CONTROL")};
  Ontology *visitorNonCritical =
      new Ontology (props->UserLevel.at ("VISITOR"), props->DeviceClass.at ("NONCRITICAL"),
                    visitorNonCriticalCap);

  vector<enums::Enum *> childNonCriticalCap = {};
  childNonCriticalCap.insert (childNonCriticalCap.end (), visitorNonCriticalCap.begin (),
                              visitorNonCriticalCap.end ());
  Ontology *childNonCritical = new Ontology (
      props->UserLevel.at ("CHILD"), props->DeviceClass.at ("NONCRITICAL"), childNonCriticalCap);

  vector<enums::Enum *> adultNonCriticalCap = {props->Action.at ("MANAGE")};
  adultNonCriticalCap.insert (adultNonCriticalCap.end (), childNonCriticalCap.begin (),
                              childNonCriticalCap.end ());
  Ontology *adultNonCritical = new Ontology (
      props->UserLevel.at ("ADULT"), props->DeviceClass.at ("NONCRITICAL"), adultNonCriticalCap);

  vector<enums::Enum *> adminNonCriticalCap = {};
  adminNonCriticalCap.insert (adminNonCriticalCap.end (), adultNonCriticalCap.begin (),
                              adultNonCriticalCap.end ());
  Ontology *adminNonCritical = new Ontology (
      props->UserLevel.at ("ADMIN"), props->DeviceClass.at ("NONCRITICAL"), adminNonCriticalCap);

  vector<Ontology *> ontologies = {visitorCritical,  childCritical,      adultCritical,
                                   adminCritical,    visitorNonCritical, childNonCritical,
                                   adultNonCritical, adminNonCritical};

  auditModule->fileSim << "Ontologies of simulation are " << ontologies.size () << ":" << endl;
  for (Ontology *ontology : ontologies)
    {
      auditModule->fileSim << *ontology << endl;
    }

  auditModule->userLevelNumber = props->UserLevel.size ();
  auditModule->deviceClassNumber = props->DeviceClass.size ();
  auditModule->actionNumber = props->Action.size ();

  auditModule->resourceIsolation =
      auditModule->userLevelNumber * auditModule->deviceClassNumber * auditModule->actionNumber;

  // Behavior Module
  int blockThreshold = 3;
  int blockInterval = 24;
  int buildInterval = 32;
  int markovThreshold = 0.1;
  ConfigurationComponent *configurationComponent =
      new ConfigurationComponent (blockThreshold, blockInterval, buildInterval, markovThreshold,
                                  devices, users, ontologies, auditModule, props);
  auditModule->fileSim << "Other configuration parameters of simulation are:" << endl;
  auditModule->fileSim << "Block Threshold: " << blockThreshold << endl;
  auditModule->fileSim << "Block Interval: " << blockInterval << endl;
  auditModule->fileSim << "Build Interval: " << buildInterval << endl;
  NotificationComponent *notificationComponent =
      new NotificationComponent (configurationComponent, auditModule);

  // Collection Module
  DataComponent *dataComponent = new DataComponent ();

  // Decision Module
  OntologyComponent *ontologyComponent =
      new OntologyComponent (configurationComponent, auditModule);
  ContextComponent *contextComponent = new ContextComponent (configurationComponent, auditModule);
  ActivityComponent *activityComponent =
      new ActivityComponent (dataComponent, configurationComponent, auditModule);
  AuthorizationComponent *authorizationComponent =
      new AuthorizationComponent (configurationComponent, ontologyComponent, contextComponent,
                                  activityComponent, notificationComponent, auditModule);

  DeviceComponent *deviceComponent =
      new DeviceComponent (authorizationComponent, dataComponent, auditModule);

  return deviceComponent;
};

void
appsConfiguration (Ipv6InterfaceContainer serverApInterface, DeviceComponent *deviceComponent,
                   double start, double stop, NodeContainer serverNode, NodeContainer staNodes,
                   Ipv6InterfaceContainer staInterface, string dataRate, vector<Device *> devices,
                   AuditComponent *auditModule)
{
  // Create a server to receive these packets
  // Start at 0s
  // Stop at final
  Address serverAddress (Inet6SocketAddress (serverApInterface.GetAddress (0, 0), 50000));
  Ptr<ZashServer> ZashServerApp = CreateObject<ZashServer> ();
  ZashServerApp->SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ZashServerApp->SetAttribute ("Local", AddressValue (serverAddress));
  ZashServerApp->SetDeviceComponent (deviceComponent);
  ZashServerApp->SetStartTime (Seconds (start));
  ZashServerApp->SetStopTime (Seconds (stop));
  serverNode.Get (0)->AddApplication (ZashServerApp);

  // Start at 5s
  // Each start 0.2s apart
  // Stop at final
  double startDevice = start + 5.0;
  for (uint32_t i = 0; i < staNodes.GetN (); ++i)
    {
      if (i == 7)
        {
          continue;
        }

      Address nodeAddress (Inet6SocketAddress (staInterface.GetAddress (i, 0), 50000));
      Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp = CreateObject<ZashDeviceEnforcer> ();
      ZashDeviceEnforcerApp->SetAttribute ("Protocol",
                                           TypeIdValue (TcpSocketFactory::GetTypeId ()));
      ZashDeviceEnforcerApp->SetAttribute ("Local", AddressValue (nodeAddress));
      ZashDeviceEnforcerApp->SetAttribute ("Remote", AddressValue (serverAddress));
      ZashDeviceEnforcerApp->SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
      ZashDeviceEnforcerApp->SetDevice (devices[i]);
      ZashDeviceEnforcerApp->SetAuditModule (auditModule);
      ZashDeviceEnforcerApp->SetStartTime (Seconds (startDevice));
      ZashDeviceEnforcerApp->SetStopTime (Seconds (stop));
      startDevice += 0.2;

      Ptr<Node> node = staNodes.Get (i);
      node->AddApplication (ZashDeviceEnforcerApp);
      auditModule->fileLog << "Installed device " << i << endl;
    }
}

void
scheduleMessages (NodeContainer staNodes, vector<Device *> devices, vector<User *> users,
                  DeviceComponent *deviceComponent, AuditComponent *auditModule, int startDay,
                  int endDay, enums::Properties *props)
{
  int user = 0;
  string accessWay = "PERSONAL";
  string localization = "INTERNAL";
  string group = "ALONE";
  string action = "CONTROL";
  auditModule->fileSim << "Devices interaction have the following properties: " << endl;
  auditModule->fileSim << "User: " << user << endl;
  auditModule->fileSim << "Access Way: " << accessWay << endl;
  auditModule->fileSim << "Localization: " << localization << endl;
  auditModule->fileSim << "Group: " << group << endl;
  auditModule->fileSim << "Action: " << action << endl;

  int idReq = 0;

  auditModule->fileLog << "Opening dataset..." << endl;
  string dataset = "data/d6_2m_0tm.csv";
  auditModule->fileMsgs << "Dataset is: " << dataset << endl;
  CsvReader csv (dataset);
  vector<int> lastState;
  time_t firstDate = (time_t) (-1);
  int msgCount = 0;
  int dayCount = 0;
  int simDayCount = 0;
  time_t lastDate = (time_t) (-1);
  while (csv.FetchNextRow ())
    {
      auditModule->fileMsgs << "Processing row " << csv.RowNumber () << "..." << endl;
      // Ignore blank lines and header
      if (csv.RowNumber () == 1 || csv.IsBlankRow ())
        {
          continue;
        }

      // Expecting vector
      vector<int> currentState (NUMBER_OF_DEVICES);
      bool ok = true;
      for (size_t i = 0; i < currentState.size (); ++i)
        {
          ok |= csv.GetValue (i, currentState[i]);
        }
      if (!ok)
        {
          // Handle error, then
          NS_LOG_ERROR ("Error parsing csv file, row number: " + csv.RowNumber ());
          continue;
        }

      string timeStr;
      csv.GetValue (DATE_COL, timeStr);
      time_t currentDate = strToTime (timeStr.c_str ());

      if (difftime (lastDate, (time_t) (-1)) == 0)
        {
          // firstDate = currentDate - 20; //
          ++dayCount;
          auditModule->fileMsgs << "Processing day " << dayCount << "..." << endl;
        }
      else if (extractDay (currentDate) != extractDay (lastDate))
        {
          ++dayCount;
          auditModule->fileMsgs << "Processing day " << dayCount << "..." << endl;
          if (startDay != 0 && dayCount >= startDay && dayCount <= endDay)
            {
              ++simDayCount;
            }
        }

      if ((startDay == 0 || dayCount == startDay) && difftime (firstDate, (time_t) (-1)) == 0)
        {
          firstDate = currentDate - 20;
        }

      if (endDay > 0 && dayCount >= endDay)
        {
          auditModule->fileMsgs << "Stopped at day " << endDay << endl;
          break;
        }

      string actStr;
      csv.GetValue (ACTIVITY_COL, actStr);

      // NS_LOG_INFO(vecToStr(currentState));
      auditModule->fileMsgs << vecToStr (currentState) << endl;

      if (currentState == lastState)
        {
          continue;
        }

      if (lastState.size () > 0)
        {
          vector<uint32_t> changes;

          for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
            {
              if (lastState[i] != currentState[i])
                {
                  changes.push_back (i);
                }
            }

          double diff = difftime (currentDate, firstDate);
          for (uint32_t change : changes)
            {
              if (change > staNodes.GetN () - 1 || change == 7)
                {
                  continue;
                }
              // NS_LOG_INFO(formatTime(currentDate) + " - " + actStr);
              auditModule->fileMsgs << formatTime (currentDate) << " - " << actStr << endl;

              string request = "[" + to_string (++idReq) + "," + to_string (change) + "," +
                               to_string (user) + "," + accessWay + "," + localization + "," +
                               group + "," + action + "," + formatTime (currentDate) + "]";

              if (dayCount < startDay)
                {
                  auditModule->fileMsgs << "*Zash Server simulated with message = " << request
                                        << endl;
                  auditModule->fileExec << "*Zash Server simulated with message = " << request
                                        << endl;
                  auditModule->zashOutput = &auditModule->fileExec;
                  Context *context =
                      new Context (props->AccessWay.at (accessWay),
                                   props->Localization.at (localization), props->Group.at (group));
                  enums::Enum *actionEnum = props->Action.at (action);
                  Request *req =
                      new Request (++idReq, devices[change], users[user], context, actionEnum);
                  deviceComponent->listenRequest (req, currentDate);
                }
              else
                {
                  // NS_LOG_INFO("Device enforcer scheduled with message = " + request);
                  auditModule->fileMsgs << "Device enforcer scheduled with message = " << request
                                        << endl;
                  auditModule->zashOutput = &auditModule->fileLog;
                  Ptr<Node> node = staNodes.Get (change);
                  Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp =
                      DynamicCast<ZashDeviceEnforcer> (node->GetApplication (0));
                  Simulator::Schedule (Seconds (diff), &ZashDeviceEnforcer::StartSending,
                                       ZashDeviceEnforcerApp, request);
                  msgCount++;
                  // NS_LOG_INFO(devices[change]->name << " will change at " << diff
                  //                                   << " seconds");
                  auditModule->fileMsgs << devices[change]->name << " will change at " << diff
                                        << " seconds" << endl;
                  diff += 0.2;
                }
            }
        }
      else
        {
          deviceComponent->dataComponent->lastState = currentState;
        }
      lastState = currentState;
      lastDate = currentDate;
    }

  auditModule->fileMsgs << "Count of messages = " << msgCount << endl;
  auditModule->fileMsgs << "Count of days = " << dayCount << endl;
  auditModule->fileMsgs << "Count of simulated days = " << simDayCount << endl;
  auditModule->fileMsgs << "Count of executed days = " << (dayCount - simDayCount) << endl;
};

int
main (int argc, char *argv[])
{
  //----------------------------------------------------------------------------------
  // Simulation logs
  //----------------------------------------------------------------------------------
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  LogComponentEnable ("ZASH", LOG_LEVEL_ALL);
  LogComponentEnable ("ZashDeviceEnforcer", LOG_LEVEL_ALL);
  LogComponentEnable ("ZashServer", LOG_LEVEL_ALL);
  // LogComponentEnable("ArpL3Protocol", LOG_LEVEL_INFO);
  LogComponentEnable ("AuditComponent", LOG_LEVEL_INFO);

  //----------------------------------------------------------------------------------
  // Simulation variables
  //----------------------------------------------------------------------------------
  // Set up some default values for the simulation.
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  double start = 0.0;
  // double stop = 86400.0;
  double stop = 4966270.0;
  // double stop = 200.0;
  uint32_t N = NUMBER_OF_DEVICES; // number of nodes in the star
  uint32_t payloadSize = 1448; /* Transport layer payload size in bytes. */
  string dataRate = "100Mbps"; /* Application layer datarate. */
  string phyRate = "HtMcs7"; /* Physical layer bitrate. */
  bool pcapTracing = false; /* PCAP Tracing is enabled or not. */
  string mode = "H"; /* Restriction mode for the system. */
  uint32_t startDay = 0; /* Start day of the simulation. */
  uint32_t endDay = 0; /* End day of the simulation. */

  // Allow the user to override any of the defaults and the above
  // Config::SetDefault()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  cmd.AddValue ("nNodes", "Number of nodes to place in the star", N);
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("dataRate", "Application data ate", dataRate);
  cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
  cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
  cmd.AddValue ("mode", "Restriction mode for the system", mode);
  cmd.AddValue ("start", "Start day of the simulation", startDay);
  cmd.AddValue ("end", "End day of the simulation", endDay);
  cmd.Parse (argc, argv);

  if (startDay != 0 && startDay < 1)
    {
      cout << "ZASH - Error: start must be greater than 0" << endl;
      return EXIT_FAILURE;
    }
  if (startDay != 0 && endDay != 0 && startDay > endDay)
    {
      cout << "ZASH - Error: start must be greater than end" << endl;
      return EXIT_FAILURE;
    }
  if (mode != "H" && mode != "S")
    {
      cout << "ZASH - Error: mode must be H (Hard) or S (Soft)" << endl;
      return EXIT_FAILURE;
    }

  // stop = (endDay - startDay) * 86400;
  double simulationTime = stop; /* Simulation time in seconds. */

  // Config::SetDefault("ns3::TcpL4Protocol::SocketType",
  //                    TypeIdValue(TypeId::LookupByName("ns3::TcpNewReno")));

  /* Configure TCP Options */
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

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

  AuditComponent *auditModule = createAudit ();

  enums::Properties *props = buildEnums (auditModule, mode);

  printEnums (props, auditModule);

  // auditModule->calculatePossibilities (props);

  // return EXIT_SUCCESS;

  //----------------------------------------------------------------------------------
  // Topology configuration
  //----------------------------------------------------------------------------------

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  WifiStandard wifiStandard = WIFI_STANDARD_80211n;
  wifiHelper.SetStandard (wifiStandard);
  Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.046));
  auditModule->fileSim << "Wifi standard is " << wifiStandard << endl;

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  // wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  // wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
  // "Frequency",
  //                                DoubleValue(5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel.Create ());
  // Set MIMO capabilities
  // wifiPhy.Set("Antennas", UintegerValue(4));
  // wifiPhy.Set("MaxSupportedTxSpatialStreams", UintegerValue(4));
  // wifiPhy.Set("MaxSupportedRxSpatialStreams", UintegerValue(4));
  // wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
  // wifiHelper.SetRemoteStationManager("ns3::AarfWifiManager");
  // wifiHelper.SetRemoteStationManager("ns3::IdealWifiManager");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                      StringValue (phyRate), "ControlMode",
                                      StringValue ("ErpOfdmRate24Mbps"));

  // Here, we will create N nodes in a star.
  auditModule->fileLog << "Create nodes." << endl;
  uint32_t nServers = 1;
  uint32_t nAps = 1;
  NodeContainer serverNode;
  serverNode.Create (nServers);
  NodeContainer apNode;
  apNode.Create (nAps);
  NodeContainer staNodes;
  staNodes.Create (N);
  NodeContainer allNodes = NodeContainer (serverNode, apNode, staNodes);
  auditModule->fileSim << "Number of servers is " << nServers << endl;
  auditModule->fileSim << "Number of APs is " << nAps << endl;
  auditModule->fileSim << "Number of STAs is " << N << endl;

  NodeContainer serverAp = NodeContainer (serverNode.Get (0), apNode.Get (0));
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("1ms"));
  NetDeviceContainer serverApDevice = csma.Install (serverAp);

  /* Configure AP */
  auditModule->fileLog << "Configure AP" << endl;
  Ssid ssid = Ssid ("network");
  wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid), "BeaconInterval",
                   TimeValue (MicroSeconds (4096000)), "BsrLifetime",
                   TimeValue (MilliSeconds (800)));

  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install (wifiPhy, wifiMac, apNode);

  /* Configure STA */
  auditModule->fileLog << "Configure STA" << endl;
  wifiMac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "WaitBeaconTimeout",
                   TimeValue (MilliSeconds (4800)), "AssocRequestTimeout",
                   TimeValue (Seconds (20)));

  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install (wifiPhy, wifiMac, staNodes);

  Config::Set ("/NodeList//DeviceList//$ns3::WifiNetDevice/HtConfiguration/"
               "ShortGuardIntervalSupported",
               BooleanValue (true));

  auditModule->fileLog << "Configure Bridge" << endl;
  BridgeHelper bridge;
  NetDeviceContainer bridgeDev;
  bridgeDev = bridge.Install (apNode.Get (0),
                              NetDeviceContainer (apDevice.Get (0), serverApDevice.Get (1)));

  /* Mobility model */
  auditModule->fileLog << "Configure mobility" << endl;
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add ("data/positions.csv");

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  // Install network stacks on the nodes
  InternetStackHelper internet;
  internet.Install (allNodes);

  // Later, we add IP addresses.
  auditModule->fileLog << "Assign IP Addresses." << endl;
  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer serverApInterface = ipv6.Assign (serverApDevice);
  serverApInterface.SetForwarding (1, true);
  serverApInterface.SetDefaultRouteInAllNodes (1);
  auditModule->fileSim << "Server IP is " << serverApInterface.GetAddress (0, 0) << endl;
  Ipv6InterfaceContainer apInterface = ipv6.Assign (bridgeDev);
  apInterface.SetForwarding (0, true);
  apInterface.SetDefaultRouteInAllNodes (0);
  auditModule->fileSim << "AP IP is " << apInterface.GetAddress (0, 0) << endl;
  Ipv6InterfaceContainer staInterface = ipv6.Assign (staDevices);
  staInterface.SetDefaultRouteInAllNodes (apInterface.GetAddress (0, 0));
  auditModule->fileSim << "STA IP start with " << staInterface.GetAddress (0, 0) << endl;

  // Turn on global static routing
  // Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Ptr<Node> n0 = apNode.Get(0);

  // StackHelper stackHelper;
  // stackHelper.PrintRoutingTable(n0);

  //----------------------------------------------------------------------------------
  // ZASH Application Logic
  //----------------------------------------------------------------------------------

  DeviceComponent *deviceComponent = buildServerStructure (auditModule, props);

  // NS_LOG_INFO(deviceComponent);

  vector<Device *> devices =
      deviceComponent->authorizationComponent->configurationComponent->devices;
  vector<User *> users = deviceComponent->authorizationComponent->configurationComponent->users;

  //----------------------------------------------------------------------------------
  // Applications configuration
  //----------------------------------------------------------------------------------

  appsConfiguration (serverApInterface, deviceComponent, start, stop, serverNode, staNodes,
                     staInterface, dataRate, devices, auditModule);

  //----------------------------------------------------------------------------------
  // Schedule messages from dataset
  //----------------------------------------------------------------------------------

  scheduleMessages (staNodes, devices, users, deviceComponent, auditModule, startDay, endDay,
                    props);

  //----------------------------------------------------------------------------------
  // Output configuration
  //----------------------------------------------------------------------------------

  createFile (auditModule->scenarioSimFile, auditModule->simDate, auditModule->fileSim.str ());
  createFile (auditModule->messagesSimFile, auditModule->simDate, auditModule->fileMsgs.str ());
  createFile (auditModule->execSimFile, auditModule->simDate, auditModule->fileExec.str ());

  // Callback Trace to Collect Messages in Device Enforcer Application
  for (uint32_t i = 0; i < staNodes.GetN (); ++i)
    {
      if (i == 7)
        {
          continue;
        }
      Ptr<Node> node = staNodes.Get (i);
      Ptr<ZashDeviceEnforcer> ZashDeviceEnforcerApp =
          DynamicCast<ZashDeviceEnforcer> (node->GetApplication (0));
      ostringstream paramTest;
      paramTest << "/NodeList/" << (ZashDeviceEnforcerApp->z_device->id)
                << "/ApplicationList/*/$ns3::ZashDeviceEnforcer/Traces";
      ZashDeviceEnforcerApp->m_traces.Connect (
          MakeCallback (&AuditComponent::deviceEnforcerCallback, auditModule), paramTest.str ());
      // Config::Connect(
      //     paramTest.str(),
      //     MakeCallback(&AuditComponent::deviceEnforcerCallback,
      //     auditComponent));
    }

  /* Enable Traces */
  if (pcapTracing)
    {
      wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      wifiPhy.EnablePcap ("AccessPoint", apDevice);
      wifiPhy.EnablePcap ("Station", staDevices);
      csma.EnablePcap ("Server", serverApDevice);
    }

  // configure tracing
  // AsciiTraceHelper ascii;
  // mobility.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));
  // csma.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));

  /* Stop Simulation */
  Simulator::Stop (Seconds (simulationTime + 1));

  // 40mx28m
  AnimationInterface anim ("animation.xml");
  anim.SetBackgroundImage ("/home/grosa/Dev/ns-allinone-3.35/ns-3.35/data/home-design.png", 0, 0,
                           0.07, 0.07, 1.0);
  for (uint32_t i = 0; i < staNodes.GetN (); ++i)
    {
      Ptr<Node> node = staNodes.Get (i);
      anim.UpdateNodeDescription (node, devices[i]->name); // Optional
      anim.UpdateNodeColor (node, 255, 0, 0); // Optional
      anim.UpdateNodeSize (node->GetId (), 0.8, 0.8);
    }
  for (uint32_t i = 0; i < apNode.GetN (); ++i)
    {
      Ptr<Node> node = apNode.Get (i);
      anim.UpdateNodeDescription (node, "AP"); // Optional
      anim.UpdateNodeColor (node, 0, 255, 0); // Optional
      anim.UpdateNodeSize (node->GetId (), 0.8, 0.8);
    }
  for (uint32_t i = 0; i < serverNode.GetN (); ++i)
    {
      Ptr<Node> node = serverNode.Get (i);
      anim.UpdateNodeDescription (node, "Local Server"); // Optional
      anim.UpdateNodeColor (node, 0, 0, 255); // Optional
      anim.UpdateNodeSize (node->GetId (), 1.2, 1.2);
    }
  anim.EnablePacketMetadata ();
  anim.EnableIpv4RouteTracking ("zash.txt", Seconds (0), Seconds (200), Seconds (5));

  // Flow monitor
  // Ptr<FlowMonitor> flowMonitor;
  // FlowMonitorHelper flowHelper;
  // flowMonitor = flowHelper.InstallAll();

  // Trace routing tables
  // Ipv4GlobalRoutingHelper g;
  // Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(
  //     "dynamic-global-routing.routes", ios::out);
  // g.PrintRoutingTableAllAt(Seconds(12), routingStream);

  //----------------------------------------------------------------------------------
  // Call simulation
  //----------------------------------------------------------------------------------

  auditModule->fileLog << "Run Simulation." << endl;
  Simulator::Run ();
  Simulator::Destroy ();
  auditModule->fileLog << "Done." << endl;

  // flowMonitor->SerializeToXmlFile("flow.xml", true, true);

  auditModule->outputMetrics ();
  createFile (auditModule->logSimFile, auditModule->simDate, auditModule->fileLog.str ());

  return 0;
}
