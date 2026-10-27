#ifndef BATTERY_H
#define BATTERY_H

#include <string>


class Battery {
private:
    float capacity;       // Maximum capacity in mAh
    float currentLevel;   // Current charge level in mAh
    float chargingRate;   // Rate of charging in mAh per cycle
    float batteryHealth;  // Health percentage (0–100)
    std::string status;   // "Charging", "Discharging", "Idle"

public:
    // Constructor
    // FIX: added explicit to prevent accidental implicit conversions.
    explicit Battery(float cap = 1000.0f, float level = 500.0f,
                     float rate = 50.0f, float health = 100.0f);

    // FIX: virtual destructor added so derived-class cleanup is safe
    //      if Battery is ever used polymorphically.
    virtual ~Battery();

    void charge();
    void discharge(float amount);

    // FIX: capacity == 0 guard in getPercentage() (see Battery.cpp)
    float getPercentage() const;

    bool isLow() const;
    std::string getStatus() const;
    void updateHealth(float decay = 0.1f);

    // Operator Overloads
    Battery operator+(const Battery& other) const;  // combine capacity
    Battery operator-(float amount)         const;  // reduce level
    bool   operator==(const Battery& other) const;  // compare level
    bool   operator<(const Battery& other)  const;  // compare capacity
};

#endif // BATTERY_H
