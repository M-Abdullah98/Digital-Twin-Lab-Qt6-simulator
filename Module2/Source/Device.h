#ifndef DEVICE_H
#define DEVICE_H

// FIX: All includes now use forward-slash paths (cross-platform).
#include "DigitalObject.h"
#include "Module1\Source\Battery.h"
#include "Module1\Source\Coordinate.h"

class Device : public DigitalObject {
protected:
    Battery    battery;           // Device power source
    Coordinate location;          // Device position in simulation
    float      powerConsumption;  // Power usage rate per tick

public:
    Device(int id, const std::string& name, const std::string& status,
           const Battery& bat, const Coordinate& loc, float consumption)
        : DigitalObject(id, name, status),
          battery(bat), location(loc), powerConsumption(consumption) {}

    // Override pure virtual functions
    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    // Device-specific methods
    void start();
    void stop();
    void move(const Coordinate& newLocation);
    void displayDeviceInfo() const;
    void consumeBattery(float amount);

    // Getters
    Coordinate       getLocation()  const;
    Battery&         getBattery();
    const Battery&   getBattery()   const;
    float            getPowerConsumption() const { return powerConsumption; }

    // Operator overloads
    bool operator==(const Device& other) const;
    bool operator<(const Device& other)  const;
};

#endif // DEVICE_H
