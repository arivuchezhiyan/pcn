/*
 * File: udp_port_delivery_sim.cc
 * Experiment 7 (Part 2): UDP Traffic and Port-Based Application Delivery
 *
 * Requirements:
 *  1. Topology: Two nodes (Node1 and Node2) connected by a Point-to-Point link.
 *  2. Application 1: UdpClient (Node1) -> UdpServer (Node2) on Port 5000.
 *  3. Application 2: UdpClient (Node1) -> UdpServer (Node2) on Port 6000.
 *  4. Both applications share the same IP addresses; UDP destination port numbers
 *     are used by the operating system stack to demultiplex incoming packets.
 *  5. Evaluates effect of varying Point-to-Point Link Latency:
 *     - 2 ms
 *     - 5 ms
 *     - 10 ms
 *     - 20 ms
 *  6. Enables PCAP packet tracing to inspect with Wireshark.
 *  7. Displays Flow 1 & Flow 2 Throughput, Bytes Received, and Packet Delivery.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpPortDeliverySimulation");

int main(int argc, char *argv[])
{
    std::string linkDelay = "2ms";       // Configurable: 2ms, 5ms, 10ms, 20ms
    std::string linkBw = "5Mbps";        // Link bandwidth
    std::string appRate = "1Mbps";       // Traffic rate for each flow
    double simTime = 10.0;               // Total simulation duration
    bool enablePcap = true;

    CommandLine cmd(__FILE__);
    cmd.AddValue("delay", "Point-to-point link propagation delay (2ms, 5ms, 10ms, 20ms)", linkDelay);
    cmd.AddValue("dataRate", "Point-to-point link bandwidth", linkBw);
    cmd.AddValue("appRate", "UDP application data rate", appRate);
    cmd.AddValue("simTime", "Total simulation duration in seconds", simTime);
    cmd.AddValue("pcap", "Enable PCAP packet tracing", enablePcap);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);

    std::cout << "========================================================================\n";
    std::cout << "  NS-3 EXPERIMENT 7 (PART 2): UDP TRAFFIC & PORT-BASED DELIVERY\n";
    std::cout << "  Link Latency : " << linkDelay << " | Bandwidth: " << linkBw << "\n";
    std::cout << "  Application 1: Node1 -> Node2 on Port 5000 (Data Rate: " << appRate << ")\n";
    std::cout << "  Application 2: Node1 -> Node2 on Port 6000 (Data Rate: " << appRate << ")\n";
    std::cout << "========================================================================\n\n";

    // 1. Create two nodes (Node1: Client, Node2: Server)
    NodeContainer nodes;
    nodes.Create(2);

    // 2. Configure Point-to-Point link
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue(linkBw));
    p2p.SetChannelAttribute("Delay", StringValue(linkDelay));

    NetDeviceContainer devices = p2p.Install(nodes);

    // 3. Install Internet Protocol Stack on both nodes
    InternetStackHelper internet;
    internet.Install(nodes);

    // 4. Assign IP addresses (10.1.1.0/24)
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    Ipv4Address serverIp = interfaces.GetAddress(1); // Node2 IP

    // -------------------------------------------------------------
    // 5. APPLICATION 1 (Port 5000)
    // -------------------------------------------------------------
    uint16_t port1 = 5000;
    UdpServerHelper server1(port1);
    ApplicationContainer serverApp1 = server1.Install(nodes.Get(1)); // Server 1 on Node2
    serverApp1.Start(Seconds(1.0));
    serverApp1.Stop(Seconds(simTime));

    // Client 1 on Node1 transmitting 512-byte packets at ~1 Mbps
    UdpClientHelper client1(serverIp, port1);
    client1.SetAttribute("MaxPackets", UintegerValue(10000));
    client1.SetAttribute("Interval", TimeValue(MicroSeconds(4096))); // 512 B * 8 / 4096 us = 1 Mbps
    client1.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer clientApp1 = client1.Install(nodes.Get(0));
    clientApp1.Start(Seconds(2.0));
    clientApp1.Stop(Seconds(simTime));

    // -------------------------------------------------------------
    // 6. APPLICATION 2 (Port 6000)
    // -------------------------------------------------------------
    uint16_t port2 = 6000;
    UdpServerHelper server2(port2);
    ApplicationContainer serverApp2 = server2.Install(nodes.Get(1)); // Server 2 on Node2
    serverApp2.Start(Seconds(1.0));
    serverApp2.Stop(Seconds(simTime));

    // Client 2 on Node1 transmitting on Port 6000
    UdpClientHelper client2(serverIp, port2);
    client2.SetAttribute("MaxPackets", UintegerValue(10000));
    client2.SetAttribute("Interval", TimeValue(MicroSeconds(4096)));
    client2.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer clientApp2 = client2.Install(nodes.Get(0));
    clientApp2.Start(Seconds(2.0));
    clientApp2.Stop(Seconds(simTime));

    // 7. Enable PCAP tracing for Wireshark analysis
    if (enablePcap) {
        p2p.EnablePcapAll("udp-port-delivery-" + linkDelay);
    }

    // Run Simulation
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // 8. Extract Metrics from UdpServer & UdpClient
    Ptr<UdpServer> srvPtr1 = DynamicCast<UdpServer>(serverApp1.Get(0));
    Ptr<UdpServer> srvPtr2 = DynamicCast<UdpServer>(serverApp2.Get(0));

    uint32_t rx1 = srvPtr1->GetReceived();
    uint32_t rx2 = srvPtr2->GetReceived();

    double activeDuration = simTime - 2.0; // 8.0 seconds (2.0s to 10.0s)
    uint64_t bytes1 = (uint64_t)rx1 * 512;
    uint64_t bytes2 = (uint64_t)rx2 * 512;

    double tp1 = (bytes1 * 8.0) / (activeDuration * 1e6);
    double tp2 = (bytes2 * 8.0) / (activeDuration * 1e6);

    std::cout << "--- Performance Results for Link Latency = " << linkDelay << " ---\n";
    std::cout << "  Flow 1 (Port 5000 -> Server 1):\n";
    std::cout << "    - Packets Received   : " << rx1 << " packets\n";
    std::cout << "    - Total Bytes        : " << bytes1 << " bytes\n";
    std::cout << "    - Throughput         : " << std::fixed << std::setprecision(4) << tp1 << " Mbps\n\n";

    std::cout << "  Flow 2 (Port 6000 -> Server 2):\n";
    std::cout << "    - Packets Received   : " << rx2 << " packets\n";
    std::cout << "    - Total Bytes        : " << bytes2 << " bytes\n";
    std::cout << "    - Throughput         : " << std::fixed << std::setprecision(4) << tp2 << " Mbps\n\n";

    std::cout << "------------------------------------------------------------------------\n";
    std::cout << " OBSERVATION SUMMARY TABLE (Current Latency: " << linkDelay << "):\n";
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << " Link Latency | Flow 1 Throughput (Port 5000) | Flow 2 Throughput (Port 6000)\n";
    std::cout << "--------------+-------------------------------+-------------------------\n";
    std::cout << "   " << std::setw(6) << linkDelay << "     |          "
              << std::fixed << std::setprecision(4) << tp1 << " Mbps          |        "
              << std::fixed << std::setprecision(4) << tp2 << " Mbps\n";
    std::cout << "------------------------------------------------------------------------\n\n";

    std::cout << "WIRESHARK VERIFICATION:\n";
    std::cout << "  PCAP trace saved as: udp-port-delivery-" << linkDelay << "-1-0.pcap\n";
    std::cout << "  - Packets with dst port 5000 delivered to UdpServer 1.\n";
    std::cout << "  - Packets with dst port 6000 delivered to UdpServer 2.\n";
    std::cout << "  - Note: Since UDP does not require ACKs or window flow control,\n";
    std::cout << "    propagation latency does NOT throttle UDP throughput (unlike TCP),\n";
    std::cout << "    as long as the aggregate rate does not exceed link capacity (5 Mbps).\n";
    std::cout << "========================================================================\n";

    Simulator::Destroy();
    return 0;
}
