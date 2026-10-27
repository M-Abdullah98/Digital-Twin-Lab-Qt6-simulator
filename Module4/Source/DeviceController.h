#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <vector>
#include "Module2\Source\Device.h"

class DeviceController {
private:
    std::vector<Device*> deviceList;  // Non-owning pointers

public:
    void    registerDevice(Device* d);
    void    removeDevice(int objectID);
    Device* searchDevice(int objectID)  const;
    void    updateDevice(Device* d);
    void    listAllDevices()            const;

    // FIX: Added size query — useful for GUI and SimulationEngine.
    int     getDeviceCount()            const { return static_cast<int>(deviceList.size()); }
};

#endif // DEVICECONTROLLER_H
