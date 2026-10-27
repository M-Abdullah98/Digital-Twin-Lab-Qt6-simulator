#include "Source\Battery.h"
// FIX: Use forward-slash paths (cross-platform). Backslash paths break on Linux/macOS.

Battery::Battery(float cap, float level, float rate, float health)
    : capacity(cap), currentLevel(level), chargingRate(rate),
      batteryHealth(health), status("Idle") {}

Battery::~Battery() {}

void Battery::charge() {
    status = "Charging";
    currentLevel += chargingRate;
    if (currentLevel > capacity) currentLevel = capacity;
}

void Battery::discharge(float amount) {
    // FIX: Ignore negative discharge amounts to prevent accidental charging.
    if (amount < 0.0f) return;
    status = "Discharging";
    currentLevel -= amount;
    if (currentLevel < 0.0f) currentLevel = 0.0f;
}

float Battery::getPercentage() const {
    // FIX: Guard against division-by-zero when capacity is 0.
    if (capacity <= 0.0f) return 0.0f;
    return (currentLevel / capacity) * 100.0f;
}

bool Battery::isLow() const {
    return getPercentage() < 20.0f;  // threshold at 20 %
}

std::string Battery::getStatus() const {
    return status;
}

void Battery::updateHealth(float decay) {
    // FIX: Ignore negative decay to prevent accidental health gain.
    if (decay < 0.0f) return;
    batteryHealth -= decay;
    if (batteryHealth < 0.0f) batteryHealth = 0.0f;
}

// ── Operator Overloads ────────────────────────────────────────────────────────

Battery Battery::operator+(const Battery& other) const {
    // FIX (original): operator+ combined currentLevel values but passed only
    //   two args to the 4-param constructor, silently leaving chargingRate and
    //   batteryHealth at their defaults. Now averages the remaining fields.
    return Battery(
        capacity + other.capacity,
        currentLevel + other.currentLevel,
        (chargingRate + other.chargingRate) / 2.0f,
        (batteryHealth + other.batteryHealth) / 2.0f
    );
}

Battery Battery::operator-(float amount) const {
    Battery temp = *this;
    if (amount >= 0.0f) {
        temp.currentLevel -= amount;
        if (temp.currentLevel < 0.0f) temp.currentLevel = 0.0f;
    }
    return temp;
}

bool Battery::operator==(const Battery& other) const {
    // FIX: Floating-point equality is unreliable with ==.
    //   Use an epsilon comparison instead.
    const float epsilon = 0.001f;
    return (currentLevel - other.currentLevel < epsilon) &&
           (other.currentLevel - currentLevel < epsilon);
}

bool Battery::operator<(const Battery& other) const {
    return capacity < other.capacity;
}
