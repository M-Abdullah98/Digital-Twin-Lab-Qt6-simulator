#include "Source\SimulationController.h"
#include <iostream>

void SimulationController::startSimulation() {
    engine.startSimulation();
    // FIX: Removed redundant duplicate log; engine already prints "Simulation started."
}

void SimulationController::pauseSimulation() {
    engine.pauseSimulation();
}

void SimulationController::resetSimulation() {
    engine.resetSimulation();
}
