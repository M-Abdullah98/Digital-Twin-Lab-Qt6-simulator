#include "Source\AutonomousDevice.h"
#include <iostream>

void AutonomousDevice::displayInfo() const {
    Device::displayInfo();
    std::cout << "AI Level: "        << aiLevel
              << " | Nav Mode: "     << navigationMode << "\n";
}

void AutonomousDevice::updateState() {
    Device::updateState();
    if (aiLevel > 3) {
        std::cout << name << " is making autonomous decisions.\n";
    }
}

float AutonomousDevice::calculateHealth() const {
    // FIX: clamp result to [0, 100] — original could return negative values
    //   when aiLevel penalty exceeded battery percentage.
    float health = battery.getPercentage() - (aiLevel * 0.5f);
    return (health < 0.0f) ? 0.0f : health;
}

void AutonomousDevice::navigate() {
    std::cout << name << " navigating in " << navigationMode << " mode.\n";
}

void AutonomousDevice::avoidObstacle() {
    std::cout << name << " avoiding obstacle (AI level " << aiLevel << ").\n";
}
