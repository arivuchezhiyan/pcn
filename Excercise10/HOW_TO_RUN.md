# How to Run Experiment 10 on Ubuntu OS
## Comparative Evaluation of Routing Protocols (RIP, OSPF, AODV) Using NS-3

This experiment compares three major dynamic routing protocols:
* **RIP** (Distance-Vector / Bellman-Ford, metric: Hop count)
* **OSPF** (Link-State / Dijkstra's Shortest Path First, metric: Link Cost / Bandwidth)
* **AODV** (Ad-hoc On-Demand Distance Vector, reactive routing)

Across three conditions:
1. **Normal Condition** (1 Mbps baseline traffic)
2. **Congestion Condition** (High load 4 Mbps traffic)
3. **Link Failure & Dynamic Re-routing** (Primary link $R1-R2$ breaks at $t = 10.0s$, measuring failover time)

---

## 1. Prerequisites on Ubuntu & NS-3 Setup

Locate your NS-3 root folder:
```bash
find ~ -maxdepth 3 -name "ns3" -type f
```
Typical path: `~/development/ns-3-dev` or `~/ns-allinone-3.xx/ns-3.xx`.

---

## 2. Step-by-Step Instructions

### Step 1: Open Terminal and Navigate to the NS-3 Root Directory
```bash
cd ~/development/ns-3-dev
# or
cd ~/ns-allinone-3.*/ns-3.*
```

---

### Step 2: Copy the C++ File to `scratch/`
```bash
cp ~/Desktop/PCN/Excercise10/routing_protocols_sim.cc scratch/
```

Verify:
```bash
ls -l scratch/routing_protocols_sim.cc
```

---

### Step 3: Run Simulations for Each Routing Protocol

#### Phase 1: Normal Network Condition
```bash
# Test RIP:
./ns3 run "scratch/routing_protocols_sim --protocol=RIP --condition=normal"

# Test OSPF:
./ns3 run "scratch/routing_protocols_sim --protocol=OSPF --condition=normal"

# Test AODV:
./ns3 run "scratch/routing_protocols_sim --protocol=AODV --condition=normal"
```

---

#### Phase 2: Link Failure & Dynamic Failover Rerouting
In this test, the primary link between Router 1 and Router 2 is torn down at $t = 10.0\text{s}$, and the routing protocol must discover the backup path via Router 3 and Router 4:
```bash
# Test RIP Convergence on link failure:
./ns3 run "scratch/routing_protocols_sim --protocol=RIP --condition=link_failure"

# Test OSPF Dynamic Re-routing on link failure:
./ns3 run "scratch/routing_protocols_sim --protocol=OSPF --condition=link_failure"

# Test AODV Route Rediscovery:
./ns3 run "scratch/routing_protocols_sim --protocol=AODV --condition=link_failure"
```

---

#### Phase 3: High Traffic Congestion Condition
```bash
./ns3 run "scratch/routing_protocols_sim --protocol=OSPF --condition=congestion"
```

---

## 3. Expected Comparative Output Summary

| Scenario / Metric | RIP (Distance-Vector) | OSPF (Link-State) | AODV (Reactive) |
| :--- | :---: | :---: | :---: |
| **Normal PDR (%)** | 98.2 % | 99.8 % | 97.4 % |
| **Normal Delay (ms)** | 18.5 ms | 12.1 ms | 24.3 ms |
| **Link Failure Failover Time** | ~12 - 18 s (Slow) | ~1.2 - 2.5 s (Fast) | ~3.0 - 4.5 s |
| **Routing Overhead** | High (Periodic broadcasts) | Low (Triggered LSAs) | On-demand (RREQ/RREP) |

* **Analysis:** OSPF exhibits the fastest route recalculation upon link failure thanks to Dijkstra's link-state database and immediate LSA flooding, whereas RIP takes much longer due to periodic table exchanges and split-horizon timers.
