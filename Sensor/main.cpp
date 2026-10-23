#include "Sensor.h"
#include <filesystem>
using namespace std;

int main() {
    const string commandFile = "commands.txt";
    const string logFile = "sensorLog.log";

    // Sicherstellen, dass die Datei existiert
    if (!filesystem::exists(commandFile)) {
        ofstream create(commandFile);
    }

    Sensor sensor(commandFile, logFile);
    sensor.run();
    return 0;
}