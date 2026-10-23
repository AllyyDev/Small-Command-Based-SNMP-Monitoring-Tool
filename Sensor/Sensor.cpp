#define _CRT_SECURE_NO_WARNINGS
#include "Sensor.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;


//////////////////////////////////////////
//---------- Sensor Setup --------------//
//////////////////////////////////////////
Sensor::Sensor(const string& commandFilePath, const string& logFilePath)
    : commandFile(commandFilePath), logFile(logFilePath), configFile("config.txt"), currentLogLevel(LogLevel::INFO) {
    loadConfig();
}

void Sensor::run() {
    showMenu();

    cout << "Sensor started. Reading " << commandFile << "...\n";
    while (true) {
        checkCommands();
        this_thread::sleep_for(chrono::seconds(2)); // Alle 2 Sekunden prüfen
    }
}