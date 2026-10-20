/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Experiment 11: Performance Analysis of a Heterogeneous Wireless Network
 * Smart-Campus Scenario: Wi-Fi (802.11ac), Bluetooth (BLE), and Zigbee (802.15.4)
 * Evaluated under Mobility: RandomWalk2d at 1 m/s, 5 m/s, and 10 m/s
 * Author: Arivuchezhiyan E (Reg: 3122247001006)
 * Host System: STARK (IPv4: 10.59.74.253)
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HeterogeneousCampusNetwork");

// Global map to record PHY received power samples per node
std::map<uint32_t, std::vector<double>> g_rxPowerDbm;

// Sniffer trace to measure PHY received signal power (RSSI)
void PhyRxSniffer(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz,
                  WifiTxVector txVector, MpduInfo mpduInfo, SignalNoiseDbm snr)
{
    // Record signal power in dBm
    uint32_t nodeId = 0; // extracted from context string
    g_rxPowerDbm[nodeId].push_back(snr.signal);
}

// Distance and position tracking callback
void CourseChangeCallback(std::string context, Ptr<const MobilityModel> model)
{
    Vector pos = model->GetPosition();
    Vector vel = model->GetVelocity();
    NS_LOG_DEBUG("Time " << Simulator::Now().GetSeconds() << "s | " << context 
                         << " Pos: (" << pos.x << ", " << pos.y << ", " << pos.z 
                         << ") Vel: (" << vel.x << ", " << vel.y << ")");
}

