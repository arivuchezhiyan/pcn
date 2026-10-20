# How to Run Experiment 7 on Ubuntu OS
## NS-3 Network Simulations: Simple Network & Port-Based UDP Traffic Delivery

This experiment comprises two laboratory NS-3 simulation assignments:
1. **Part 1 (`simple_network_sim.cc`): Simulation of Simple Network using NS-3**
   * 4 nodes ($n0, n1, n2, n3$) connected via three point-to-point links (5 Mbps, 2 ms).
   * Two concurrent UDP traffic flows ($n0 \to n2$ at 1 Mbps on Port 5000; $n3 \to n0$ at 500 Kbps on Port 6000).
   * Calculates Total Bytes Received, Throughput, and **Average Packet Latency WITHOUT using FlowMonitor** (via direct packet tracing callbacks).
2. **Part 2 (`udp_port_delivery_sim.cc`): UDP Traffic & Port-Based Application Delivery**
   * Point-to-point link between `Node1` and `Node2`.
   * Two simultaneous UDP applications on the same IP addresses demultiplexed by Port 5000 and Port 6000.
   * Analyzes effect of varying link latency (**2 ms, 5 ms, 10 ms, 20 ms**) on end-to-end throughput.
   * Generates PCAP packet capture traces for Wireshark inspection.

---

## 1. Prerequisites on Ubuntu & NS-3 Setup

### Where is NS-3 Located?
Your NS-3 discrete-event simulator is located at:
* **`~/development/ns-3-dev`** (or `~/ns-allinone-3.*/ns-3.*`)

Verify by running:
```bash
find ~ -maxdepth 3 -name "ns3" -type f
```

---

## 2. Step-by-Step Instructions

### Step 1: Open Terminal & Navigate to NS-3 Root Folder
```bash
cd ~/development/ns-3-dev
```

---

### Step 2: Copy the C++ Simulation Files into `scratch/`
Custom NS-3 simulation scripts are executed from the **`scratch/`** directory:
```bash
cp ~/Desktop/PCN/Excercise7/simple_network_sim.cc scratch/
cp ~/Desktop/PCN/Excercise7/udp_port_delivery_sim.cc scratch/
```

Verify both files are placed in `scratch/`:
```bash
ls -l scratch/*sim.cc
```

---

### Step 3: Run Part 1 — Simple Network Simulation (`simple_network_sim`)

Run the Part 1 simulation:
```bash
./ns3 run scratch/simple_network_sim
```
*(Note: Do NOT add `.cc` in the run command).*

#### Expected Console Output (Without FlowMonitor):
```text
========================================================================
  NS-3 EXPERIMENT 7 (PART 1): SIMPLE NETWORK PERFORMANCE RESULTS
  Calculated Without FlowMonitor (Using Direct Packet Tracing)
========================================================================

--- Flow 1: Node n0 -> Node n2 (Configured Rate: 1 Mbps, Active: 2s - 10s) ---
  - Total Packets Transmitted : 1953 packets
  - Total Packets Received    : 1951 packets
  - Total Bytes Received      : 998912 bytes
  - Throughput                : 0.9989 Mbps (998.9120 Kbps)
  - Average Packet Latency    : 5.7344 ms

--- Flow 2: Node n3 -> Node n0 (Configured Rate: 500 Kbps, Active: 3s - 10s) ---
  - Total Packets Transmitted : 854 packets
  - Total Packets Received    : 853 packets
  - Total Bytes Received      : 436736 bytes
  - Throughput                : 0.4991 Mbps (499.1269 Kbps)
  - Average Packet Latency    : 5.7344 ms

========================================================================
 9. COMPARATIVE PERFORMANCE EVALUATION
------------------------------------------------------------------------
 Metric                     | Flow 1 (n0 -> n2)       | Flow 2 (n3 -> n0)
----------------------------+-------------------------+---------------------
 Hops Traversed             | 2 hops (n0-n1, n1-n2)   | 2 hops (n3-n1, n1-n0)
 Offered Load               | 1.0000 Mbps             | 0.5000 Mbps (500 Kbps)
 Total Bytes Received       |          998912 bytes |        436736 bytes
 Achieved Throughput        |          0.9989 Mbps |        0.4991 Mbps
 Average Latency            |          5.7344 ms    |        5.7344 ms
 Delivery Ratio             | 100.00 %                | 100.00 %
========================================================================
```

---

### Step 4: Run Part 2 — UDP Port Delivery & Latency Variation (`udp_port_delivery_sim`)

Run the simulation across the four required link propagation latencies:

#### 1. Latency = 2 ms:
```bash
./ns3 run "scratch/udp_port_delivery_sim --delay=2ms"
```

#### 2. Latency = 5 ms:
```bash
./ns3 run "scratch/udp_port_delivery_sim --delay=5ms"
```

#### 3. Latency = 10 ms:
```bash
./ns3 run "scratch/udp_port_delivery_sim --delay=10ms"
```

#### 4. Latency = 20 ms:
```bash
./ns3 run "scratch/udp_port_delivery_sim --delay=20ms"
```

---

### Step 5: Wireshark Packet Inspection
The simulation automatically produces PCAP files (e.g., `udp-port-delivery-2ms-1-0.pcap`).
Open the capture file in Wireshark on Ubuntu:
```bash
wireshark udp-port-delivery-2ms-1-0.pcap &
```

* **Verify Port Demultiplexing:**
  * Filter `udp.port == 5000` to inspect packets delivered to Server 1.
  * Filter `udp.port == 6000` to inspect packets delivered to Server 2.

---

### Step 6: Observations Table Summary

| Link Latency | Flow 1 Throughput (Port 5000) | Flow 2 Throughput (Port 6000) | Observation |
| :---: | :---: | :---: | :--- |
| **2 ms** | 0.9999 Mbps | 0.9999 Mbps | Full 100% throughput achieved |
| **5 ms** | 0.9999 Mbps | 0.9999 Mbps | Full 100% throughput achieved |
| **10 ms** | 0.9999 Mbps | 0.9999 Mbps | Full 100% throughput achieved |
| **20 ms** | 0.9999 Mbps | 0.9999 Mbps | Full 100% throughput achieved |

* **Analysis:** Unlike TCP (which relies on ACKs and congestion window growth restricted by Round-Trip Time), UDP is connectionless. Therefore, varying propagation latency does not degrade UDP throughput as long as the aggregate traffic does not exceed the link's physical bandwidth capacity (5 Mbps).
