/* ========================================================================
 * ZASH Simulation
 *
 * Author:  Giovanni Rosa da Silva (grsilva@inf.ufpr.br)
 * ========================================================================
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <sstream>
#include <string>

#include "ns3/address-utils.h"
#include "ns3/address.h"
#include "ns3/application-container.h"
#include "ns3/applications-module.h"
#include "ns3/attribute.h"
#include "ns3/callback.h"
#include "ns3/core-module.h"
#include "ns3/data-rate.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/names.h"
#include "ns3/net-device.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/object-factory.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/socket.h"
#include "ns3/stats-module.h"
#include "ns3/string.h"
#include "ns3/tag.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/traced-callback.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/udp-socket.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-module.h"
#include "sys/stat.h"
#include "sys/types.h"

// Created files
#include "models_zash.h"
// #include "ns3/statistics.h"
// #include "ns3/status-on.h"
// #include "ns3/stealth-on-off.h"
// #include "ns3/stealth-packet-sink.h"
// #include "ns3/stealth-tag.h"
// #include "ns3/utils.h"

/*
 * Model:
 *
 * 	   WiFi (Ad hoc)
 *  Node1		  Node2
 *  Source		  Sink
 *   (*) --------> (*)
 *  10.0.0.1	 10.0.0.2
 *  OnOff		 OnOff		>> Search neighbors
 * PacketSink	 PacketSink	>> Receive messages and answers
 * StatusOn		 StatusOn	>> Controls emergency situation
 */

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("CenarioZASH");

#define NUMBER_OF_DEVICES 29
#define DATE_COL 30

/* ========================================================================
 * Experiment
 * ========================================================================
 */

