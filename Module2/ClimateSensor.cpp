#include "Source\ClimateSensor.h"
#include <iostream>

void ClimateSensor::displayInfo() const {
    EnvironmentalSensor::displayInfo();
    std::cout << "Climate Index: "        << climateIndex
              << " | Storm Level: "        << stormPredictionLevel
              << " | Wind: "              << windSpeed << " m/s"
              << " | Rain: "             << rainLevel  << " mm\n";
}

void ClimateSensor::updateState() {
    EnvironmentalSensor::updateState();
    std::cout << name << " analyzing climate patterns...\n";
}

float ClimateSensor::calculateHealth() const {
    // FIX: clamp to [0, 100].
    float health = climateIndex > 0.8f ? 90.0f : 70.0f;
    return (health < 0.0f) ? 0.0f : health;
}

void ClimateSensor::analyzeClimateTrend() {
    std::cout << name << " — Temp trend: " << temperatureTrend
              << " | Humidity trend: "      << humidityTrend << "\n";
}

void ClimateSensor::predictWeather() {
    // FIX: Produce a meaningful prediction based on stormPredictionLevel.
    if (stormPredictionLevel >= 3) {
        std::cout << name << " WARNING: high storm probability (level "
                  << stormPredictionLevel << ").\n";
    } else {
        std::cout << name << ": weather stable (storm level "
                  << stormPredictionLevel << ").\n";
    }
}

void ClimateSensor::detectClimateChange() {
    std::cout << name << " detecting climate change signals...\n";
}

void ClimateSensor::generateClimateReport() {
    std::cout << "=== Climate Report: " << name << " ===\n";
    displayClimateData();
}

void ClimateSensor::displayClimateData() const {
    displayInfo();
}
