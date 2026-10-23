# Small Command Based SNMP Monitoring Tool
A dynamic C++ sensor that monitors device health via SNMP and ICMP (Ping). The tool watches a local configuration file for commands, allowing for automated network monitoring without restarting the application.

## Features
- Host Registry: Add and manage hosts with SNMP-specific credentials (Version and Community).
- Automated Ping Analytics: Ping hosts by name and extract latency/packet loss data.
- Interface Discovery: Scan all physical and virtual interfaces on a device via SNMP.
- Health Tracking: Monitor device system uptime to detect unexpected reboots.
- Port Control: Remotely toggle administrative status (Up/Down) of network ports.
- Structured Logging: Generate JSON-formatted logs with configurable severity levels.


## Project Structure

```
Sensor/
└── Sensor
    ├── Header Files/
    │   └── Sensor.h            # Main class declaration and data structures
    ├── Source Files/
    │   ├── main.cpp            # Application entry point
    │   ├── Sensor.cpp          # Core lifecycle (run loop)
    │   ├── Sensor.Commands.cpp # Command routing and parsing logic
    │   ├── Sensor.Hosts.cpp    # Host management and CRUD operations
    │   ├── Sensor.SNMP.cpp     # SNMP-specific logic and system calls
    │   └── Sensor.Helpers.cpp  # Logging, UI, and utility functions
    ├── commands.txt            # Input file for runtime commands
    ├── config.txt              # configuration file
    └── sensorLog.log           # Output file for JSON logs
```

## Configuration
config.txt

Create this file in the root directory to set the verbosity of the log file:

    1: Critical (Severe failures only)
    2: Error (Critical + Errors)
    3: Info (Standard operation logs/results)
    4: Debug (Everything)

## Available Commands

| Command | Arguments | Description |
| :--- | :--- | :--- |
| `add_host` | `<name> <ip> <ver> <comm>` | Adds a new device to the session registry with SNMP credentials. |
| `ping` | `<hostname>` | Pings a registered host and logs latency/packet loss analytics. |
| `hosts_dump` | None | Lists all hosts currently stored in the session. |
| `snmp_scan_interface` | `<hostname>` | Scans and lists all interfaces (physical, VLAN, tunnel) on the device. |
| `snmp_get_health` | `<hostname>` | Fetches the system uptime of the device to detect reboots. |
| `snmp_get_portStatus` | `<hostname>` | Displays the operational (active/inactive) status of all ports. |
| `snmp_set_portStatus` | `<host> <idx> <status>` | Sets the administrative status of a specific port to `up` or `down`. |

## Requirements

    Compiler: Visual Studio 2019+ (C++17 standard required)
    Environment: Windows with WSL (for snmp tools)
