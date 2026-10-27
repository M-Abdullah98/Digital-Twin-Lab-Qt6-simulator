#include "Source\SensorController.h"
#include <iostream>

void SensorController::addSensor(Sensor* s) {
    if (!s) return;
    sensorList.push_back(s);
    std::cout << "Sensor added: " << s->getName() << "\n";
}

void SensorController::removeSensor(int sensorID) {
    for (auto it = sensorList.begin(); it != sensorList.end(); ++it) {
        if ((*it)->getObjectID() == sensorID) {
            std::cout << "Sensor removed: " << (*it)->getName() << "\n";
            sensorList.erase(it);
            return;
        }
    }
    // FIX: Throw instead of silent message (mirrors DeviceController fix).
    throw std::runtime_error("SensorController: sensor ID " +
                             std::to_string(sensorID) + " not found.");
}

void SensorController::updateSensorReading(Sensor* s) {
    if (!s) return;
    s->updateState();
}

void SensorController::listSensors() const {
    if (sensorList.empty()) {
        std::cout << "No sensors registered.\n";
        return;
    }
    for (auto* s : sensorList) s->displayInfo();
}
