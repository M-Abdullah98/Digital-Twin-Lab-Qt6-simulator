#ifndef SURVEILLANCEDRONE_H
#define SURVEILLANCEDRONE_H

#include "Drone.h"

class SurveillanceDrone : public Drone {
protected:
    int  zoomLevel;       // Camera zoom factor
    bool trackingEnabled; // Target tracking on/off

public:
    SurveillanceDrone(int id, const std::string& name, const std::string& status,
                      const Battery& bat, const Coordinate& loc, float consumption,
                      int aiLvl, const std::string& navMode,
                      float alt, float spd, bool cam,
                      int zoom, bool tracking)
        : Drone(id, name, status, bat, loc, consumption, aiLvl, navMode, alt, spd, cam),
          zoomLevel(zoom), trackingEnabled(tracking) {}

    void  displayInfo()     const override;
    void  updateState()           override;
    float calculateHealth() const override;

    void trackTarget();
    void stopTracking();   // FIX: Added complement to trackTarget().
    void recordVideo();
};

#endif // SURVEILLANCEDRONE_H
