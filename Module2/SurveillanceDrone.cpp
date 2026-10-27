#include "Source\SurveillanceDrone.h"
#include <iostream>

void SurveillanceDrone::displayInfo() const {
    Drone::displayInfo();
    std::cout << "Zoom Level: " << zoomLevel
              << " | Tracking: " << (trackingEnabled ? "Enabled" : "Disabled") << "\n";
}

void SurveillanceDrone::updateState() {
    Drone::updateState();
    if (trackingEnabled) {
        std::cout << name << " is actively tracking a target.\n";
    }
}

float SurveillanceDrone::calculateHealth() const {
    // FIX: clamp to [0, 100].
    float health = Drone::calculateHealth() - (zoomLevel * 0.2f);
    return (health < 0.0f) ? 0.0f : health;
}

void SurveillanceDrone::trackTarget() {
    trackingEnabled = true;
    std::cout << name << " started tracking target.\n";
}

void SurveillanceDrone::stopTracking() {
    trackingEnabled = false;
    std::cout << name << " stopped tracking.\n";
}

void SurveillanceDrone::recordVideo() {
    if (cameraEnabled) {
        std::cout << name << " recording video at zoom x" << zoomLevel << ".\n";
    } else {
        std::cout << name << ": cannot record — camera is disabled.\n";
    }
}
