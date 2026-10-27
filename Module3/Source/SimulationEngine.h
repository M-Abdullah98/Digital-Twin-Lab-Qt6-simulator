#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

// FIX: Header filename case standardised to match class name (SimulationEngine.h).
//   Original was lowercase "simulationEngine.h" — inconsistent with all other headers.

#include <vector>
#include "Module2\Source\Device.h"
#include "Module2\Source\Sensor.h"

class SimulationEngine {
private:
    std::vector<Device*> devices;      // Registered devices (non-owning pointers)
    std::vector<Sensor*> sensors;      // Registered sensors (non-owning pointers)
    int  simulationTime;               // Current simulation tick count
    bool isRunning;                    // Simulation active flag
    int  updateInterval;               // Ticks per update cycle

public:
    explicit SimulationEngine(int interval = 1)
        : simulationTime(0), isRunning(false), updateInterval(interval) {}

    // Core controls
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();

    // Registration
    void registerDevice(Device* d);
    void registerSensor(Sensor* s);

    // Simulation logic
    void updateDeviceState();
    void simulateMovement();
    void simulateBatteryUsage();
    void simulateSensorData();

    // State queries
    bool getIsRunning()    const { return isRunning; }
    int  getSimulationTime() const { return simulationTime; }
};

#endif // SIMULATIONENGINE_H
