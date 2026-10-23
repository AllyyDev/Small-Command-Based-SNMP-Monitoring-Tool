#define _CRT_SECURE_NO_WARNINGS
#include "Sensor.h"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <filesystem>
#include <array>
#include <memory>
using namespace std;
namespace fs = filesystem;

//////////////////////////////////////////
//---------- Commands Setup ------------//
//////////////////////////////////////////
void Sensor::checkCommands() {
    if (!fs::exists(commandFile)) return;

    ifstream file(commandFile);
    string line;
    bool foundContent = false;

    while (getline(file, line)) {
        string cleanLine = trim(line);
        if (!cleanLine.empty()) {
            executeCommand(cleanLine);
            foundContent = true;
        }
    }
    file.close();

    // If we read something, clear the file now
    if (foundContent) {
        ofstream clearFile(commandFile, ios::trunc);
    }
}

string Sensor::runSystemCommand(const std::string& cmd) {
    array<char, 128> buffer{};
    string result;

    // Prepending "wsl " to the command so Windows executes it inside Linux
    string wslCommand = "wsl " + cmd;

    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(wslCommand.c_str(), "r"), _pclose);

    if (!pipe) {
        return "Error: Could not execute command via WSL.";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void Sensor::executeCommand(const string& line) {
    string command = trim(line);
    logConsole(LogLevel::DEBUG, "system", "New command detected: " + command);
    logEvent(LogLevel::DEBUG, "system", "Processing command: " + command);

    // add_host <name> <ip> <version> <community>
    if (command.find("add_host ") == 0) {
        stringstream addHostsStream(command);
        string cmd, name, ip, version, community;
        addHostsStream >> cmd >> name >> ip >> version >> community;
        handleAddHost(name, ip, version, community, command);
    }
    // ping <hostname>
    else if (command.find("ping ") == 0) {
        stringstream pingsStream(command);
        string cmd, hostname;
        pingsStream >> cmd >> hostname;
        handlePing(hostname, command);
    }
    // hosts_dump
    else if (command == "hosts_dump") {
        handleHostsDump(command);
    }
    // snmp_scan_interface <hostname>
    else if (command.find("snmp_scan_interface ") == 0) {
        stringstream ss(command);
        string cmd, hostname;
        ss >> cmd >> hostname;
        handleSnmpScan(hostname, command);
    }
    // snmp_get_health <hostname>
    else if (command.find("snmp_get_health ") == 0) {
        stringstream ss(command);
        string cmd, hostname;
        ss >> cmd >> hostname;
        handleSnmpHealth(hostname, command);
    }
    // snmp_get_portStatus <hostname>
    else if (command.find("snmp_get_portStatus ") == 0) {
        stringstream ss(command);
        string cmd, hostname;
        ss >> cmd >> hostname;
        handleSnmpPortStatus(hostname, command);
    }
    // snmp_set_portStatus <hostname> <index> <status>
    else if (command.find("snmp_set_portStatus ") == 0) {
        stringstream ss(command);
        string cmd, hostname, index, status;
        ss >> cmd >> hostname >> index >> status;
        handleSnmpSetPort(hostname, index, status, command);
    }
    // Unknown command
    else {
        logEvent(LogLevel::ERROR, command, "Unknown command");
        logConsole(LogLevel::ERROR, command, "Unknown command");
    }
}