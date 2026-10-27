#include "Source\TelemetryData.h"
#include <iostream>
#include <cmath>  // std::fabs

TelemetryData::TelemetryData(float temp, float pres, float hum,
                             float signal,
                             const std::string& time,
                             const std::string& id)
    : temperature(temp), pressure(pres), humidity(hum),
      signalStrength(signal), timestamp(time), deviceID(id) {}

void TelemetryData::resetTelemetry() {
    temperature = pressure = humidity = 0.0f;
    signalStrength = 100.0f;
    timestamp = "N/A";
    deviceID  = "Unknown";
}

void TelemetryData::updateReading(float temp, float pres, float hum,
                                  float signal,
                                  const std::string& time,
                                  const std::string& id) {
    temperature    = temp;
    pressure       = pres;
    humidity       = hum;
    signalStrength = signal;
    timestamp      = time;
    deviceID       = id;
}

void TelemetryData::displayTelemetry() const {
    std::cout << "Device: "          << deviceID       << " | Time: " << timestamp << "\n"
              << "Temperature: "     << temperature     << " °C\n"
              << "Pressure: "        << pressure        << " Pa\n"
              << "Humidity: "        << humidity        << " %\n"
              << "Signal Strength: " << signalStrength  << " %\n";
}

bool TelemetryData::isSignalWeak() const {
    return signalStrength < 30.0f;  // threshold at 30 %
}

// ── Operator Overloads ────────────────────────────────────────────────────────

TelemetryData TelemetryData::operator+(const TelemetryData& other) const {
    // Semantics: average of both readings (documented intent).
    return TelemetryData(
        (temperature    + other.temperature)    / 2.0f,
        (pressure       + other.pressure)       / 2.0f,
        (humidity       + other.humidity)       / 2.0f,
        (signalStrength + other.signalStrength) / 2.0f,
        timestamp, deviceID
    );
}

bool TelemetryData::operator==(const TelemetryData& other) const {
    // FIX: Use epsilon comparison for all float members.
    const float eps = 1e-4f;
    return (std::fabs(temperature    - other.temperature)    < eps) &&
           (std::fabs(pressure       - other.pressure)       < eps) &&
           (std::fabs(humidity       - other.humidity)       < eps) &&
           (std::fabs(signalStrength - other.signalStrength) < eps);
}

bool TelemetryData::operator>(const TelemetryData& other) const {
    return signalStrength > other.signalStrength;
}
