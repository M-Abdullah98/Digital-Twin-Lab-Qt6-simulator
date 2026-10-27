#ifndef ENVIRONMENTALSENSOR_H
#define ENVIRONMENTALSENSOR_H

#include "Sensor.h"

class EnvironmentalSensor : public Sensor {
protected:
    std::string measurementType;
    std::string calibrationDate;
    float       measurementRange;
    std::string unitOfMeasurement;
    bool        isCalibrated;

public:
    EnvironmentalSensor(int id, const std::string& name, const std::string& status,
                        int sID, const std::string& type,
                        const TelemetryData& data, const Coordinate& loc,
                        const std::string& mType, const std::string& calDate,
                        float range, const std::string& unit)
        : Sensor(id, name, status, sID, type, data, loc),
          measurementType(mType), calibrationDate(calDate),
          measurementRange(range), unitOfMeasurement(unit),
          isCalibrated(false) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void calibrateSensor();
    void measureEnvironment();
    void validateReading();
    void displayEnvironmentData() const;
    void resetSensor();

    bool getIsCalibrated() const { return isCalibrated; }
};

#endif // ENVIRONMENTALSENSOR_H
