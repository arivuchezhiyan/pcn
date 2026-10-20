# How to Run Experiment 1 on Ubuntu OS
## Study of Networking Diagnostic Tools & Socket Programming System Calls

This experiment covers:
1. **Network Diagnostic Commands** (`ping`, `traceroute`, `netstat`, `ifconfig`/`ip`, `nslookup`, `arp`)
2. **TCP Client-Server Communication** (`server_tcp.c`, `client_tcp.c`)
3. **UDP Client-Server Communication** (`server_udp.c`, `client_udp.c`)

---

## 1. Prerequisites on Ubuntu
Ensure `gcc` and basic network tools are installed:
```bash
sudo apt update
sudo apt install build-essential net-tools iputils-ping traceroute dnsutils -y
```

---

## 2. Running Diagnostic Commands
Open a terminal and run the diagnostic tools:
```bash
# Test connectivity and measure RTT
ping -c 4 127.0.0.1
ping -c 4 google.com

# Trace packet route to a remote host
traceroute google.com

# View network interfaces and IP configuration
ifconfig
# or
ip addr show

# View active listening ports and connections
netstat -tuln

# Query DNS resolution for domain
nslookup google.com

# View local ARP cache table
arp -a
# or
ip neigh
```

---

## 3. Running TCP Socket Programs (`server_tcp.c` & `client_tcp.c`)

Navigate to the experiment folder:
```bash
cd ~/Desktop/PCN/Excercise1
```

### Step 1: Compile the C programs
```bash
gcc server_tcp.c -o server_tcp
gcc client_tcp.c -o client_tcp
```

### Step 2: Open TWO terminal windows
* **Terminal 1 (Server):**
  ```bash
  ./server_tcp
  ```
  *Expected Output:*
  ```text
  [TCP Server] Listening on Port 8080...
  Waiting for incoming client connections...
  ```

* **Terminal 2 (Client):**
  ```bash
  ./client_tcp
  ```
  *Interaction:* Enter string or message from keyboard:
  ```text
  Enter message to send to server: Hello TCP Server
  [Client] Sent: Hello TCP Server
  [Client] Received Echo/Response from Server: HELLO TCP SERVER
  ```

---

## 4. Running UDP Socket Programs (`server_udp.c` & `client_udp.c`)

### Step 1: Compile the C programs
```bash
gcc server_udp.c -o server_udp
gcc client_udp.c -o client_udp
```

### Step 2: Run Server and Client
* **Terminal 1 (Server):**
  ```bash
  ./server_udp
  ```
  *Expected Output:*
  ```text
  [UDP Server] Bound to Port 8081. Waiting for datagrams...
  ```

* **Terminal 2 (Client):**
  ```bash
  ./client_udp
  ```
  *Interaction:*
  ```text
  Enter datagram message: Hello UDP Server
  [Client] Sent 16 bytes to server.
  [Client] Received reply: Echo - Hello UDP Server
  ```
