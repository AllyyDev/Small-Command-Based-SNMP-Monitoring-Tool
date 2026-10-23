#include "Sensor.h"
#include <iostream>
#include <cctype>
using namespace std;

//////////////////////////////////////////
//------------ Hosts Setup -------------//
//////////////////////////////////////////
void Sensor::handleAddHost(const string& name, const string& ip, const string& version, const string& community, const string& commandLine) {
    if (!name.empty() && !ip.empty() && !version.empty() && !community.empty()) {
        hosts[name] = { ip, version, community };
        string msg = "Added host " + name + " (IP: " + ip + ", SNMP: " + version + ", Community: " + community + ")";
        logEvent(LogLevel::INFO, commandLine, msg);
        logConsole(LogLevel::INFO, commandLine, msg);
        handleSnmpHealth(name, "AUTO-CHECK");
    }
    else {
        logEvent(LogLevel::ERROR, commandLine, "Usage: add_host <name> <ip> <version> <community>");
        logConsole(LogLevel::ERROR, commandLine, "Usage: add_host <name> <ip> <version> <community>");
    }
}

void Sensor::handlePing(const string& hostname, const string& commandLine) {
    if (hosts.find(hostname) != hosts.end()) {
        string ip = hosts[hostname].ip;
        logConsole(LogLevel::INFO, commandLine, "Pinging host " + hostname + " (" + ip + ")");

        string command = "ping -c 4 " + ip;
        string output = runSystemCommand(command);

        string status = (output.find("Request timed out") != string::npos) ? "TIMEOUT" : "OK";
        string latency = "N/A";
        string loss = "N/A";

        // Extract Latency
        size_t timePos = output.find("time=");
        if (timePos != string::npos) {
            size_t endPos = output.find("ms", timePos);
            if (endPos != string::npos) latency = output.substr(timePos, endPos - timePos + 3);
        }

        // Extract Loss
        size_t percentPos = output.find("%");
        if (percentPos != string::npos) {
            size_t startNum = percentPos;
            while (startNum > 0 && isdigit(static_cast<unsigned char>(output[startNum - 1]))) startNum--;
            loss = output.substr(startNum, percentPos - startNum + 1);
        }

        string finalMessage = "Status: " + status + " | Latency: " + latency + " | Loss: " + loss;
        LogLevel resLevel = (status == "OK") ? LogLevel::INFO : LogLevel::ERROR;
        logEvent(resLevel, commandLine, finalMessage);
        logConsole(resLevel, commandLine, finalMessage);
    }
    else {
        logEvent(LogLevel::ERROR, commandLine, "Host '" + hostname + "' not found.");
        logConsole(LogLevel::ERROR, commandLine, "Host '" + hostname + "' not found.");
    }
}

void Sensor::handleHostsDump(const string& commandLine) {
    string msg = "Current Hosts:\n";
    if (hosts.empty()) {
        msg += "No hosts added yet.";
    }
    else {
        for (auto const& [name, hostInfo] : hosts) {
            msg += "\n - " + name + ": " + hostInfo.ip;
        }
    }
    logEvent(LogLevel::INFO, commandLine, "Dump performed");
    logConsole(LogLevel::INFO, commandLine, msg);
}