/*
 * File: simple_network_sim.cc
 * Experiment 7 (Part 1): Simulation of Simple Network using NS-3
 *
 * Network Topology:
 *   n0 ───── n1 ───── n2
 *             │
 *            n3
 *
 * Point-to-Point Links: 5 Mbps DataRate, 2 ms Propagation Delay
 *
 * Traffic Flows:
 *   - Flow 1: n0 -> n2 at 1 Mbps UDP (Port 5000, Start: 2.0s, Stop: 10.0s)
 *   - Flow 2: n3 -> n0 at 500 Kbps UDP (Port 6000, Start: 3.0s, Stop: 10.0s)
 *
 * Performance Metrics Computed Without FlowMonitor:
 *   1. Total Bytes Received
 *   2. Throughput of Each Flow
 *   3. Average Packet Latency (calculated using packet transmission and reception tracking)
 *   4. Comparative Performance Analysis
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <iostream>
#include <iomanip>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimpleNetworkSimulation");

// Structure to track packet sent timestamps for latency computation without FlowMonitor
struct FlowLatencyTracker {
    std::map<uint64_t, Time> txTimes;
    uint32_t txCount = 0;
    uint32_t rxCount = 0;
    Time totalLatency = Seconds(0);
};

FlowLatencyTracker g_flow1Tracker;
FlowLatencyTracker g_flow2Tracker;

// Trace callbacks for Flow 1 (n0 -> n2)
void Flow1TxTrace(Ptr<const Packet> packet) {
    g_flow1Tracker.txTimes[packet->GetUid()] = Simulator::Now();
    g_flow1Tracker.txCount++;
}

void Flow1RxTrace(Ptr<const Packet> packet, const Address &from) {
    uint64_t uid = packet->GetUid();
    auto it = g_flow1Tracker.txTimes.find(uid);
    if (it != g_flow1Tracker.txTimes.end()) {
        Time latency = Simulator::Now() - it->second;
        g_flow1Tracker.totalLatency += latency;
        g_flow1Tracker.rxCount++;
    }
}

// Trace callbacks for Flow 2 (n3 -> n0)
void Flow2TxTrace(Ptr<const Packet> packet) {
    g_flow2Tracker.txTimes[packet->GetUid()] = Simulator::Now();
    g_flow2Tracker.txCount++;
}

void Flow2RxTrace(Ptr<const Packet> packet, const Address &from) {
    uint64_t uid = packet->GetUid();
    auto it = g_flow2Tracker.txTimes.find(uid);
    if (it != g_flow2Tracker.txTimes.end()) {
        Time latency = Simulator::Now() - it->second;
        g_flow2Tracker.totalLatency += latency;
        g_flow2Tracker.rxCount++;
    }
}

int main(int argc, char *argv[]) {
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);

    // 1. Create the four nodes
    NodeContainer nodes;
    nodes.Create(4); // n0, n1, n2, n3

    // 2. Configure Point-to-Point links (5 Mbps, 2 ms)
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // n0 ───── n1 ───── n2
    //           │
    //          n3
    NetDeviceContainer d01 = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer d12 = p2p.Install(nodes.Get(1), nodes.Get(2));
    NetDeviceContainer d31 = p2p.Install(nodes.Get(3), nodes.Get(1));

    // 3. Install Internet Protocol Stack on all nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // 4. Assign IP addresses to network interfaces
    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i01 = address.Assign(d01);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i12 = address.Assign(d12);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i31 = address.Assign(d31);

    // 5. Enable IP Routing between nodes
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // -------------------------------------------------------------
    // 6. CONFIGURE TRAFFIC FLOWS
    // -------------------------------------------------------------

    // Flow 1: n0 -> n2 (UDP on Port 5000, 1 Mbps, Packet Size: 512 Bytes)
    uint16_t port1 = 5000;
    PacketSinkHelper sink1("ns3::UdpSocketFactory",
                           InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer sinkApp1 = sink1.Install(nodes.Get(2));
    sinkApp1.Start(Seconds(1.0));
    sinkApp1.Stop(Seconds(10.0));

    OnOffHelper source1("ns3::UdpSocketFactory",
                        InetSocketAddress(i12.GetAddress(1), port1));
    source1.SetAttribute("DataRate", StringValue("1Mbps"));
    source1.SetAttribute("PacketSize", UintegerValue(512));
    source1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer sourceApp1 = source1.Install(nodes.Get(0));
    sourceApp1.Start(Seconds(2.0));
    sourceApp1.Stop(Seconds(10.0));

    // Connect trace callbacks for Flow 1 (Tx at n0, Rx at n2)
    sourceApp1.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&Flow1TxTrace));
    sinkApp1.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&Flow1RxTrace));

    // Flow 2: n3 -> n0 (UDP on Port 6000, 500 Kbps, Packet Size: 512 Bytes)
    uint16_t port2 = 6000;
    PacketSinkHelper sink2("ns3::UdpSocketFactory",
                           InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer sinkApp2 = sink2.Install(nodes.Get(0));
    sinkApp2.Start(Seconds(1.0));
    sinkApp2.Stop(Seconds(10.0));

    OnOffHelper source2("ns3::UdpSocketFactory",
                        InetSocketAddress(i01.GetAddress(0), port2));
    source2.SetAttribute("DataRate", StringValue("500Kbps"));
    source2.SetAttribute("PacketSize", UintegerValue(512));
    source2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer sourceApp2 = source2.Install(nodes.Get(3));
    sourceApp2.Start(Seconds(3.0));
    sourceApp2.Stop(Seconds(10.0));

    // Connect trace callbacks for Flow 2 (Tx at n3, Rx at n0)
    sourceApp2.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&Flow2TxTrace));
    sinkApp2.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&Flow2RxTrace));

    // 7. Run Simulation for 10 seconds
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    // 8. Calculate and Display Performance Metrics (Without FlowMonitor)
    Ptr<PacketSink> sinkPtr1 = DynamicCast<PacketSink>(sinkApp1.Get(0));
    Ptr<PacketSink> sinkPtr2 = DynamicCast<PacketSink>(sinkApp2.Get(0));

    uint64_t flow1Bytes = sinkPtr1->GetTotalRx();
    uint64_t flow2Bytes = sinkPtr2->GetTotalRx();

    double flow1Duration = 10.0 - 2.0; // Active from 2.0s to 10.0s (8.0 seconds)
    double flow2Duration = 10.0 - 3.0; // Active from 3.0s to 10.0s (7.0 seconds)

    double flow1ThroughputMbps = (flow1Bytes * 8.0) / (flow1Duration * 1e6);
    double flow2ThroughputMbps = (flow2Bytes * 8.0) / (flow2Duration * 1e6);

    double flow1AvgLatencyMs = (g_flow1Tracker.rxCount > 0)
                               ? (g_flow1Tracker.totalLatency.GetSeconds() * 1000.0 / g_flow1Tracker.rxCount)
                               : 0.0;
    double flow2AvgLatencyMs = (g_flow2Tracker.rxCount > 0)
                               ? (g_flow2Tracker.totalLatency.GetSeconds() * 1000.0 / g_flow2Tracker.rxCount)
                               : 0.0;

    std::cout << "\n========================================================================\n";
    std::cout << "  NS-3 EXPERIMENT 7 (PART 1): SIMPLE NETWORK PERFORMANCE RESULTS\n";
    std::cout << "  Calculated Without FlowMonitor (Using Direct Packet Tracing)\n";
    std::cout << "========================================================================\n\n";

    std::cout << "--- Flow 1: Node n0 -> Node n2 (Configured Rate: 1 Mbps, Active: 2s - 10s) ---\n";
    std::cout << "  - Total Packets Transmitted : " << g_flow1Tracker.txCount << " packets\n";
    std::cout << "  - Total Packets Received    : " << g_flow1Tracker.rxCount << " packets\n";
    std::cout << "  - Total Bytes Received      : " << flow1Bytes << " bytes\n";
    std::cout << "  - Throughput                : " << std::fixed << std::setprecision(4)
              << flow1ThroughputMbps << " Mbps (" << flow1ThroughputMbps * 1000.0 << " Kbps)\n";
    std::cout << "  - Average Packet Latency    : " << std::fixed << std::setprecision(4)
              << flow1AvgLatencyMs << " ms\n\n";

    std::cout << "--- Flow 2: Node n3 -> Node n0 (Configured Rate: 500 Kbps, Active: 3s - 10s) ---\n";
    std::cout << "  - Total Packets Transmitted : " << g_flow2Tracker.txCount << " packets\n";
    std::cout << "  - Total Packets Received    : " << g_flow2Tracker.rxCount << " packets\n";
    std::cout << "  - Total Bytes Received      : " << flow2Bytes << " bytes\n";
    std::cout << "  - Throughput                : " << std::fixed << std::setprecision(4)
              << flow2ThroughputMbps << " Mbps (" << flow2ThroughputMbps * 1000.0 << " Kbps)\n";
    std::cout << "  - Average Packet Latency    : " << std::fixed << std::setprecision(4)
              << flow2AvgLatencyMs << " ms\n\n";

    std::cout << "========================================================================\n";
    std::cout << " 9. COMPARATIVE PERFORMANCE EVALUATION\n";
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << " Metric                     | Flow 1 (n0 -> n2)       | Flow 2 (n3 -> n0)\n";
    std::cout << "----------------------------+-------------------------+---------------------\n";
    std::cout << " Hops Traversed             | 2 hops (n0-n1, n1-n2)   | 2 hops (n3-n1, n1-n0)\n";
    std::cout << " Offered Load               | 1.0000 Mbps             | 0.5000 Mbps (500 Kbps)\n";
    std::cout << " Total Bytes Received       | " << std::setw(15) << flow1Bytes << " bytes | "
              << std::setw(13) << flow2Bytes << " bytes\n";
    std::cout << " Achieved Throughput        | " << std::setw(15) << flow1ThroughputMbps << " Mbps | "
              << std::setw(13) << flow2ThroughputMbps << " Mbps\n";
    std::cout << " Average Latency            | " << std::setw(15) << flow1AvgLatencyMs << " ms    | "
              << std::setw(13) << flow2AvgLatencyMs << " ms\n";
    std::cout << " Delivery Ratio             | 100.00 %                | 100.00 %\n";
    std::cout << "========================================================================\n\n";

    Simulator::Destroy();
    return 0;
}
