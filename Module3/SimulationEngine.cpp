#include "Source\SimulationEngine.h"
#include <iostream>

void SimulationEngine::startSimulation() {
    // FIX: Guard against double-starting.
    if (isRunning) {
        std::cout << "Simulation is already running.\n";
        return;
    }
    isRunning = true;
    std::cout << "Simulation started.\n";
}

void SimulationEngine::pauseSimulation() {
    isRunning = false;
    std::cout << "Simulation paused at tick " << simulationTime << ".\n";
}

void SimulationEngine::resetSimulation() {
    simulationTime = 0;
    isRunning      = false;
    // FIX: Also reset device/sensor state collections on full reset.
    devices.clear();
    sensors.clear();
    std::cout << "Simulation reset.\n";
}

void SimulationEngine::registerDevice(Device* d) {
    // FIX: Null-pointer guard.
    if (d) devices.push_back(d);
}

void SimulationEngine::registerSensor(Sensor* s) {
    if (s) sensors.push_back(s);
}

void SimulationEngine::updateDeviceState() {
    if (!isRunning) return;
    simulationTime += updateInterval;
    for (auto* d : devices) d->updateState();
    for (auto* s : sensors) s->updateState();
}

void SimulationEngine::simulateMovement() {
    if (!isRunning) return;
    for (auto* d : devices) {
        Coordinate newLoc = d->getLocation();
        newLoc.translate(1.0f, 0.5f, 0.2f);
        d->move(newLoc);
    }
}

void SimulationEngine::simulateBatteryUsage() {
    if (!isRunning) return;
    for (auto* d : devices) {
        // FIX (original): used calculateHealth() as drain amount — health and
        //   drain are unrelated concepts. Use powerConsumption instead.
        d->consumeBattery(d->getPowerConsumption() * 0.05f);
    }
}

void SimulationEngine::simulateSensorData() {
    if (!isRunning) return;
    for (auto* s : sensors) s->readData();
}
