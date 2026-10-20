# How to Run Experiment 3 on Ubuntu OS
## Implementation of HTTP & HTTPS Client and Web Server using TCP Sockets

This experiment implements:
1. **Custom C HTTP Web Server** (`http_server.c`): Serves static HTML, text, and files over TCP port 8000.
2. **HTTP Client** (`http_client.c`): Connects to web server, transmits HTTP GET requests, strips headers, downloads payload, and calculates performance metrics (TTFB, download time, throughput).
3. **Secure HTTPS Client** (`https_client.c`): OpenSSL TLS socket client.

---

## 1. Prerequisites on Ubuntu
Ensure standard build tools and OpenSSL development headers are installed:
```bash
sudo apt update
sudo apt install build-essential libssl-dev -y
```

Navigate to the directory:
```bash
cd ~/Desktop/PCN/Excercise3
```

---

## 2. Running C HTTP Web Server & HTTP Client

### Step 1: Create a simple test webpage (if not already present)
```bash
echo "<html><body><h1>Hello from PCN Lab Server</h1></body></html>" > index.html
echo "Computer Networks Lab Sample Notes" > notes.txt
```

### Step 2: Compile the Server and Client
```bash
gcc http_server.c -o http_server
gcc http_client.c -o http_client
```

### Step 3: Run Server and Client in Separate Terminals

* **Terminal 1 (HTTP Web Server):**
  ```bash
  ./http_server 8000
  ```
  *Expected Output:*
  ```text
  ==================================================
      Custom C HTTP Web Server Running on Port 8000
      Base Directory : .
      Listening at   : http://localhost:8000/
  ==================================================
  ```

* **Terminal 2 (HTTP Client):**
  ```bash
  ./http_client
  ```
  *Interaction:* Enter URL when prompted:
  ```text
  Enter URL (e.g., http://localhost:8000/index.html): http://localhost:8000/index.html
  
  [HTTP Client] Connecting to localhost on Port 8000...
  [HTTP Client] Sending HTTP GET request:
  GET /index.html HTTP/1.1
  Host: localhost
  Connection: close
  
  --- Server Response ---
  HTTP/1.1 200 OK
  Content-Type: text/html
  Content-Length: 55
  
  Saved payload to: downloaded_index.html
  
  --- Performance Metrics ---
  TCP Connection Time (RTT) : 0.45 ms
  Time To First Byte (TTFB) : 0.82 ms
  Total Download Time       : 1.12 ms
  Effective Throughput      : 49.10 KB/s
  ```

* You can also open your browser on Ubuntu and visit `http://localhost:8000/index.html`.

---

## 3. Running the Secure HTTPS Client (`https_client.c`)

### Step 1: Compile with OpenSSL libraries
```bash
gcc https_client.c -lssl -lcrypto -o https_client
```

### Step 2: Run against a live HTTPS server
```bash
./https_client https://www.google.com/
```
*Expected Output:*
* Completes SSL/TLS Handshake
* Verifies Certificate & Cipher Suite
* Retrieves HTTPS encrypted response and displays TTFB and throughput metrics.
