#ifndef TELEMETRYDATA_H
#define TELEMETRYDATA_H

#include <string>
// FIX: Removed <iostream> from header — only needed in the .cpp.
// FIX: Removed 'using namespace std;' from header.

class TelemetryData {
private:
    float       temperature;
    float       pressure;
    float       humidity;
    float       signalStrength;
    std::string timestamp;
    std::string deviceID;

public:
    // FIX: string parameters passed by const-reference to avoid unnecessary copies.
    explicit TelemetryData(float temp   = 0.0f,
                           float pres   = 0.0f,
                           float hum    = 0.0f,
                           float signal = 100.0f,
                           const std::string& time = "N/A",
                           const std::string& id   = "Unknown");

    void resetTelemetry();

    void updateReading(float temp, float pres, float hum, float signal,
                       const std::string& time, const std::string& id);

    void displayTelemetry() const;
    bool isSignalWeak()     const;

    // Operator Overloads
    TelemetryData operator+(const TelemetryData& other) const;  // average readings
    bool          operator==(const TelemetryData& other) const; // compare readings
    bool          operator>(const TelemetryData& other)  const; // compare signal
};

#endif // TELEMETRYDATA_H
