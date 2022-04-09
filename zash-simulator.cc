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

// Default Network topology, 9 nodes in a star
/*
          n2 n3 n4
           \ | /
            \|/
       n1---n0---n5
            /| \
           / | \
          n8 n7 n6
*/
// - CBR Traffic goes from the star "arms" to the "hub"
// - Tracing of queues and packet receptions to file
//   "tcp-star-server.tr"
// - pcap traces also generated in the following files
//   "tcp-star-server-$n-$i.pcap" where n and i represent node and interface
//   numbers respectively
// Usage examples for things you might want to tweak:
//       ./waf --run="tcp-star-server"
//       ./waf --run="tcp-star-server --nNodes=25"
//       ./waf --run="tcp-star-server --ns3::OnOffApplication::DataRate=10000"
//       ./waf --run="tcp-star-server --ns3::OnOffApplication::PacketSize=500"
// See the ns-3 tutorial for more info on the command line:
// http://www.nsnam.org/tutorials.html

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "ns3/applications-module.h"
#include "ns3/bridge-helper.h"
#include "ns3/core-module.h"
#include "ns3/csma-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
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

int main(int argc, char *argv[]) {
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this

  LogComponentEnable("ZASH", LOG_LEVEL_ALL);
  LogComponentEnable("DeviceEnforcer", LOG_LEVEL_ALL);
  LogComponentEnable("ZashServer", LOG_LEVEL_ALL);
  // LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);

  // Set up some default values for the simulation.
  Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(250));
  Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("5kb/s"));
  double start = 0.0;
  double stop = 10.0;
  uint32_t N = NUMBER_OF_DEVICES;   // number of nodes in the star
  uint32_t payloadSize = 1472;      /* Transport layer payload size in bytes. */
  string dataRate = "100Mbps";      /* Application layer datarate. */
  string tcpVariant = "TcpNewReno"; /* TCP variant type. */
  string phyRate = "HtMcs7";        /* Physical layer bitrate. */
  double simulationTime = 10;       /* Simulation time in seconds. */
  bool pcapTracing = false;         /* PCAP Tracing is enabled or not. */

  // Allow the user to override any of the defaults and the above
  // Config::SetDefault()s at run-time, via command-line arguments
  CommandLine cmd(__FILE__);
  cmd.AddValue("nNodes", "Number of nodes to place in the star", N);
  cmd.AddValue("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue("dataRate", "Application data ate", dataRate);
  cmd.AddValue(
      "tcpVariant",
      "Transport protocol to use: TcpNewReno, "
      "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
      "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat ",
      tcpVariant);
  cmd.AddValue("phyRate", "Physical layer bitrate", phyRate);
  cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue("pcap", "Enable/disable PCAP Tracing", pcapTracing);
  cmd.Parse(argc, argv);

  tcpVariant = string("ns3::") + tcpVariant;
  // Select TCP variant
  if (tcpVariant.compare("ns3::TcpWestwoodPlus") == 0) {
    // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TcpWestwood::GetTypeId()));
    // the default protocol type in ns3::TcpWestwood is WESTWOOD
    Config::SetDefault("ns3::TcpWestwood::ProtocolType",
                       EnumValue(TcpWestwood::WESTWOODPLUS));
  } else {
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(tcpVariant, &tcpTid),
                        "TypeId " << tcpVariant << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(tcpVariant)));
  }

  /* Configure TCP Options */
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  wifiHelper.SetStandard(WIFI_STANDARD_80211n_2_4GHZ);

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel", "Frequency",
                                 DoubleValue(5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel(wifiChannel.Create());
  wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
  wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                                     StringValue(phyRate), "ControlMode",
                                     StringValue("HtMcs0"));

  // Here, we will create N nodes in a star.
  NS_LOG_INFO("Create nodes.");
  NodeContainer serverNode;
  serverNode.Create(1);
  NodeContainer apNode;
  apNode.Create(1);
  NodeContainer staNodes;
  staNodes.Create(N);
  NodeContainer allNodes = NodeContainer(serverNode, apNode, staNodes);

  NodeContainer serverAp = NodeContainer(serverNode.Get(0), apNode.Get(0));
  // PointToPointHelper p2p;
  // p2p.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
  // p2p.SetChannelAttribute("Delay", StringValue("1ms"));
  // NetDeviceContainer serverApDevice = p2p.Install(serverAp);
  CsmaHelper csma;
  // csma.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
  // csma.SetChannelAttribute("Delay", StringValue("1ms"));
  NetDeviceContainer serverApDevice = csma.Install(serverAp);

  /* Configure AP */
  Ssid ssid = Ssid("network");
  wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install(wifiPhy, wifiMac, apNode);

  /* Configure STA */
  wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing",
                  BooleanValue(false));

  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install(wifiPhy, wifiMac, staNodes);

  BridgeHelper bridge;
  NetDeviceContainer bridgeDev;
  bridgeDev = bridge.Install(
      apNode.Get(0), NetDeviceContainer(apDevice, serverApDevice.Get(1)));

  /* Mobility model */
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
  Ipv4AddressHelper address;
  address.SetBase("192.168.0.0", "255.255.255.0");
  Ipv4InterfaceContainer serverApInterface =
      address.Assign(serverApDevice.Get(0));
  Ipv4InterfaceContainer apInterface = address.Assign(bridgeDev);
  Ipv4InterfaceContainer staInterface = address.Assign(staDevices);

  // Turn on global static routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  //----------------------------------------------------------------------------------
  // ZASH Application Logic
  //----------------------------------------------------------------------------------

  vector<User *> users = {
      new User(1, enums::UserLevel.at("ADMIN"), enums::Age.at("ADULT")),
      new User(2, enums::UserLevel.at("ADULT"), enums::Age.at("ADULT")),
      new User(3, enums::UserLevel.at("CHILD"), enums::Age.at("TEEN")),
      new User(4, enums::UserLevel.at("CHILD"), enums::Age.at("KID")),
      new User(5, enums::UserLevel.at("VISITOR"), enums::Age.at("ADULT"))};

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

  // Audit Module
  AuditComponent *auditModule = new AuditComponent();

  // Behavior Module
  ConfigurationComponent *configurationComponent =
      new ConfigurationComponent(3, 24, 32, devices, users, ontologies);
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

  NS_LOG_INFO(deviceComponent);

  // Collection Module
  // Create a server to receive these packets
  Address serverAddress(InetSocketAddress(serverApInterface.GetAddress(0), 9));
  Ptr<ZashServer> ZashServerApp = CreateObject<ZashServer>();
  ZashServerApp->SetAttribute("Protocol",
                              TypeIdValue(TcpSocketFactory::GetTypeId()));
  ZashServerApp->SetAttribute("Local", AddressValue(serverAddress));
  // ZashServerApp->SetDeviceComponent(deviceComponent);
  ZashServerApp->SetStartTime(Seconds(start));
  ZashServerApp->SetStopTime(Seconds(stop));
  serverNode.Get(0)->AddApplication(ZashServerApp);

  // Create a packet sink on the star "hub" to router these packets
  // Address routerAddress1(InetSocketAddress(apInterface.GetAddress(0), 9));
  // Address routerAddress2(InetSocketAddress(serverApInterface.GetAddress(1),
  // 9)); Ptr<ZashRouter> ZashRouterApp = CreateObject<ZashRouter>();
  // ZashRouterApp->SetAttribute("Protocol",
  //                             TypeIdValue(TcpSocketFactory::GetTypeId()));
  // ZashRouterApp->SetAttribute("Local", AddressValue(routerAddress1));
  // ZashRouterApp->SetAttribute("Router", BooleanValue(true));
  // ZashRouterApp->SetStartTime(Seconds(start));
  // ZashRouterApp->SetStopTime(Seconds(stop));
  // ZashRouterApp->zashPacketSender->SetAttribute(
  //     "Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
  // ZashRouterApp->zashPacketSender->SetAttribute("Local",
  //                                               AddressValue(routerAddress2));
  // ZashRouterApp->zashPacketSender->SetAttribute("Remote",
  //                                               AddressValue(serverAddress));
  // ZashRouterApp->zashPacketSender->SetAttribute(
  //     "DataRate", DataRateValue(DataRate(dataRate)));
  // ZashRouterApp->zashPacketSender->SetAttribute("StopApp",
  // BooleanValue(false));
  // ZashRouterApp->zashPacketSender->SetStartTime(Seconds(start));
  // ZashRouterApp->zashPacketSender->SetStopTime(Seconds(stop));
  // apNode.Get(0)->AddApplication(ZashRouterApp);
  // apNode.Get(0)->AddApplication(ZashRouterApp->zashPacketSender);

  int user = 0;
  // User *simUser = users[user];
  string accessWay = "PERSONAL";
  string localization = "INTERNAL";
  string group = "ALONE";
  string action = "CONTROL";
  // Context *simContext =
  //     new Context(enums::AccessWay.at("PERSONAL"),
  //                 enums::Localization.at("INTERNAL"),
  //                 enums::Group.at("ALONE"));
  // enums::Enum *simAction = enums::Action.at("CONTROL");

  int idReq = 0;

  NS_LOG_INFO("Opening dataset...");
  CsvReader csv("data/d6_2m_0tm.csv");
  vector<int> lastState;
  time_t firstDate = (time_t)(-1);
  while (csv.FetchNextRow()) {
    NS_LOG_INFO("Processing row " + to_string(csv.RowNumber()) + "...");
    // Ignore blank lines and header
    if (csv.RowNumber() == 1 || csv.IsBlankRow()) {
      continue;
    }

    if (csv.RowNumber() == 20) {
      break;
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
      firstDate = currentDate;
    }

    string actStr;
    csv.GetValue(ACTIVITY_COL, actStr);

    // NS_LOG_INFO(formatTime(currentDate) + " - " +
    //             to_string(deviceComponent->auditComponent->reqNumber));

    NS_LOG_INFO(vecToStr(currentState));

    if (currentState == lastState) {
      continue;
    }

    if (lastState.size() > 0) {
      vector<int> changes;

      for (int i = 0; i < NUMBER_OF_DEVICES; ++i) {
        if (lastState[i] != currentState[i]) {
          changes.push_back(i);
        }
      }

      //   cout << "Last State =    " << vecToStr(lastState) <<
      //   endl; cout << "Current State = " << vecToStr(currentState) << endl;

      for (int change : changes) {
        // NS_LOG_INFO("Change on " + devices[change]->name);
        NS_LOG_INFO(formatTime(currentDate) + " - " + actStr);

        double diff = difftime(currentDate, firstDate);

        Address nodeAddress(
            InetSocketAddress(staInterface.GetAddress(change), 9));
        string request = "[" + to_string(++idReq) + "," + to_string(change) +
                         "," + to_string(user) + "," + accessWay + "," +
                         localization + "," + group + "," + action + "]";
        NS_LOG_INFO("Device enforcer created with message = " + request);
        Ptr<DeviceEnforcer> DeviceEnforcerApp = CreateObject<DeviceEnforcer>();
        DeviceEnforcerApp->SetAttribute(
            "Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
        DeviceEnforcerApp->SetAttribute("Local", AddressValue(nodeAddress));
        DeviceEnforcerApp->SetAttribute("Remote", AddressValue(serverAddress));
        DeviceEnforcerApp->SetAttribute("DataRate",
                                        DataRateValue(DataRate(dataRate)));
        DeviceEnforcerApp->SetStartTime(Seconds(diff));
        DeviceEnforcerApp->SetStopTime(Seconds(diff + 1.0));
        DeviceEnforcerApp->SetMessage(request);

        Ptr<Node> node = staNodes.Get(change);

        NS_LOG_INFO(devices[change]->name + " will change at " +
                    to_string(diff) + " seconds");

        node->AddApplication(DeviceEnforcerApp);
        // deviceComponent->listenRequest(req, currentDate);
        // cout << endl;
      }
    }
    lastState = currentState;
  }

  // Create the OnOff applications to send TCP to the server
  // OnOffHelper server("ns3::TcpSocketFactory",
  //                    (InetSocketAddress(apInterface.GetAddress(0), 9)));
  // server.SetAttribute("PacketSize", UintegerValue(payloadSize));
  // server.SetAttribute("OnTime",
  //                     StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  // server.SetAttribute("OffTime",
  //                     StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  // server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
  // ApplicationContainer serverApp = server.Install(staNodes);

  // normally wouldn't need a loop here but the server IP address is different
  // on each p2p subnet
  // ApplicationContainer clientApps;
  // for (uint32_t i = 0; i < staNodes.GetN(); ++i) {
  //   AddressValue remoteAddress(
  //       InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0), port));
  // clientHelper.SetAttribute("Remote", remoteAddress);
  // clientApps.Add(clientHelper.Install(staNodes.Get(i)));
  // }
  /* Start Applications */
  // sinkApp.Start(Seconds(0.0));
  // serverApp.Start(Seconds(1.0));

  /* Enable Traces */
  if (pcapTracing) {
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.EnablePcap("AccessPoint", apDevice);
    wifiPhy.EnablePcap("Station", staDevices);
    csma.EnablePcap("Server", serverApDevice);
  }

  // configure tracing
  AsciiTraceHelper ascii;
  // mobility.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));
  csma.EnableAsciiAll(ascii.CreateFileStream("zash.tr"));
  // p2p.EnablePcapAll("tcp-star-server");

  /* Stop Simulation */
  Simulator::Stop(Seconds(simulationTime + 1));

  // 40mx28m
  AnimationInterface anim("animation.xml");
  anim.SetBackgroundImage(
      "/home/grosa/Dev/ns-allinone-3.35/ns-3.35/scratch/home-design.png", 0, 0,
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
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  flowMonitor->SerializeToXmlFile("flow.xml", true, true);

  return 0;
}
