#ifndef SIMULATIONCONTROLLER_H
#define SIMULATIONCONTROLLER_H

#include "Module3\Source\SimulationEngine.h"

class SimulationController {
private:
    SimulationEngine engine;

public:
    explicit SimulationController(int interval = 1) : engine(interval) {}

    void startSimulation();
    void pauseSimulation();
    void resetSimulation();

    // Expose engine for device/sensor registration
    SimulationEngine& getEngine() { return engine; }
};

#endif // SIMULATIONCONTROLLER_H
