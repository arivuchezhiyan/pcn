# How to Run Experiment 2 on Ubuntu OS
## Multi-Client Student Information Retrieval Application using TCP & UDP Sockets

This experiment implements:
1. **Concurrent Multithreaded TCP Server** using POSIX Threads (`pthread`) querying `students.csv` database.
2. **Interactive TCP Client** querying student details by Roll Number.
3. **UDP Server & Client** for connectionless student profile lookups.

---

## 1. Prerequisites on Ubuntu
Ensure `gcc` and `make` are installed:
```bash
sudo apt update
sudo apt install build-essential -y
```

Make sure you are in the `Excercise2` directory where `students.csv` is present:
```bash
cd ~/Desktop/PCN/Excercise2
ls -l students.csv
```

---

## 2. Running the Multithreaded TCP Application

### Step 1: Compile the Programs
> **Note:** The server uses POSIX threads, so the `-pthread` flag is mandatory.
```bash
gcc -pthread tcp_server_students.c -o tcp_server_students
gcc tcp_client_students.c -o tcp_client_students
```

### Step 2: Open TWO (or more) Terminals

* **Terminal 1 (TCP Server):**
  ```bash
  ./tcp_server_students
  ```
  *Expected Output:*
  ```text
  [TCP Server] Multi-Client Academic Server Initialized.
  [TCP Server] Database 'students.csv' loaded successfully.
  [TCP Server] Listening on Port 8080...
  ```

* **Terminal 2 (TCP Client 1):**
  ```bash
  ./tcp_client_students
  ```
  *Prompt & Query:*
  ```text
  Connected to Student Database Server.
  Enter Student Roll Number to Search (or 'exit'): 3122247001006
  
  --- Academic Profile Found ---
  Roll Number : 3122247001006
  Name        : Arivuchezhiyan E
  Department  : CSE
  Semester    : V
  CGPA        : 8.85
  ------------------------------
  ```

* **Terminal 3 (TCP Client 2 - Concurrent Test):**
  Open another terminal and connect simultaneously to verify multithreaded concurrent handling:
  ```bash
  ./tcp_client_students
  ```
  Enter Roll Number: `3122247001001` or any valid roll from `students.csv`.

---

## 3. Running the UDP Application

### Step 1: Compile the Programs
```bash
gcc udp_server_students.c -o udp_server_students
gcc udp_client_students.c -o udp_client_students
```

### Step 2: Run Server and Client

* **Terminal 1 (UDP Server):**
  ```bash
  ./udp_server_students
  ```
  *Expected Output:*
  ```text
  [UDP Server] Listening for queries on Port 8082...
  ```

* **Terminal 2 (UDP Client):**
  ```bash
  ./udp_client_students
  ```
  *Prompt:*
  ```text
  Enter Roll Number to query via UDP: 3122247001006
  [Client] Response received from server:
  Student Found: Arivuchezhiyan E | CSE | Sem 5 | CGPA 8.85
  ```
