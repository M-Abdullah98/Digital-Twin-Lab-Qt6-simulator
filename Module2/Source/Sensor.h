#ifndef SENSOR_H
#define SENSOR_H

#include "DigitalObject.h"
#include "Module1\Source\TelemetryData.h"
#include "Module1\Source\Coordinate.h"

class Sensor : public DigitalObject {
protected:
    int            sensorID;
    std::string    sensorType;
    TelemetryData  telemetry;
    Coordinate     location;

public:
    Sensor(int id, const std::string& name, const std::string& status,
           int sID, const std::string& type,
           const TelemetryData& data, const Coordinate& loc)
        : DigitalObject(id, name, status),
          sensorID(sID), sensorType(type), telemetry(data), location(loc) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void          readData();
    void          updateTelemetry(const TelemetryData& newData);
    void          displaySensorInfo() const;
    void          activate();
    void          deactivate();
    TelemetryData getReading() const;

    // Getter
    int         getSensorID()   const { return sensorID; }
    std::string getSensorType() const { return sensorType; }
};

#endif // SENSOR_H
