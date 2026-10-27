#ifndef CLIMATESENSOR_H
#define CLIMATESENSOR_H

#include "EnvironmentalSensor.h"

class ClimateSensor : public EnvironmentalSensor {
protected:
    float temperatureTrend;
    float humidityTrend;
    float windSpeed;
    float rainLevel;
    float climateIndex;       // 0.0 – 1.0
    int   stormPredictionLevel;

public:
    ClimateSensor(int id, const std::string& name, const std::string& status,
                  int sID, const std::string& type,
                  const TelemetryData& data, const Coordinate& loc,
                  const std::string& mType, const std::string& calDate,
                  float range, const std::string& unit,
                  float tempTrend, float humTrend, float wind,
                  float rain, float index, int stormLevel)
        : EnvironmentalSensor(id, name, status, sID, type, data, loc,
                              mType, calDate, range, unit),
          temperatureTrend(tempTrend), humidityTrend(humTrend),
          windSpeed(wind), rainLevel(rain),
          climateIndex(index), stormPredictionLevel(stormLevel) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void analyzeClimateTrend();
    void predictWeather();
    void detectClimateChange();
    void generateClimateReport();
    void displayClimateData() const;
};

#endif // CLIMATESENSOR_H
