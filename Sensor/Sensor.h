#pragma once
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <map>
using namespace std;

// Define Log Levels
enum class LogLevel {
    CRITICAL = 1,
    ERROR = 2,
    INFO = 3,
    DEBUG = 4
};

// Host Struct
struct HostInfo {
    string ip;
    string snmpVersion;
    string community;
};

class Sensor {
public:
    Sensor(const string& commandFilePath, const string& logFilePath);
    void run();

private:
    string commandFile;
    string logFile;
    string configFile;
    LogLevel currentLogLevel;
    map<string, HostInfo> hosts;

    // Core Logic
    void checkCommands();
    void executeCommand(const string& commandLine);
    string runSystemCommand(const string& cmd);

    // Host management
    void handleAddHost(const string& name, const string& ip, const string& version, const string& community, const string& commandLine);
    void handlePing(const string& hostname, const string& commandLine);
    void handleHostsDump(const string& commandLine);

    // SNMP specific methods
    void handleSnmpScan(const string& hostname, const string& commandLine);
    void handleSnmpHealth(const string& hostname, const string& commandLine);
    void handleSnmpPortStatus(const string& hostname, const string& commandLine);
    void handleSnmpSetPort(const string& hostname, const string& index, const string& status, const string& commandLine);


    // Helpers and Logging
    void logEvent(LogLevel level, const string& command, const string& message);
    void logConsole(LogLevel level, const string& command, const string& message);
    void loadConfig();
    void showMenu() const;
    string getTimestamp();
    string levelToString(LogLevel level);
    string trim(const string& s);
};
