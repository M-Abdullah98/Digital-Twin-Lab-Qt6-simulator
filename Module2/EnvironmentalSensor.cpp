#include "Source\EnvironmentalSensor.h"
#include <iostream>

void EnvironmentalSensor::displayInfo() const {
    Sensor::displayInfo();
    std::cout << "Measurement: " << measurementType
              << " | Range: "    << measurementRange
              << " "             << unitOfMeasurement
              << " | Calibrated: " << (isCalibrated ? "Yes" : "No") << "\n";
}

void EnvironmentalSensor::updateState() {
    Sensor::updateState();
    if (!isCalibrated) {
        std::cout << name << " requires calibration.\n";
    }
}

float EnvironmentalSensor::calculateHealth() const {
    return isCalibrated ? 100.0f : 70.0f;
}

void EnvironmentalSensor::calibrateSensor() {
    isCalibrated = true;
    std::cout << name << " calibrated on " << calibrationDate << ".\n";
}

void EnvironmentalSensor::measureEnvironment() {
    // FIX: Guard against measuring while uncalibrated.
    if (!isCalibrated) {
        std::cout << name << ": measurement skipped — sensor not calibrated.\n";
        return;
    }
    std::cout << name << " measuring " << measurementType << "...\n";
}

void EnvironmentalSensor::validateReading() {
    std::cout << name << " validating measurement...\n";
}

void EnvironmentalSensor::displayEnvironmentData() const {
    displayInfo();
}

void EnvironmentalSensor::resetSensor() {
    isCalibrated = false;
    std::cout << name << " reset — recalibration required.\n";
}
