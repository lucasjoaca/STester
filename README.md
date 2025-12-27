# STester - Multi-Protocol Network Diagnostic Tool

STester is a Client-Server network diagnostic application developed in C. It is designed to probe, measure, and log the status of various network services including HTTP, FTP, Echo, and TCP Latency. The server architecture is multi-threaded, allowing it to handle control commands and execute test suites concurrently.

## 🚀 Key Features
- **Multi-Protocol Support**: Built-in testers for HTTP GET, FTP (Connect, MKD, Upload), Echo services, and TCP Latency.
- **Modular Architecture**: Protocol-specific logic is decoupled into standalone modules located in the `Testers/` directory.
- **Automated Logging**: All test results are timestamped and saved to a local `logs.txt` file, which can be retrieved remotely by the client.
- **Concurrent Execution**: Utilizes POSIX threads (pthreads) to manage client requests without blocking diagnostic operations.

## 🛠️ Installation and Compilation

The project includes a `Makefile` for streamlined compilation.

1. **Clone the repository**:
   ```bash
   git clone [https://github.com/yourusername/stester.git](https://github.com/yourusername/stester.git)
   cd stester

    Compile the project:
    Bash
make

Note: This command uses the Makefile to generate the server executable.

Compile the client:
Bash

    gcc client.c -o client

## 📖 Usage Guide
### 1. Start the Diagnostic Server

Run the server executable on the machine you wish to use as the testing hub:
Bash

./server

The server listens on the default port 5000.

### 2. Run the Client

Start the interactive client on your local machine:
Bash

./client

### 3. Configuring and Running Tests

Once the client is running, follow the on-screen menu:

    Add HTTP Test: Provide the Target IP and Port (e.g., 80 or 8080).

    Add FTP Test: Requires credentials in user:password:parameter format (e.g., admin:12345:test_folder).

    Add Latency Test: Measures TCP handshake timing to a specific port.

    Start Suite: Sends the command to the server to begin executing all queued tests.

    Get Logs: Displays the results of the executed tests (PASSED/FAILED).

## 🧪 Validation & Testing

The tool has been validated using Wireshark to analyze traffic over different network interfaces:

    Loopback (lo): Used for local service testing (local FTP/HTTP daemons).

    VPN (tun0): Used for remote testing against university servers to verify connectivity across secure tunnels.

    Protocol Verification: Validated TCP 3-way handshakes, HTTP status codes (200 OK), and FTP Passive Mode transitions.

## 📂 Project Structure

    server.c - Core logic, thread management, and command parsing.

    client.c - Interactive user interface.

    Testers/ - Specific implementations for each network protocol.

    Makefile - Build automation script.

    logs.txt - Persistent storage for diagnostic results.
