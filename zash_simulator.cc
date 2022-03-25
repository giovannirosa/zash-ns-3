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
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpServer");

int main(int argc, char *argv[]) {
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this

  // LogComponentEnable ("TcpServer", LOG_LEVEL_INFO);
  // LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL);
  // LogComponentEnable ("TcpSocketImpl", LOG_LEVEL_ALL);
  // LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);

  // Set up some default values for the simulation.
  Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(250));
  Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("5kb/s"));
  uint32_t N = 9;                   // number of nodes in the star
  uint32_t payloadSize = 1472;      /* Transport layer payload size in bytes. */
  std::string dataRate = "100Mbps"; /* Application layer datarate. */
  std::string tcpVariant = "TcpNewReno"; /* TCP variant type. */
  std::string phyRate = "HtMcs7";        /* Physical layer bitrate. */
  double simulationTime = 10;            /* Simulation time in seconds. */
  bool pcapTracing = false;              /* PCAP Tracing is enabled or not. */

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

  tcpVariant = std::string("ns3::") + tcpVariant;
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
  NodeContainer clientNodes;
  serverNode.Create(1);
  clientNodes.Create(N - 1);
  NodeContainer allNodes = NodeContainer(serverNode, clientNodes);

  /* Configure AP */
  Ssid ssid = Ssid("network");
  wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install(wifiPhy, wifiMac, serverNode);

  /* Configure STA */
  wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));

  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install(wifiPhy, wifiMac, clientNodes);

  /* Mobility model */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc =
      CreateObject<ListPositionAllocator>();
  for (int i = 0; i < 29; ++i) {
    positionAlloc->Add(Vector(i, i, 0.0));
  }

  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(allNodes);

  // Install network stacks on the nodes
  InternetStackHelper internet;
  internet.Install(allNodes);

  // Collect an adjacency list of nodes for the p2p topology
  // std::vector<NodeContainer> nodeAdjacencyList(N - 1);
  // for (uint32_t i = 0; i < nodeAdjacencyList.size(); ++i) {
  //   nodeAdjacencyList[i] = NodeContainer(serverNode, clientNodes.Get(i));
  // }

  // We create the channels first without any IP addressing information
  // NS_LOG_INFO("Create channels.");
  // PointToPointHelper p2p;
  // p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  // p2p.SetChannelAttribute("Delay", StringValue("2ms"));
  // std::vector<NetDeviceContainer> deviceAdjacencyList(N - 1);
  // for (uint32_t i = 0; i < deviceAdjacencyList.size(); ++i) {
  //   deviceAdjacencyList[i] = p2p.Install(nodeAdjacencyList[i]);
  // }

  // Later, we add IP addresses.
  NS_LOG_INFO("Assign IP Addresses.");
  // Ipv4AddressHelper ipv4;
  // std::vector<Ipv4InterfaceContainer> interfaceAdjacencyList(N - 1);
  // for (uint32_t i = 0; i < interfaceAdjacencyList.size(); ++i) {
  //   std::ostringstream subnet;
  //   subnet << "10.1." << i + 1 << ".0";
  //   ipv4.SetBase(subnet.str().c_str(), "255.255.255.0");
  //   interfaceAdjacencyList[i] = ipv4.Assign(deviceAdjacencyList[i]);
  // }

  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface;
  apInterface = address.Assign(apDevice);
  Ipv4InterfaceContainer staInterface;
  staInterface = address.Assign(staDevices);

  // Turn on global static routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Create a packet sink on the star "hub" to receive these packets
  uint16_t port = 50000;
  Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
  PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApp = sinkHelper.Install(serverNode);
  sinkApp.Start(Seconds(1.0));
  sinkApp.Stop(Seconds(10.0));

  // Create the OnOff applications to send TCP to the server
  OnOffHelper server("ns3::TcpSocketFactory",
                     (InetSocketAddress(apInterface.GetAddress(0), 9)));
  server.SetAttribute("PacketSize", UintegerValue(payloadSize));
  server.SetAttribute("OnTime",
                      StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  server.SetAttribute("OffTime",
                      StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
  ApplicationContainer serverApp = server.Install(clientNodes);

  // normally wouldn't need a loop here but the server IP address is different
  // on each p2p subnet
  // ApplicationContainer clientApps;
  // for (uint32_t i = 0; i < clientNodes.GetN(); ++i) {
  //   AddressValue remoteAddress(
  //       InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0), port));
  //   clientHelper.SetAttribute("Remote", remoteAddress);
  //   clientApps.Add(clientHelper.Install(clientNodes.Get(i)));
  // }
  /* Start Applications */
  sinkApp.Start(Seconds(0.0));
  serverApp.Start(Seconds(1.0));

  /* Enable Traces */
  if (pcapTracing) {
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.EnablePcap("AccessPoint", apDevice);
    wifiPhy.EnablePcap("Station", staDevices);
  }

  // configure tracing
  // AsciiTraceHelper ascii;
  // p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"));
  // p2p.EnablePcapAll("tcp-star-server");

  /* Stop Simulation */
  Simulator::Stop(Seconds(simulationTime + 1));

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

  return 0;
}