void ZASHSimulation(uint32_t nNodes, string simDate, uint32_t fixNodeNumber,
                    char runMode) {
    // string label;
    // switch (runMode) {
    //     case 'S':
    //         label = "Standard";
    //         break;
    //     case 'B':
    //         label = "Before";
    //         break;
    //     case 'E':
    //         label = "Equal";
    //         break;
    //     case 'A':
    //         label = "After";
    //         break;
    //     default:
    //         label = "";
    // }
    NS_LOG_INFO("ZASH - Setting parameters...");

    vector<User> users = { User(1, UserLevel.ADMIN, Age.ADULT),
                           User(2, UserLevel.ADULT, Age.ADULT),
                           User(3, UserLevel.CHILD, Age.TEEN),
                           User(4, UserLevel.CHILD, Age.KID),
                           User(5, UserLevel.VISITOR, Age.ADULT) };

    vector<Device> devices = {}

    double start = 0.0;
    double stop = 900.0;
    string traceFile = "scratch/d6_2m_0tm.csv";
    // string traceFile = "scratch/mobilitymodel.tr";
    string scenarioSimFile;
    string tracesFolder;
    vector<int> healthNodes;     // Store nodes with health interest
    vector<int> emergencyTimes;  // Store times to nodes enters in emergency
    vector<string> interestsTemp;
    vector<int> fixedNodes;
    vector<int> fixedEmergTimes;
    // Fixed Nodes and Emergency times depending on Mode
    if (runMode == 'S') {
        fixedNodes = {52, 37, 70};  // 480s
        fixedEmergTimes = {480};
    } else {
        // fixedNodes = {0, 5, /*90s*/ 13, 16, /*110s*/ 25, 32 /*210s*/};
        // fixedEmergTimes = {90, 110, 210};
        fixedNodes = {31, 54, /*360s*/ 71, 99, /*300s*/ 93, 96 /*350s*/};
        fixedEmergTimes = {360, 300, 350};
    }

    NodeContainer::Iterator it;
    uint16_t port = 9;
    int i = 0;
    int x = 0;

    // Creating and defining seeds to be used
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e(seed);

    // Vector to store interests to be distributed to all nodes
    vector<vector<string>> interestsTo(100, vector<string>(5, " "));
    vector<string> competencesTo(100, " ");

    // Vector to store interests to be distributed to a fixed node
    vector<string> fixInterest = {"health", "tourism", "music", "movies",
                                  "books"};
    // Fixed competence is other for Standard Mode and doctor for alternatives
    string fixCompetence = runMode == 'S' ? "other" : "doctor";

    //----------------------------------------------------------------------------------
    // Create a folder for traces (stealth_traces) inside ns3 folder
    //----------------------------------------------------------------------------------

    tracesFolder = "stealth_traces/";
    errno = 0;
    int folder =
        mkdir(tracesFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (folder < 0 && errno != EEXIST)
        cout << "Fail creating folder for traces!" << endl;

    tracesFolder.append(simDate.substr(0, simDate.size() - 2).c_str());
    tracesFolder.append("/");

    // Creates a folder for specific simulation
    folder = mkdir(tracesFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (folder == -1)
        cout << "Fail creating sub folder for specific traces!" << folder << endl;

    ostringstream convert;
    convert << tracesFolder.c_str() << "stealth_simulation_scenario_"
            << simDate.substr(0, simDate.size() - 2).c_str() << ".txt";
    scenarioSimFile = convert.str();

    // Create a string stream to store simulation scenario data
    stringstream fileSim;

    // Save start seed in file
    fileSim << "Start seed: " << seed << endl
            << endl;

    // Competences to be distributed to all nodes
    // Only one per node
    vector<string> competences = {"doctor", "nurse", "caregiver", "other"};
    vector<int> nCompetences = {10, 15, 20, 55};

    // Save competences and its distribution in scenario configuration data file
    fileSim << "Competences: " << competences[0].c_str() << ", "
            << competences[1].c_str() << ", " << competences[2].c_str() << ", "
            << competences[3].c_str() << endl;

    fileSim << "Competences distribution: " << nCompetences[0] << ", "
            << nCompetences[1] << ", " << nCompetences[2] << ", "
            << nCompetences[3] << endl
            << endl;

    // Interests to be distributed to all nodes
    // At least one interest per node and a maximum of all interests
    vector<string> interestss = {"health", "tourism", "music", "movies", "books"};
    vector<int> nInterests = {20, 30, 45, 60, 15};

    // Create and distribute interests and competences to assign to nodes
    competencesTo = DistributeCompetences(competences, nCompetences, 100, seed);
    interestsTo = DistributeInterests(interestss, nInterests, 100, seed);

    // If a node has to be fixed during simulation, change its
    // interests and competence to fixed
    if (fixNodeNumber != 0) {
        for (i = 0; i < (int)fixNodeNumber; i++) {
            // Assign a competence to fixed nodes
            competencesTo[fixedNodes[i]] = fixCompetence.c_str();
            // Assign interests to fixed node
            interestsTo[fixedNodes[i]].clear();
            for (x = 0; x < (int)fixInterest.size(); x++)
                interestsTo[fixedNodes[i]].push_back(fixInterest[x].c_str());
        }
    }
    // Get the nodes with health interest to have status changed during simulation
    healthNodes = GetsHealthInterestNodes(interestsTo);

    // Import node's mobility from the trace file
    // Necessary to use a helper from NS2
    Ns2MobilityHelper ns2 = Ns2MobilityHelper(traceFile);

    // Create nodes to be used during simulation
    NodeContainer Nodes;
    Nodes.Create(nNodes);

    // Install node's mobility in all nodes
    ns2.Install();

    // Initialize node's status, service status, attending and shutdown
    for (it = Nodes.Begin(); it != Nodes.End(); it++) {
        (*it)->SetAttribute("Status", BooleanValue(false));
        (*it)->SetAttribute("ServiceStatus", BooleanValue(false));
        (*it)->SetAttribute("ServiceTime", DoubleValue(0.0));
        (*it)->SetAttribute("Attending", BooleanValue(false));
        (*it)->SetAttribute("Shutdown", BooleanValue(false));
    }
    //----------------------------------------------------------------------------------
    // Assign competence and interests to nodes
    //----------------------------------------------------------------------------------

    NS_LOG_INFO("STEALTH - Assign competence and interests ...");

    // Store competences and its distribution to save in a scenario file
    fileSim << "Competences assigned to nodes:" << endl;

    for (i = 0; i < 100; i++)  // Pass through all nodes
        fileSim << "Node[" << i << "]: " << competencesTo[i].c_str() << endl;
    fileSim << endl;

    fileSim << "Interests: " << interestss[0].c_str() << ", "
            << interestss[1].c_str() << ", " << interestss[2].c_str() << ", "
            << interestss[3].c_str() << ", " << interestss[4].c_str() << endl;

    fileSim << "Interests distribution: " << nInterests[0] << ", "
            << nInterests[1] << ", " << nInterests[2] << ", " << nInterests[3]
            << ", " << nInterests[4] << endl
            << endl;

    fileSim << "Interests assigned to nodes:" << endl;
    // ----------------------------------------------------------------------

    // Assign Competence and Interests for all nodes used in the simulations
    for (i = 0; i < 100; i++)  // Pass through all nodes
    {
        stringstream nodeInterests;
        interestsTemp.clear();
        nodeInterests << "Node[" << i << "]: ";
        for (x = 0; x < (int)interestsTo[i].size();
             x++)  // Pass through all the interests to be distributed to
        {
            if (interestsTo[i][x].empty() == true)
                break;  // break if no more interests inside vector
            else {
                nodeInterests << interestsTo[i][x].c_str() << ", ";
                interestsTemp.push_back(interestsTo[i][x]);
            }
        }

        fileSim << nodeInterests.str().substr(0, nodeInterests.str().size() - 2)
                << endl;

        // Assign competence to the node i
        Nodes.Get(i)->SetAttribute("Competence", StringValue(competencesTo[i]));
        // Assign interests to the node i
        Nodes.Get(i)->SetInterests(interestsTemp);
        interestsTemp.clear();
    }

    fileSim << endl;

    //----------------------------------------------------------------------------------
    // Set wifi network - Ad Hoc
    //----------------------------------------------------------------------------------

    NS_LOG_INFO("STEALTH - Configuring wifi network (Ad Hoc)...");

    // Create wifi network 802.11a

    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                                 StringValue("OfdmRate6Mbps"), "RtsCtsThreshold",
                                 UintegerValue(0));

    // MAC Layer non QoS

    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac");

    // PHY layer

    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel(wifiChannel.Create());

    // Creating and installing netdevices in all nodes
    NetDeviceContainer devices;
    devices = wifi.Install(wifiPhy, wifiMac, Nodes);

    // Create and install Internet stack protocol
    InternetStackHelper stack;
    stack.Install(Nodes);

    // Set IPv4 address to node's interfaces
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer NodesInterface;
    address.SetBase("192.168.1.0", "255.255.255.0");

    NodesInterface = address.Assign(devices);

    //----------------------------------------------------------------------------------
    // Install applications
    //----------------------------------------------------------------------------------

    NS_LOG_INFO("STEALTH - Install applications ...");

    //----------------------------------------------------------------------------------
    // Set Sink application
    //----------------------------------------------------------------------------------

    Address SinkBroadAddress(InetSocketAddress(Ipv4Address::GetAny(),
                                               port));  // SinkAddress for messages

    NS_LOG_INFO("STEALTH - Install Sink application ...");

    //----------------------------------------------------------------------------------
    // Set Sink application
    //----------------------------------------------------------------------------------
    // Install Sink in all nodes

    for (it = Nodes.Begin(); it != Nodes.End(); it++) {
        Ptr<StealthPacketSink> SinkApp = CreateObject<StealthPacketSink>();
        (*it)->AddApplication(SinkApp);

        SinkApp->SetStartTime(Seconds(start));
        SinkApp->SetStopTime(Seconds(stop));

        SinkApp->Setup(SinkBroadAddress, 1, competences);  // 1 -> UDP, 2 -> TCP
    }

    //----------------------------------------------------------------------------------
    // Set OnOff application
    //----------------------------------------------------------------------------------

    // Install OnOff in all nodes

    NS_LOG_INFO("STEALTH - Install OnOff application ...");

    for (it = Nodes.Begin(); it != Nodes.End(); it++) {
        Ptr<StealthOnOff> OnOffApp = CreateObject<StealthOnOff>();
        (*it)->AddApplication(OnOffApp);

        // Set to send to broadcast address
        OnOffApp->Setup(InetSocketAddress(Ipv4Address("255.255.255.255"), 9),
                        1);  // 1 -> UDP, 2 -> TCP

        OnOffApp->SetAttribute(
            "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        OnOffApp->SetAttribute(
            "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=3]"));
        OnOffApp->SetAttribute("DataRate", StringValue("500kb/s"));
        OnOffApp->SetAttribute("PacketSize", UintegerValue(6));

        OnOffApp->SetStartTime(Seconds(start));
        OnOffApp->SetStopTime(Seconds(stop));
        start += 0.2;  // Avoid to start all the OnOff together
    }

    Statistics statistics(simDate, tracesFolder.c_str());

    //----------------------------------------------------------------------------------
    // Set StatusOn application
    //----------------------------------------------------------------------------------

    // Generate times to nodes enter in emergency
    emergencyTimes = GenerateEmergTimes((int)healthNodes.size(), seed);

    // Install StatusOn only in nodes with health interest
    NS_LOG_INFO("STEALTH - Install StatusOn application ...");

    it = Nodes.Begin();
    cout << "Healthy nodes: ";
    fileSim << "Healthy nodes: ";
    stringstream nodesHealthy;

    for (i = 0; i != (int)healthNodes.size();
         i++)  // Install in all nodes with health interest
    {
        bool isEqual = false;
        bool isSecond = false;  // Second Node from the pair
        Ptr<StatusOn> StatusOnApp = CreateObject<StatusOn>();
        (*(it + healthNodes[i]))->AddApplication(StatusOnApp);
        (*(it + healthNodes[i]));
        // Set to send UDP messages
        StatusOnApp->Setup(1, competences);  // 1 -> UDP, 2 -> TCP

        StatusOnApp->SetAttribute(
            "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        StatusOnApp->SetAttribute(
            "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=3]"));
        StatusOnApp->SetAttribute("DataRate", StringValue("500kb/s"));
        StatusOnApp->SetAttribute("PacketSize", UintegerValue(6));
        StatusOnApp->SetStopTime(Seconds(stop));
        int t;
        if (fixNodeNumber != 0)  // checks for fix node
        {
            for (x = 0; x < (int)fixNodeNumber; x++)
                if (fixedNodes[x] == healthNodes[i]) {
                    isEqual = true;
                    isSecond = x % 2 != 0;  // if is the second from the pairs
                    t = x / 2;              // time index from Fixed Emergency Times array
                }
        }
        int fixTime;
        if (runMode == 'S') {
            fixTime = fixedEmergTimes[0];  // [480]
        } else {
            fixTime = fixedEmergTimes[t];  // [90, 110, 200]
        }
        if (isSecond) {
            int plus = 0;
            if (runMode == 'B')  // subtract 10 from second node
                plus = -10;
            else if (runMode == 'A')  // add 10 from second node
                plus = 10;
            fixTime += plus;
            StatusOnApp->SetStartTime(Seconds(fixTime));  // fix node emergency time
            cout << healthNodes[i] << " (" << fixTime << "), ";
        } else if (isEqual) {
            StatusOnApp->SetStartTime(Seconds(fixTime));  // fix node emergency time
            cout << healthNodes[i] << " (" << fixTime << "), ";
        } else {
            StatusOnApp->SetStartTime(Seconds(emergencyTimes[i]));
            cout << healthNodes[i] << " (" << emergencyTimes[i] << "), ";
        }
        nodesHealthy << healthNodes[i] << ", ";
    }
    cout << endl;

    fileSim << nodesHealthy.str().substr(0, nodesHealthy.str().size() - 2)
            << endl;

    stringstream timesEmerg;
    fileSim << "Emergency times: ";
    for (i = 0; i != (int)emergencyTimes.size(); i++)
        timesEmerg << emergencyTimes[i] << ", ";
    fileSim << timesEmerg.str().substr(0, timesEmerg.str().size() - 2) << endl;

    stringstream fixedTimes;
    copy(fixedEmergTimes.begin(), fixedEmergTimes.end(),
         std::ostream_iterator<int>(fixedTimes, ", "));
    if (fixNodeNumber != 0) {
        fileSim << "Fixed Node(s): ";
        for (i = 0; i < (int)fixNodeNumber; i++)
            fileSim << fixedNodes[i] << " ";
        fileSim << "- emergency time(s): " << fixedTimes.str().c_str() << endl;
    } else
        fileSim << endl;

    //----------------------------------------------------------------------------------
    // Saving simulation scenario data
    //----------------------------------------------------------------------------------

    // Create a file and save simulation scenario data
    NS_LOG_INFO("STEALTH - Saving simulation scenario data ...");
    NS_LOG_INFO(tracesFolder.c_str());

    CreateSimScenarioFile(scenarioSimFile.c_str(), simDate, fileSim.str());

    //----------------------------------------------------------------------------------
    // Callback configuration
    //----------------------------------------------------------------------------------
    NS_LOG_INFO("STEALTH - Configuring callbacks ...");

    // Callback Trace to Collect Emergency Messages in StatusOn Application
    // Installed only in nodes with health interest 15Nov18
    it = Nodes.Begin();
    for (uint8_t i = 0; i != healthNodes.size(); i++) {
        uint32_t nodeID = (*(it + healthNodes[i]))->GetId();
        ostringstream paramTest;
        paramTest << "/NodeList/" << (nodeID)
                  << "/ApplicationList/*/$ns3::StatusOn/EmergNodes";
        Config::Connect(paramTest.str().c_str(),
                        MakeCallback(&Statistics::EmergencyCallback, &statistics));
    }

    // Callback Trace to Collect data from StealthOnOff Application
    // Installed only in nodes with health interest, for while 15Nov18
    for (uint8_t i = 0; i != healthNodes.size(); i++) {
        uint32_t nodeID = (*(it + healthNodes[i]))->GetId();
        ostringstream paramTest;
        paramTest << "/NodeList/" << (nodeID)
                  << "/ApplicationList/*/$ns3::StealthOnOff/Broadcasts";
        Config::Connect(paramTest.str().c_str(),
                        MakeCallback(&Statistics::BroadcastCallback, &statistics));
    }

    // Callback Trace to Collect data from StealthPacketSink Application
    // Installed in all nodes 15Nov18
    for (it = Nodes.Begin(); it != Nodes.End(); it++) {
        uint32_t nodeID = (*it)->GetId();
        ostringstream paramTest;
        paramTest << "/NodeList/" << (nodeID)
                  << "/ApplicationList/*/$ns3::StealthPacketSink/SinkTraces";
        Config::Connect(paramTest.str().c_str(),
                        MakeCallback(&Statistics::ReceiverCallback, &statistics));
    }

    //----------------------------------------------------------------------------------
    // Enabling pcap - Disabled
    //----------------------------------------------------------------------------------
    // wifiPhy.EnablePcapAll("StealthSimulation",true);

    //----------------------------------------------------------------------------------
    // Animation Interface NetAnim
    //----------------------------------------------------------------------------------

    // (Insert code here!)

    //----------------------------------------------------------------------------------
    // Start / Stop simulation
    //----------------------------------------------------------------------------------

    NS_LOG_INFO("STEALTH - Starting Simulation...");
    Simulator::Stop(Seconds(900));
    Simulator::Run();
    Simulator::Destroy();

    //----------------------------------------------------------------------------------
    // Tracing
    //----------------------------------------------------------------------------------

    cout << "----------------- Total data -----------------" << endl;
    cout << "Total broadcasts received: " << statistics.m_broadcastReceived
         << endl;
    cout << "Total identification received: " << statistics.m_idReceived << endl;
    cout << "Total emergency messages sent: " << statistics.m_emergMsgSent
         << endl;
    cout << "Total emergency messages received: " << statistics.m_emergMsgReceived
         << endl;
    cout << "Total emergency messages confirmed: " << statistics.m_emergMsgACK
         << endl;
    cout << "Total attending interrupted: " << statistics.m_attInterrupted
         << endl;

    //----------------------------------------------------------------------------------
    // Metrics - calcs
    //----------------------------------------------------------------------------------

    // (Insert code here!)
}

