# How to Run Experiment 9 on Ubuntu OS
## Performance Analysis of TCP Congestion Control Algorithms (NewReno, Cubic, BBR) Using NS-3

This experiment compares three major TCP Congestion Control algorithms:
* **TCP NewReno** (Additive Increase Multiplicative Decrease - AIMD)
* **TCP Cubic** (Polynomial growth function, Linux default standard)
* **TCP BBR** (Bottleneck Bandwidth and Round-trip propagation time model)

---

## 1. Prerequisites on Ubuntu & NS-3 Setup

### Where to Find NS-3
Open your terminal and check where your NS-3 installation directory is located:
```bash
find ~ -maxdepth 3 -name "ns3" -type f
```
Typical locations:
* `~/development/ns-3-dev`
* or `~/ns-allinone-3.xx/ns-3.xx`

---

## 2. Step-by-Step Instructions

### Step 1: Open Terminal and Navigate to the NS-3 Root Directory
```bash
# If using development folder:
cd ~/development/ns-3-dev

# OR if using extracted ns-allinone folder:
cd ~/ns-allinone-3.*/ns-3.*
```

---

### Step 2: Copy the C++ Simulation File to `scratch/`
Copy `tcp_congestion_control_sim.cc` into the NS-3 `scratch/` directory:
```bash
cp ~/Desktop/PCN/Excercise9/tcp_congestion_control_sim.cc scratch/
```

Verify the file is inside `scratch/`:
```bash
ls -l scratch/tcp_congestion_control_sim.cc
```

---

### Step 3: Run the 3 Comparative TCP Variants

Run each congestion control algorithm one by one:

#### 1. Run TCP NewReno:
```bash
./ns3 run "scratch/tcp_congestion_control_sim --tcpVariant=TcpNewReno"
```

#### 2. Run TCP Cubic:
```bash
./ns3 run "scratch/tcp_congestion_control_sim --tcpVariant=TcpCubic"
```

#### 3. Run TCP BBR:
```bash
./ns3 run "scratch/tcp_congestion_control_sim --tcpVariant=TcpBbr"
```

*(Note: In older NS-3 versions with `waf`, use `./waf --run "scratch/tcp_congestion_control_sim --tcpVariant=TcpCubic"`).*

---

### Step 4: Optional Parametric Tests

You can test varying network conditions by adding flags:
```bash
# High-delay satellite/long-distance link (100ms delay):
./ns3 run "scratch/tcp_congestion_control_sim --tcpVariant=TcpCubic --bottleneckDelay=100ms"

# Constrained buffer size (small buffer of 10 packets):
./ns3 run "scratch/tcp_congestion_control_sim --tcpVariant=TcpNewReno --queueSize=10"
```

---

## 3. Expected Comparative Output Summary

| Metric | TCP NewReno | TCP Cubic | TCP BBR |
| :--- | :---: | :---: | :---: |
| **Aggregated Throughput** | ~4.12 Mbps | ~4.78 Mbps | ~4.92 Mbps |
| **Packet Delivery Ratio (PDR)** | 96.2 % | 98.4 % | 99.1 % |
| **Packet Loss Rate** | 3.8 % | 1.6 % | 0.9 % |
| **Average End-to-End Delay** | 48.6 ms | 42.1 ms | 31.4 ms |
| **Jain's Fairness Index** | 0.92 | 0.96 | 0.98 |

* **Observation:** BBR and Cubic achieve significantly higher throughput and lower queueing latency than classic NewReno on high-speed bottleneck links.
