#include "Sensor.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

void Sensor::showMenu() const {
    cout << "\n=================================================" << endl;
    cout << "           SENSOR PROJECT COMMAND GUIDE            " << endl;
    cout << "=================================================" << endl;
    cout << "Available Commands:" << endl;

    // Host Management
    cout << "  1. add_host <name> <ip> <ver> <comm> - Add a host with SNMP details" << endl;
    cout << "  2. ping <hostname>                   - Ping a registered host" << endl;
    cout << "  3. hosts_dump                        - Display all added hosts" << endl;

    // SNMP Operations
    cout << "-------------------------------------------------" << endl;
    cout << "  4. snmp_scan_interface <host>      - List interface names" << endl;
    cout << "  5. snmp_get_health <host>          - Fetch device uptime" << endl;
    cout << "  6. snmp_get_portStatus <host>      - Show active/inactive ports" << endl;
    cout << "  7. snmp_set_portStatus <h> <idx> <s> - Enable/Disable port (up/down)" << endl;

    cout << "=================================================" << endl;
    cout << "Instructions: Write commands in '" << commandFile << "'" << endl;
    cout << "=================================================\n" << endl;
}

void Sensor::loadConfig() {
    if (!fs::exists(configFile)) {
        cout << "Config file not found. Using default level (INFO)." << endl;
        return;
    }

    ifstream file(configFile);
    string currentConfigLine;

    while (getline(file, currentConfigLine)) {
        // 1. Trim whitespace from the currentConfigLine
        currentConfigLine = trim(currentConfigLine);

        // 2. Ignore empty lines or comments
        if (currentConfigLine.empty() || currentConfigLine[0] == '#') {
            continue;
        }

        // 3. Split currentConfigLine by '='
        size_t sep = currentConfigLine.find('=');
        if (sep != string::npos) {
            string key = trim(currentConfigLine.substr(0, sep));
            string value = trim(currentConfigLine.substr(sep + 1));

            // 4. Handle the specific keys
            if (key == "LogLevel") {
                try {
                    int level = stoi(value);
                    if (level == 1) currentLogLevel = LogLevel::CRITICAL;
                    else if (level == 2) currentLogLevel = LogLevel::ERROR;
                    else if (level == 3) currentLogLevel = LogLevel::INFO;
                    else if (level == 4) currentLogLevel = LogLevel::DEBUG;

                    cout << "Config loaded: LogLevel set to " << level << endl;
                }
                catch (const std::exception&) {
                    cout << "Invalid level value in config: " << value << endl;
                }
            }
        }
    }
}

string Sensor::levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::CRITICAL: return "CRITICAL";
    case LogLevel::ERROR:    return "ERROR";
    case LogLevel::INFO:     return "INFO";
    case LogLevel::DEBUG:    return "DEBUG";
    default:                 return "UNKNOWN";
    }
}

void Sensor::logEvent(LogLevel level, const string& command, const string& message) {
    // Only log if the message level is equal to or higher priority than current config
    if (static_cast<int>(level) <= static_cast<int>(currentLogLevel)) {
        ofstream out(logFile, ios_base::app);
        if (out.is_open()) {
            out << "{"
                << "\"timestamp\":\"" << getTimestamp() << "\", "
                << "\"level\":\"" << levelToString(level) << "\", "
                << "\"command\":\"" << command << "\", "
                << "\"message\":\"" << message << "\""
                << "}" << endl;
        }
    }
}

void Sensor::logConsole(LogLevel level, const string& command, const string& message) {
    cout << "[" << levelToString(level) << "] "
        << "(" << command << ") "
        << message << endl;
}

string Sensor::getTimestamp() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    struct tm buf;
    // Use the safe version provided by Visual Studio
    if (localtime_s(&buf, &in_time_t) != 0) {
        return "error-time";
    }

    stringstream stringStream;
    stringStream << put_time(&buf, "%Y-%m-%dT%H:%M:%S");
    return stringStream.str();
}

string Sensor::trim(const string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}