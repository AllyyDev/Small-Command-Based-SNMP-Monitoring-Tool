#define _CRT_SECURE_NO_WARNINGS
#include "Sensor.h"
#include <iostream>
#include <sstream>
#include <cctype>

using namespace std;

void Sensor::handleSnmpScan(const string& hostname, const string& commandLine) {
    if (hosts.find(hostname) == hosts.end()) {
        logConsole(LogLevel::ERROR, commandLine, "Host not found.");
        return;
    }
    HostInfo hostInfo = hosts[hostname];
    string command = "snmpwalk -v" + hostInfo.snmpVersion + " -c " + hostInfo.community + " " + hostInfo.ip + " .1.3.6.1.2.1.2.2.1.2";
    string output = runSystemCommand(command);
    logEvent(LogLevel::INFO, commandLine, "Interfaces found for " + hostname + ":\n" + output);
    logConsole(LogLevel::INFO, commandLine, "Interfaces scanned for " + hostname);
}

void Sensor::handleSnmpHealth(const string& hostname, const string& commandLine) {
    if (hosts.find(hostname) == hosts.end()) {
        logConsole(LogLevel::ERROR, commandLine, "Host not found.");
        return;
    }
    HostInfo hostInfo = hosts[hostname];
    string command = "snmpget -v" + hostInfo.snmpVersion + " -c " + hostInfo.community + " " + hostInfo.ip + " .1.3.6.1.2.1.1.3.0";
    string output = runSystemCommand(command);
    logEvent(LogLevel::INFO, commandLine, "Uptime for " + hostname + ": " + output);
    logConsole(LogLevel::INFO, commandLine, "Health Check (" + hostname + "): " + output);
}

void Sensor::handleSnmpPortStatus(const std::string& hostname, const std::string& commandLine) {
    if (hosts.find(hostname) == hosts.end()) {
        logConsole(LogLevel::ERROR, commandLine, "Host not found.");
        return;
    }
    HostInfo hostInfo = hosts[hostname];
    string command = "snmpwalk -v" + hostInfo.snmpVersion + " -c " + hostInfo.community + " " + hostInfo.ip + " .1.3.6.1.2.1.2.2.1.8";
    string output = runSystemCommand(command);

    stringstream ss(output);
    string line;
    string result = "Port Status for " + hostname + ":\n";

    while (getline(ss, line)) {
        size_t eqPos = line.find('=');
        if (eqPos != string::npos) {
            // Isolate the part after the '=' (e.g., " INTEGER: 2")
            string valuePart = line.substr(eqPos + 1);

            // Only check for "1" in the value portion, not the OID prefix
            string status = (valuePart.find("1") != string::npos) ? "ACTIVE" : "INACTIVE";
            result += line + " -> " + status + "\n";
        }
    }
    logEvent(LogLevel::INFO, commandLine, result);
    logConsole(LogLevel::INFO, commandLine, result);
}

void Sensor::handleSnmpSetPort(const string& hostname, const string& index, const string& status, const string& commandLine) {
    if (hosts.find(hostname) == hosts.end()) {
        logConsole(LogLevel::ERROR, commandLine, "Host not found.");
        return;
    }
    HostInfo hostInfo = hosts[hostname];
    string statusValue = (status == "up") ? "1" : "2";
    string snmpCommand = "snmpset -v" + hostInfo.snmpVersion + " -c " + hostInfo.community + " " + hostInfo.ip + " .1.3.6.1.2.1.2.2.1.7." + index + " integer " + statusValue;
    string output = runSystemCommand(snmpCommand);
    logEvent(LogLevel::CRITICAL, commandLine, "Port " + index + " set to " + status + ": " + output);
    logConsole(LogLevel::CRITICAL, commandLine, "Port " + index + " set to " + status + ": " + output);
}