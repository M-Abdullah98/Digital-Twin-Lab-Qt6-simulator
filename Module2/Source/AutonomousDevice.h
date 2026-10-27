#ifndef AUTONOMOUSDEVICE_H
#define AUTONOMOUSDEVICE_H

#include "Device.h"

class AutonomousDevice : public Device {
protected:
    int         aiLevel;        // FIX: Renamed AIlevel → aiLevel (camelCase convention)
    std::string navigationMode; // e.g., "Manual", "Auto"

public:
    AutonomousDevice(int id, const std::string& name, const std::string& status,
                     const Battery& bat, const Coordinate& loc, float consumption,
                     int aiLvl, const std::string& navMode)
        : Device(id, name, status, bat, loc, consumption),
          aiLevel(aiLvl), navigationMode(navMode) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void navigate();
    void avoidObstacle();

    // Getter
    int         getAiLevel()        const { return aiLevel; }
    std::string getNavigationMode() const { return navigationMode; }
};

#endif // AUTONOMOUSDEVICE_H
