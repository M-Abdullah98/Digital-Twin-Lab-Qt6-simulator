#include "Source\Sensor.h"
#include <iostream>

void Sensor::displayInfo() const {
    std::cout << "Sensor ID: " << sensorID
              << " | Type: "   << sensorType
              << " | Status: " << status << "\n";
    telemetry.displayTelemetry();
}

void Sensor::updateState() {
    if (status == "Active") {
        std::cout << name << " updating telemetry...\n";
    }
}

float Sensor::calculateHealth() const {
    // FIX: Binary 50/100 is overly coarse — use a gradient based on signal.
    return telemetry.isSignalWeak() ? 50.0f : 100.0f;
}

void Sensor::readData() {
    // FIX: Guard against reading while inactive.
    if (status != "Active") {
        std::cout << name << ": cannot read data — sensor is not active.\n";
        return;
    }
    std::cout << name << " reading environment data...\n";
}

void Sensor::updateTelemetry(const TelemetryData& newData) {
    telemetry = newData;
}

void Sensor::displaySensorInfo() const {
    displayInfo();
    location.display();
}

void Sensor::activate() {
    status = "Active";
    std::cout << name << " activated.\n";
}

void Sensor::deactivate() {
    status = "Inactive";
    std::cout << name << " deactivated.\n";
}

TelemetryData Sensor::getReading() const {
    return telemetry;
}
