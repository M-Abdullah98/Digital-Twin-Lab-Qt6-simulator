#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include <vector>
#include <stdexcept>
#include "Module2\Source\Sensor.h"

class SensorController {
private:
    std::vector<Sensor*> sensorList;  // Non-owning pointers

public:
    void    addSensor(Sensor* s);
    void    removeSensor(int sensorID);
    void    updateSensorReading(Sensor* s);
    void    listSensors() const;

    int     getSensorCount() const { return static_cast<int>(sensorList.size()); }
};

#endif // SENSORCONTROLLER_H
