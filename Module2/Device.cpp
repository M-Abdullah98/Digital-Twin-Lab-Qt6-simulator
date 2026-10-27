#include "Source\Device.h"
#include <iostream>

void Device::displayInfo() const {
    std::cout << "Device ID: "  << objectID
              << " | Name: "    << name
              << " | Status: "  << status
              << " | Power: "   << battery.getPercentage() << " %\n";
}

void Device::updateState() {
    if (status == "Active") {
        battery.discharge(powerConsumption);
    }
}

float Device::calculateHealth() const {
    return battery.getPercentage();
}

void Device::start() {
    status = "Active";
    std::cout << name << " started.\n";
}

void Device::stop() {
    status = "Inactive";
    std::cout << name << " stopped.\n";
}

void Device::move(const Coordinate& newLocation) {
    location = newLocation;
    std::cout << name << " moved to new coordinates.\n";
}

void Device::displayDeviceInfo() const {
    displayInfo();
    location.display();
}

void Device::consumeBattery(float amount) {
    battery.discharge(amount);
}

Coordinate Device::getLocation() const {
    return location;
}

Battery& Device::getBattery() {
    return battery;
}

const Battery& Device::getBattery() const {
    return battery;
}

bool Device::operator==(const Device& other) const {
    return objectID == other.objectID;
}

bool Device::operator<(const Device& other) const {
    return powerConsumption < other.powerConsumption;
}
