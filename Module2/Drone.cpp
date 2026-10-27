#include "Source\Drone.h"
#include <iostream>

void Drone::displayInfo() const {
    AutonomousDevice::displayInfo();
    std::cout << "Altitude: " << altitude
              << " | Speed: " << speed
              << " | Camera: " << (cameraEnabled ? "On" : "Off") << "\n";
}

void Drone::updateState() {
    AutonomousDevice::updateState();
    if (status == "Flying") {  // FIX: was "Active" — Drone sets status to "Flying" on takeOff()
        altitude += 0.1f;
        speed    += 0.05f;
    }
}

float Drone::calculateHealth() const {
    // FIX: clamp to [0, 100].
    float health = battery.getPercentage() - (speed * 0.1f);
    return (health < 0.0f) ? 0.0f : health;
}

void Drone::takeOff() {
    altitude = 10.0f;
    status   = "Flying";
    std::cout << name << " took off.\n";
}

void Drone::land() {
    altitude = 0.0f;
    speed    = 0.0f;  // FIX: speed should reset to 0 on landing.
    status   = "Landed";
    std::cout << name << " landed.\n";
}

void Drone::captureImage() {
    if (cameraEnabled) {
        std::cout << name << " captured an image at altitude " << altitude << " m.\n";
    } else {
        std::cout << name << ": camera is disabled.\n";
    }
}

float Drone::operator-(const Drone& other) const {
    return location.distanceTo(other.location);
}

Drone Drone::operator+(float altIncrease) const {
    Drone temp = *this;
    // FIX: guard against negative increase silently lowering altitude.
    if (altIncrease > 0.0f)
        temp.altitude += altIncrease;
    return temp;
}
