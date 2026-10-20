# How to Run Experiment 8 on Ubuntu OS
## Simulation and Performance Analysis of TCP and UDP Traffic Using NS-3

This experiment evaluates the interaction and performance trade-offs between **TCP (Reliable, Congestion-Controlled)** and **UDP (Unreliable, Constant Bit Rate)** traffic competing over a bottleneck link in a dumbbell network topology.

---

## 1. Prerequisites on Ubuntu & NS-3 Location

### What is NS-3?
NS-3 (Network Simulator 3) is a discrete-event network simulator written in C++ and Python.

### Where is NS-3 located on your system?
When you extract NS-3 on Ubuntu, it is typically located in your home directory or development folder, for example:
* `~/development/ns-3-dev`
* or `~/ns-allinone-3.xx/ns-3.xx` (e.g., `~/ns-allinone-3.41/ns-3.41`)

Find your NS-3 installation by running in terminal:
```bash
find ~ -maxdepth 3 -name "ns3" -type f
```

---

## 2. Step-by-Step Instructions to Run Experiment 8

### Step 1: Open Terminal and Navigate to your NS-3 Root Directory
```bash
# Example if located in development:
cd ~/development/ns-3-dev

# OR if using extracted ns-allinone:
cd ~/ns-allinone-3.*/ns-3.*
```
Verify you see the `./ns3` executable (or `./waf` in older versions):
```bash
ls -l ns3
```

---

### Step 2: Copy the Simulation File into the `scratch/` Folder
In NS-3, custom user scripts are placed inside the **`scratch/`** directory. When placed in `scratch/`, NS-3 automatically compiles and links them without needing to modify build files.

Run the copy command:
```bash
# Copy from your PCN folder to ns-3 scratch/
cp ~/Desktop/PCN/Excercise8/tcp_udp_traffic_sim.cc scratch/
```

Verify it is placed:
```bash
ls -l scratch/tcp_udp_traffic_sim.cc
```

---

### Step 3: Run the Simulation

Run the experiment with default baseline parameters:
```bash
./ns3 run scratch/tcp_udp_traffic_sim
```
*(Note: Do not include the `.cc` extension in the run command).*

If using older NS-3 versions with `waf`:
```bash
./waf --run scratch/tcp_udp_traffic_sim
```

---

### Step 4: Running with Parametric Variations (Different Test Cases)

You can pass command-line arguments to test different network conditions:

1. **Test Varying Bottleneck Bandwidth (e.g., 5 Mbps):**
   ```bash
   ./ns3 run "scratch/tcp_udp_traffic_sim --bottleneckBw=5Mbps"
   ```

2. **Test Varying Propagation Delay (e.g., 50 ms high latency):**
   ```bash
   ./ns3 run "scratch/tcp_udp_traffic_sim --bottleneckDelay=50ms"
   ```

3. **Test Varying Queue Size (Bufferbloat vs DropTail buffer size):**
   ```bash
   ./ns3 run "scratch/tcp_udp_traffic_sim --queueSize=10"
   ```

4. **Test Higher Aggressive UDP Traffic (e.g., 5 Mbps UDP stream):**
   ```bash
   ./ns3 run "scratch/tcp_udp_traffic_sim --udpRate=5Mbps"
   ```

5. **Generate PCAP Traces for Wireshark Analysis:**
   ```bash
   ./ns3 run "scratch/tcp_udp_traffic_sim --enablePcap=true"
   # View captured packets with Wireshark:
   wireshark tcp_udp_sim-r1-r2.pcap &
   ```

---

## 3. Expected Output Summary
```text
================================================================
  NS-3 TCP & UDP TRAFFIC SIMULATION (EXPERIMENT 8)             
================================================================
Flow [TCP Flow (n0 -> n3)]:
  - Tx Packets        : 3125
  - Rx Packets        : 3048
  - Packet Delivery   : 97.54 %
  - Packet Loss Rate  : 2.46 %
  - Throughput        : 1.25 Mbps
  - Mean Delay        : 28.45 ms

Flow [UDP Flow (n1 -> n2)]:
  - Tx Packets        : 4882
  - Rx Packets        : 4510
  - Packet Delivery   : 92.38 %
  - Packet Loss Rate  : 7.62 %
  - Throughput        : 0.92 Mbps
  - Mean Delay        : 22.10 ms
================================================================
```
