# How to Run Experiment 4 on Ubuntu OS
## Inspecting HTTP Packets using Wireshark & C Packet Sniffer

This experiment covers:
1. **Wireshark Network Packet Sniffer**: Capturing and filtering live HTTP GET/response packets.
2. **C Packet Sniffer & Protocol Analyzer** (`http_packet_sniffer.c`): Raw socket packet dissection in C.
3. **C HTTP Traffic Generator** (`http_traffic_generator.c`): Generates standard GET and conditional GET (`If-Modified-Since`) traffic to test `200 OK` and `304 Not Modified`.
4. **ARP Simulation** (`arp_simulation.c`): Address Resolution Protocol simulation.

---

## 1. Prerequisites on Ubuntu
Install Wireshark and build tools:
```bash
sudo apt update
sudo apt install wireshark tshark build-essential -y

# Allow non-root users to capture packets
sudo dpkg-reconfigure wireshark-common
# (Select "Yes" when prompted)
sudo usermod -aG wireshark $USER
newgrp wireshark
```

Navigate to the directory:
```bash
cd ~/Desktop/PCN/Excercise4
```

---

## 2. Running Wireshark GUI for HTTP Capture

1. Launch Wireshark:
   ```bash
   wireshark &
   ```
2. Double-click on your active interface (e.g., `eth0`, `wlan0`, or `any`).
3. In the display filter bar at the top, type:
   ```text
   http
   ```
   and press Enter.
4. Generate HTTP traffic using our C traffic generator or browser:
   ```bash
   gcc http_traffic_generator.c -o http_traffic_generator
   ./http_traffic_generator
   ```
5. Inspect the captured packets in Wireshark:
   * **Packet 1:** HTTP `GET /wireshark-labs/HTTP-wireshark-file1.html HTTP/1.1`
   * **Packet 2:** Server response `HTTP/1.1 200 OK`
   * **Packet 3 (Conditional GET):** Includes `If-Modified-Since:` header
   * **Packet 4:** Server response `HTTP/1.1 304 Not Modified`

---

## 3. Running the C Packet Sniffer & Analyzer (`http_packet_sniffer.c`)

### Step 1: Compile the Sniffer
```bash
gcc http_packet_sniffer.c -o http_packet_sniffer
```

### Step 2: Run the Sniffer
* **With root permissions (Live Packet Capture):**
  ```bash
  sudo ./http_packet_sniffer
  ```
  Generates traffic in another terminal, and the sniffer decodes:
  * IP Header (Version, Source IP, Destination IP, TTL)
  * TCP Header (Source Port, Destination Port, Sequence/ACK, Flags)
  * HTTP Protocol line and Hex/ASCII payload dump.

* **Without root permissions:**
  ```bash
  ./http_packet_sniffer
  ```
  Automatically activates built-in HTTP packet inspection simulation mode.

---

## 4. Running ARP Simulation (`arp_simulation.c`)
```bash
gcc arp_simulation.c -o arp_simulation
./arp_simulation
```
*Expected Output:*
* Displays initial local ARP Cache Table (`IP <-> MAC`).
* Tests **ARP Cache Hit** for cached host.
* Tests **ARP Cache Miss**: Broadcasts `ARP Request ("Who has IP ...")` and receives unicast `ARP Reply`.
* Updates local ARP cache table with new dynamic entry.
