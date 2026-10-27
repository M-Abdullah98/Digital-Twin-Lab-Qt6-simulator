#include "Source\DeviceController.h"
#include <iostream>

void DeviceController::registerDevice(Device* d) {
    // FIX: Null-pointer guard.
    if (!d) return;
    deviceList.push_back(d);
    std::cout << "Device registered: " << d->getName() << "\n";
}

void DeviceController::removeDevice(int objectID) {
    for (auto it = deviceList.begin(); it != deviceList.end(); ++it) {
        if ((*it)->getObjectID() == objectID) {
            std::cout << "Device removed: " << (*it)->getName() << "\n";
            deviceList.erase(it);
            return;
        }
    }
    // FIX: Throw a proper exception instead of silent console message.
    throw std::runtime_error("DeviceController: device ID " +
                             std::to_string(objectID) + " not found.");
}

Device* DeviceController::searchDevice(int objectID) const {
    for (auto* d : deviceList) {
        if (d->getObjectID() == objectID) return d;
    }
    return nullptr;  // Caller must check for null.
}

void DeviceController::updateDevice(Device* d) {
    if (!d) return;
    d->updateState();
}

void DeviceController::listAllDevices() const {
    if (deviceList.empty()) {
        std::cout << "No devices registered.\n";
        return;
    }
    for (auto* d : deviceList) d->displayInfo();
}
