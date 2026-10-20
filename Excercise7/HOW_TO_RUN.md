# How to Run Experiment 7 on Ubuntu OS
## Flow Control (Sliding Window Protocols) & Distance Vector Routing

This folder contains complete C implementations for Data Link Layer Flow Control and Network Layer Routing:
1. **Stop-and-Wait ARQ Protocol** (`sliding_window_stop_and_wait.c`)
2. **Go-Back-N (GBN) Sliding Window Protocol** (`sliding_window_go_back_n.c`)
3. **Selective Repeat ARQ Protocol** (`sliding_window_selective_repeat.c`)
4. **Distance Vector Routing Algorithm (Bellman-Ford)** (`distance_vector_routing.c`)
5. **Address Resolution Protocol (ARP)** (`arp_simulation.c`)

---

## 1. Prerequisites on Ubuntu
```bash
sudo apt update
sudo apt install build-essential -y
```

Navigate to the directory:
```bash
cd ~/Desktop/PCN/Excercise7
```

---

## 2. Compilation and Execution

### A. Stop-and-Wait ARQ Protocol
Models sending single frames, timer countdowns, simulated frame/ACK drops, and retransmissions:
```bash
gcc sliding_window_stop_and_wait.c -o stop_and_wait
./stop_and_wait
```
*Key Output:* Displays transmission log, lost frame detection, timeout retransmission, and protocol efficiency percentage.

---

### B. Go-Back-N (GBN) Sliding Window Protocol
Models window size $N = 4$, cumulative ACKs, window sliding, and retransmission of all frames from window base on timeout:
```bash
gcc sliding_window_go_back_n.c -o go_back_n
./go_back_n
```
*Key Output:* Traces sender window $[base, next\_seq\_num]$, simulated network packet drop, Go-Back-N window rollbacks, and transmission efficiency.

---

### C. Selective Repeat ARQ Protocol
Models receiver buffering of out-of-order frames and selective retransmission of lost packets only:
```bash
gcc sliding_window_selective_repeat.c -o selective_repeat
./selective_repeat
```
*Key Output:* Shows individual ACKs, receiver buffer storage, and selective retransmission of only corrupted frames.

---

### D. Distance Vector Routing Algorithm (Bellman-Ford)
Models a multi-node mesh network, routing table exchanges between neighbors, and convergence to shortest paths:
```bash
gcc distance_vector_routing.c -o distance_vector_routing
./distance_vector_routing
```
*Key Output:* Prints convergence rounds 1, 2, 3... and final routing tables with `[Destination | Distance | Next Hop]`.

---

### E. Address Resolution Protocol (ARP) Simulation
```bash
gcc arp_simulation.c -o arp_simulation
./arp_simulation
```
*Key Output:* Demonstrates ARP cache hits, broadcast requests, unicast replies, and cache table updates.