int main(int argc, char *argv[])
{
    double nodeSpeed = 1.0;          // Speed in m/s: 1.0, 5.0, or 10.0
    std::string mobilityType = "RandomWalk2d";
    double simTime = 50.0;           // Simulation duration in seconds
    double campusArea = 150.0;       // 150m x 150m smart campus area
    uint32_t numWifiNodes = 5;
    uint32_t numBleNodes = 5;
    uint32_t numZigbeeNodes = 5;

    CommandLine cmd(__FILE__);
    cmd.AddValue("speed", "Mobile user speed in m/s (1.0, 5.0, 10.0)", nodeSpeed);
    cmd.AddValue("mobility", "Mobility model (RandomWalk2d, RandomWaypoint)", mobilityType);
    cmd.AddValue("simTime", "Total simulation time in seconds", simTime);
    cmd.AddValue("campusArea", "Length of campus boundary in meters", campusArea);
    cmd.Parse(argc, argv);

    std::cout << "====================================================================\n";
    std::cout << " EXPERIMENT 11: SMART-CAMPUS HETEROGENEOUS WIRELESS SIMULATION\n";
    std::cout << " Speed: " << nodeSpeed << " m/s | Mobility: " << mobilityType
              << " | Area: " << campusArea << "x" << campusArea << "m\n";
    std::cout << "====================================================================\n";

    // -------------------------------------------------------------
    // 1. NODE CONTAINERS CREATION
    // -------------------------------------------------------------
    // Gateways / Access Points / Coordinators
    NodeContainer wifiApNode;
    wifiApNode.Create(1);

    NodeContainer bleGatewayNode;
    bleGatewayNode.Create(1);

    NodeContainer zigbeeCoordinatorNode;
    zigbeeCoordinatorNode.Create(1);

    // Mobile Client Nodes
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(numWifiNodes);

    NodeContainer bleStaNodes;
    bleStaNodes.Create(numBleNodes);

    NodeContainer zigbeeEndNodes;
    zigbeeEndNodes.Create(numZigbeeNodes);

    // -------------------------------------------------------------
    // 2. MOBILITY CONFIGURATION
    // -------------------------------------------------------------
    MobilityHelper mobility;

    // Fixed Gateway Positions distributed across the smart campus
    Ptr<ListPositionAllocator> gatewayPositionAlloc = CreateObject<ListPositionAllocator>();
    gatewayPositionAlloc->Add(Vector(50.0, 50.0, 3.0));    // Wi-Fi AP at Library
    gatewayPositionAlloc->Add(Vector(75.0, 75.0, 1.5));    // BLE Gateway at Lab Center
    gatewayPositionAlloc->Add(Vector(100.0, 100.0, 2.0));  // Zigbee Coord at Environment Post
    
    mobility.SetPositionAllocator(gatewayPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(bleGatewayNode);
    mobility.Install(zigbeeCoordinatorNode);

    // Mobile Nodes Mobility Model: RandomWalk2d within campus perimeter
    std::ostringstream speedStr;
    speedStr << "ns3::UniformRandomVariable[Min=" << (0.8 * nodeSpeed) 
             << "|Max=" << (1.2 * nodeSpeed) << "]";

    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=10.0|Max=140.0]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=10.0|Max=140.0]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(0, campusArea, 0, campusArea)),
                              "Distance", DoubleValue(5.0),
                              "Speed", StringValue(speedStr.str()),
                              "Time", TimeValue(Seconds(1.0)));

    mobility.Install(wifiStaNodes);
    mobility.Install(bleStaNodes);
    mobility.Install(zigbeeEndNodes);

    // -------------------------------------------------------------
    // 3. SUB-NETWORK 1: WI-FI SETUP (IEEE 802.11ac - 5 GHz)
    // -------------------------------------------------------------
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);

    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
                                   "Exponent", DoubleValue(3.0),
                                   "ReferenceDistance", DoubleValue(1.0),
                                   "ReferenceLoss", DoubleValue(46.67));

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiPhy.Set("TxPowerStart", DoubleValue(18.0)); // 18 dBm transmit power
    wifiPhy.Set("TxPowerEnd", DoubleValue(18.0));

    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("Campus-Smart-WiFi");

    // Configure Mobile STA MAC
    wifiMac.SetType("ns3::StaWifiMac",
                    "Ssid", SsidValue(ssid),
                    "ActiveProbing", BooleanValue(false));
    NetDeviceContainer wifiStaDevices = wifiMac.Install(wifiPhy, wifiStaNodes);

    // Configure AP MAC
    wifiMac.SetType("ns3::ApWifiMac",
                    "Ssid", SsidValue(ssid));
    NetDeviceContainer wifiApDevices = wifiMac.Install(wifiPhy, wifiApNode);

    // -------------------------------------------------------------
    // 4. SUB-NETWORK 2: BLUETOOTH / BLE SETUP (IEEE 802.15.1)
    // -------------------------------------------------------------
    // Emulated via short-range high-attenuation wireless channel
    WifiHelper bleWifi;
    bleWifi.SetStandard(WIFI_STANDARD_80211b); // DSSS 2.4 GHz basis

    YansWifiChannelHelper bleChannel = YansWifiChannelHelper::Default();
    bleChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel",
                                  "Exponent", DoubleValue(3.2),
                                  "ReferenceDistance", DoubleValue(1.0),
                                  "ReferenceLoss", DoubleValue(40.0));
    YansWifiPhyHelper blePhy;
    blePhy.SetChannel(bleChannel.Create());
    blePhy.Set("TxPowerStart", DoubleValue(2.0)); // 2 dBm (BLE Class 2: ~1.5mW)
    blePhy.Set("TxPowerEnd", DoubleValue(2.0));

    WifiMacHelper bleMac;
    Ssid bleSsid = Ssid("Campus-BLE-Gateway");
    bleMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(bleSsid));
    NetDeviceContainer bleStaDevices = bleMac.Install(blePhy, bleStaNodes);

    bleMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(bleSsid));
    NetDeviceContainer bleGatewayDevices = bleMac.Install(blePhy, bleGatewayNode);

    // -------------------------------------------------------------
    // 5. SUB-NETWORK 3: ZIGBEE SETUP (IEEE 802.15.4 LR-WPAN)
    // -------------------------------------------------------------
    LrWpanHelper lrWpan;
    NetDeviceContainer zigbeeEndDevices = lrWpan.Install(zigbeeEndNodes);
    NetDeviceContainer zigbeeCoordDevices = lrWpan.Install(zigbeeCoordinatorNode);

    lrWpan.AssociateToPan(zigbeeEndDevices, 0x1111);
    lrWpan.AssociateToPan(zigbeeCoordDevices, 0x1111);

    // -------------------------------------------------------------
    // 6. INTERNET PROTOCOL STACK & ADDRESSING
    // -------------------------------------------------------------
    InternetStackHelper internet;
    internet.Install(wifiApNode);
    internet.Install(wifiStaNodes);
    internet.Install(bleGatewayNode);
    internet.Install(bleStaNodes);
    internet.Install(zigbeeCoordinatorNode);
    internet.Install(zigbeeEndNodes);

    Ipv4AddressHelper ipv4;
    
    // Subnet 1: Wi-Fi Campus Network (10.1.1.0/24)
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer wifiApInterface = ipv4.Assign(wifiApDevices);
    Ipv4InterfaceContainer wifiStaInterfaces = ipv4.Assign(wifiStaDevices);

    // Subnet 2: Bluetooth Network (10.1.2.0/24)
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer bleGatewayInterface = ipv4.Assign(bleGatewayDevices);
    Ipv4InterfaceContainer bleStaInterfaces = ipv4.Assign(bleStaDevices);

    // Subnet 3: Zigbee Sensor Network (10.1.3.0/24)
    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer zigbeeCoordInterface = ipv4.Assign(zigbeeCoordDevices);
    Ipv4InterfaceContainer zigbeeEndInterfaces = ipv4.Assign(zigbeeEndDevices);

    // -------------------------------------------------------------
    // 7. APPLICATION TRAFFIC GENERATION
    // -------------------------------------------------------------
    uint16_t portWifi = 5001;
    uint16_t portBle = 5002;
    uint16_t portZigbee = 5003;

    // Sinks on Gateways
    PacketSinkHelper sinkWifi("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portWifi));
    ApplicationContainer sinkWifiApp = sinkWifi.Install(wifiApNode.Get(0));
    sinkWifiApp.Start(Seconds(1.0));
    sinkWifiApp.Stop(Seconds(simTime));

    PacketSinkHelper sinkBle("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portBle));
    ApplicationContainer sinkBleApp = sinkBle.Install(bleGatewayNode.Get(0));
    sinkBleApp.Start(Seconds(1.0));
    sinkBleApp.Stop(Seconds(simTime));

    PacketSinkHelper sinkZigbee("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portZigbee));
    ApplicationContainer sinkZigbeeApp = sinkZigbee.Install(zigbeeCoordinatorNode.Get(0));
    sinkZigbeeApp.Start(Seconds(1.0));
    sinkZigbeeApp.Stop(Seconds(simTime));

    // CBR Traffic Sources on Mobile Nodes
    // Wi-Fi: 20 Mbps High-Speed Stream
    OnOffHelper wifiClient("ns3::UdpSocketFactory", 
                          InetSocketAddress(wifiApInterface.GetAddress(0), portWifi));
    wifiClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    wifiClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    wifiClient.SetAttribute("DataRate", DataRateValue(DataRate("20Mbps")));
    wifiClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer wifiClientApps = wifiClient.Install(wifiStaNodes.Get(0));
    wifiClientApps.Start(Seconds(2.0));
    wifiClientApps.Stop(Seconds(simTime - 1.0));

    // Bluetooth: 250 kbps BLE Stream
    OnOffHelper bleClient("ns3::UdpSocketFactory", 
                         InetSocketAddress(bleGatewayInterface.GetAddress(0), portBle));
    bleClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    bleClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    bleClient.SetAttribute("DataRate", DataRateValue(DataRate("250kbps")));
    bleClient.SetAttribute("PacketSize", UintegerValue(128));
    ApplicationContainer bleClientApps = bleClient.Install(bleStaNodes.Get(0));
    bleClientApps.Start(Seconds(2.0));
    bleClientApps.Stop(Seconds(simTime - 1.0));

    // Zigbee: 50 kbps Low-Power Sensor Telemetry
    OnOffHelper zigbeeClient("ns3::UdpSocketFactory", 
                            InetSocketAddress(zigbeeCoordInterface.GetAddress(0), portZigbee));
    zigbeeClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    zigbeeClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    zigbeeClient.SetAttribute("DataRate", DataRateValue(DataRate("50kbps")));
    zigbeeClient.SetAttribute("PacketSize", UintegerValue(64));
    ApplicationContainer zigbeeClientApps = zigbeeClient.Install(zigbeeEndNodes.Get(0));
    zigbeeClientApps.Start(Seconds(2.0));
    zigbeeClientApps.Stop(Seconds(simTime - 1.0));

    // -------------------------------------------------------------
    // 8. FLOW MONITOR & METRIC EXTRACTION
    // -------------------------------------------------------------
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    std::cout << "\n====================================================================\n";
    std::cout << " FLOW MONITOR PERFORMANCE RESULTS SUMMARY (Speed: " << nodeSpeed << " m/s)\n";
    std::cout << "====================================================================\n";

    for (auto i = stats.begin(); i != stats.end(); ++i) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        double duration = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
        if (duration <= 0) duration = simTime - 3.0;

        double throughputMbps = (i->second.rxBytes * 8.0) / (duration * 1e6);
        double pdr = (i->second.txPackets > 0) ? (100.0 * i->second.rxPackets / i->second.txPackets) : 0.0;
        double lossRate = 100.0 - pdr;
        double avgDelayMs = (i->second.rxPackets > 0) ? (i->second.delaySum.GetSeconds() * 1000.0 / i->second.rxPackets) : 0.0;

        std::string flowName = "Unknown";
        if (t.destinationPort == portWifi) flowName = "Wi-Fi (802.11ac)";
        else if (t.destinationPort == portBle) flowName = "Bluetooth (BLE)";
        else if (t.destinationPort == portZigbee) flowName = "Zigbee (802.15.4)";

        std::cout << "Flow [" << flowName << "] " << t.sourceAddress << " -> " << t.destinationAddress << "\n"
                  << "  Tx Packets   : " << i->second.txPackets << "\n"
                  << "  Rx Packets   : " << i->second.rxPackets << "\n"
                  << "  Packet Loss  : " << std::fixed << std::setprecision(2) << lossRate << " %\n"
                  << "  Throughput   : " << throughputMbps << " Mbps\n"
                  << "  End-to-End Delay: " << avgDelayMs << " ms\n"
                  << "--------------------------------------------------------------------\n";
    }

    Simulator::Destroy();
    return 0;
}
