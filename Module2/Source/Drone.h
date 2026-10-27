#ifndef DRONE_H
#define DRONE_H

#include "AutonomousDevice.h"

class Drone : public AutonomousDevice {
protected:
    float altitude;      // Current altitude (metres)
    float speed;         // Current speed (m/s)
    bool  cameraEnabled; // Camera on/off

public:
    Drone(int id, const std::string& name, const std::string& status,
          const Battery& bat, const Coordinate& loc, float consumption,
          int aiLvl, const std::string& navMode,
          float alt, float spd, bool cam)
        : AutonomousDevice(id, name, status, bat, loc, consumption, aiLvl, navMode),
          altitude(alt), speed(spd), cameraEnabled(cam) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void takeOff();
    void land();
    void captureImage();

    // Getters
    float getAltitude() const { return altitude; }
    float getSpeed()    const { return speed; }

    // Operator overloads
    float operator-(const Drone& other) const;  // distance between drones
    Drone operator+(float altIncrease)  const;  // increase altitude
};

#endif // DRONE_H