/* ------------------------------------------------------------------------
 * End of Experiment
 * ------------------------------------------------------------------------
 */

/* ========================================================================
 * Main
 * ========================================================================
 */

int main(int argc, char *argv[]) {
    // Set Parameters
    uint32_t nNodes = 100;
    string simTime;
    //   uint32_t fixNode = 0;
    //   char runMode = 'S';
    //   set<char> runModeSet = {'S', 'B', 'E', 'A'};
    /*
    runMode = S: Standard Execution
    runMode = B: Testing Before Case
    runMode = E: Testing Equal Case
    runMode = A: Testing After Case */

    LogComponentEnable("CenarioZASH", LOG_LEVEL_INFO);

    NS_LOG_INFO("ZASH - Initializing...");

    CommandLine cmd;
    cmd.AddValue("nNodes", "Number of node devices", nNodes);
    //   cmd.AddValue("fixNode", "Number of nodes with fixed settings", fixNode);
    //   cmd.AddValue("runMode", "Mode of simulation execution", runMode);
    cmd.Parse(argc, argv);

    //   if (runModeSet.find(runMode) == runModeSet.end()) {
    //     cout << "ZASH - Error: runMode supports the following:\n"
    //          << "\t- S: Standard\n"
    //          << "\t- B: Before\n"
    //          << "\t- E: Equal\n"
    //          << "\t- A: After" << endl;
    //     NS_LOG_INFO("ZASH - Done!...");
    //     return 1;
    //   }

    simTime = GetTimeOfSimulationStart();

    // SeedManager::SetRun(nRun); // update seed to n executions

    // Check if fixNode is below 4 for Standard mode or
    // if it's below 7 and even for other modes
    //   if ((fixNode < 4 && runMode == 'S') ||
    //       (fixNode < 7 && (fixNode % 2 == 0) && runMode != 'S'))
    ZASHSimulation(nNodes, simTime, fixNode, runMode);
    //   else {
    //     cout << "ZASH - Error: Maximum 3 fixed nodes for Standard mode and 6 "
    //             "for others! Must be even number for modes other than Standard!"
    //          << endl;
    //     NS_LOG_INFO("ZASH - Done!...");
    //     return 1;
    //   }

    NS_LOG_INFO("ZASH - Done!...");

    return 0;
}

/* ------------------------------------------------------------------------
 * End of Main
 * ------------------------------------------------------------------------
 */
