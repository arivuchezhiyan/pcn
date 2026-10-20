# How to Run Experiment 11 on Ubuntu OS
## Performance Analysis of a Heterogeneous Wireless Network Using NS-3 & C

This experiment designs and evaluates a **Smart-Campus Network** consisting of three heterogeneous wireless technologies communicating with their access/coordinator nodes under mobility:
1. **Wi-Fi** (IEEE 802.11ac at 5 GHz)
2. **Bluetooth** (BLE at 2.4 GHz)
3. **Zigbee** (IEEE 802.15.4 LR-WPAN at 2.4 GHz)

Evaluates four key performance metrics at speeds of **1 m/s**, **5 m/s**, and **10 m/s**:
* **Signal Strength (Received Power)** in dBm
* **Throughput** (Mbps / kbps)
* **Packet Loss Percentage** (%)
* **End-to-End Delay** (ms)

---

## METHOD 1: Running the Standalone C Simulation (Fast & Lightweight)

If you want to run the experiment immediately on Ubuntu using standard `gcc` (without building NS-3):

### Step 1: Open Terminal & Navigate to Excercise11
```bash
cd ~/Desktop/PCN/Excercise11
```

### Step 2: Compile with GCC
```bash
gcc -O2 heterogeneous_wireless_sim.c -lm -o sim11
```

### Step 3: Execute the Simulation
```bash
./sim11
```

*Expected Output:*
Outputs full performance evaluation tables for Wi-Fi, Bluetooth, and Zigbee across 1 m/s, 5 m/s, and 10 m/s with detailed comparative analysis.

---

## METHOD 2: Running the Full Discrete-Event Simulation in NS-3

### Step 1: Locate your NS-3 Installation
Check where NS-3 is extracted on Ubuntu:
```bash
find ~ -maxdepth 3 -name "ns3" -type f
```
Typical directory: `~/development/ns-3-dev` or `~/ns-allinone-3.xx/ns-3.xx`.

---

### Step 2: Navigate to NS-3 Root Directory
```bash
cd ~/development/ns-3-dev
# or
cd ~/ns-allinone-3.*/ns-3.*
```

---

### Step 3: Copy the `.cc` Simulation Script to `scratch/`
Place `heterogeneous_wireless_sim.cc` in the NS-3 `scratch/` folder:
```bash
cp ~/Desktop/PCN/Excercise11/heterogeneous_wireless_sim.cc scratch/
```

Verify that it exists inside `scratch/`:
```bash
ls -l scratch/heterogeneous_wireless_sim.cc
```

---

### Step 4: Run the Simulation at Different Node Speeds

Run the simulation for each mobility condition:

#### 1. Low Mobility Speed (1 m/s - Walking Speed):
```bash
./ns3 run "scratch/heterogeneous_wireless_sim --speed=1.0"
```

#### 2. Medium Mobility Speed (5 m/s - Running / Jogging Speed):
```bash
./ns3 run "scratch/heterogeneous_wireless_sim --speed=5.0"
```

#### 3. High Mobility Speed (10 m/s - Cycling / Campus Shuttle):
```bash
./ns3 run "scratch/heterogeneous_wireless_sim --speed=10.0"
```

*(Note: If your NS-3 uses `waf`, run with `./waf --run "scratch/heterogeneous_wireless_sim --speed=1.0"`).*

---

### Step 5: Optional Parametric Runs
* **Random Waypoint Mobility Model:**
  ```bash
  ./ns3 run "scratch/heterogeneous_wireless_sim --speed=5.0 --mobility=RandomWaypoint"
  ```
* **Longer Simulation Time (e.g., 100 seconds):**
  ```bash
  ./ns3 run "scratch/heterogeneous_wireless_sim --speed=1.0 --simTime=100.0"
  ```

---

## Expected Output Summary

```text
====================================================================
 FLOW MONITOR PERFORMANCE RESULTS SUMMARY (Speed: 1.0 m/s)
====================================================================
Flow [Wi-Fi (802.11ac)] 10.1.1.2 -> 10.1.1.1
  Tx Packets        : 12200
  Rx Packets        : 11956
  Packet Loss       : 2.00 %
  Throughput        : 19.18 Mbps
  End-to-End Delay  : 5.34 ms
--------------------------------------------------------------------
Flow [Bluetooth (BLE)] 10.1.2.2 -> 10.1.2.1
  Tx Packets        : 4880
  Rx Packets        : 4613
  Packet Loss       : 5.47 %
  Throughput        : 231.6 kbps
  End-to-End Delay  : 10.23 ms
--------------------------------------------------------------------
Flow [Zigbee (802.15.4)] 10.1.3.2 -> 10.1.3.1
  Tx Packets        : 976
  Rx Packets        : 934
  Packet Loss       : 4.30 %
  Throughput        : 46.87 kbps
  End-to-End Delay  : 15.53 ms
====================================================================
```
