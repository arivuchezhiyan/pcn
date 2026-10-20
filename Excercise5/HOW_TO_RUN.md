# How to Run Experiment 5 on Ubuntu OS
## Simulation of Domain Name Resolution (DNS) Using UDP Socket Programming

This experiment simulates the **Domain Name System (DNS)** using connectionless UDP sockets:
1. **Iterative DNS Server** (`dns_server.c`): Maintains domain-to-IPv4 database and responds to queries on UDP port 5353.
2. **Interactive DNS Client** (`dns_client.c`): Queries domain names from keyboard, checks local cache, sends UDP query to server, and displays IP result or `NXDOMAIN (Domain Not Found)` error.

---

## 1. Prerequisites on Ubuntu
Ensure `gcc` is installed:
```bash
sudo apt update
sudo apt install build-essential -y
```

Navigate to the directory:
```bash
cd ~/Desktop/PCN/Excercise5
```

---

## 2. Compilation
```bash
gcc dns_server.c -o dns_server
gcc dns_client.c -o dns_client
```

---

## 3. Running Server and Client

Open **TWO** terminal windows:

### Terminal 1: Start DNS Server
```bash
./dns_server
```
*Expected Server Output:*
```text
===================================================
     UDP DNS Simulation Server Active on Port 5353
===================================================
Database entries loaded:
  - google.com          -> 142.250.190.46
  - ssn.edu.in          -> 103.110.245.10
  - github.com          -> 140.82.121.3
  - yahoo.com           -> 98.137.11.163
Waiting for incoming DNS lookup requests...
```

### Terminal 2: Run DNS Client
```bash
./dns_client
```

*Interactive Queries to Try:*
1. **Lookup 1 (Valid domain):**
   ```text
   Enter Domain Name to Resolve (or 'exit'): google.com
   [Client] Sending UDP DNS query for 'google.com'...
   [Client] Response received from DNS Server:
            Domain : google.com
            IPv4   : 142.250.190.46
            Status : 200 SUCCESS
   ```

2. **Lookup 2 (Local Cache Hit Test):**
   ```text
   Enter Domain Name to Resolve (or 'exit'): google.com
   [Client] Local DNS Cache HIT! Resolved without network query:
            Domain : google.com -> 142.250.190.46
   ```

3. **Lookup 3 (Non-existent Domain - Error Test):**
   ```text
   Enter Domain Name to Resolve (or 'exit'): nonexistentwebsite123.org
   [Client] Sending UDP DNS query for 'nonexistentwebsite123.org'...
   [Client] Response received from DNS Server:
            Status : NXDOMAIN (Domain Name Not Found in DNS Registry)
   ```
