# How to Run Experiment 6 on Ubuntu OS
## Implementation of Error Detection using Checksum and CRC

This experiment implements Data Link Layer error detection mechanisms:
1. **Interactive TCP Error Detection Server** (`error_detection_server.c`): Receives codewords/checksums, performs Modulo-2 polynomial division for CRC ($G(x) = x^4 + x + 1$) and 1's complement summation for Checksum, verifies frame integrity, and returns `ACK` or `NACK`.
2. **Interactive TCP Error Detection Client** (`error_detection_client.c`): Accepts keyboard text strings, converts characters to binary bits, computes CRC and Checksum, offers interactive error-injection testing (injecting flipped bits), and sends data to the server.

---

## 1. Prerequisites on Ubuntu
Ensure `gcc` is installed:
```bash
sudo apt update
sudo apt install build-essential -y
```

Navigate to the directory:
```bash
cd ~/Desktop/PCN/Excercise6
```

---

## 2. Compilation
```bash
gcc error_detection_server.c -o error_detection_server
gcc error_detection_client.c -o error_detection_client
```

---

## 3. Running Server and Client

Open **TWO** terminal windows:

### Terminal 1: Start Server
```bash
./error_detection_server
```
*Expected Server Output:*
```text
============================================================
   Error Detection Server (Checksum & CRC) Listening on 8080
============================================================
Waiting for sender client connection...
```

### Terminal 2: Run Client
```bash
./error_detection_client
```

*Interactive Steps on Client:*
1. **Enter Text String:**
   ```text
   Enter text string to transmit: NETWORKS
   ```
2. **Select Error Detection Method:**
   * Enter `1` for **16-bit One's Complement Checksum**
   * Enter `2` for **Cyclic Redundancy Check (CRC-4)** with Generator `10011` ($x^4 + x + 1$)
3. **Test Case 1 — Error-Free Transmission:**
   * Choose to NOT inject any error (`n`).
   * *Result:* Server confirms remainder is `0000` / checksum is valid and returns `ACK: Frame Verified Error-Free!`.
4. **Test Case 2 — Error Injection Test (Corrupted Frame):**
   * Choose to inject a bit flip error (`y`).
   * *Result:* Server detects non-zero remainder / invalid checksum and returns `NACK: Bit Error Detected! Frame Discarded.`.
