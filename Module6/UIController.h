#ifndef UICONTROLLER_H
#define UICONTROLLER_H

// ============================================================
//  UIController.h
//  Single bridge between every GUI window and the simulation.
//  All windows talk to this — never to each other directly.
// ============================================================

#include <QObject>
#include <QString>
#include <QVector>
#include <QTimer>

// ── DeviceInfo: holds everything the GUI needs for one device ─
// Now includes X, Y, Z from Coordinate.cpp
struct DeviceInfo {
    int     id;
    QString name;
    QString type;
    int     battery;    // 0–100 %
    bool    active;
    float   altitude;   // Z axis — metres above ground
    float   speed;      // current speed m/s
    float   x;          // Coordinate X (east-west position in metres)
    float   y;          // Coordinate Y (north-south position in metres)
    float   z;          // Coordinate Z (same as altitude for drones)
    bool    cameraOn = false;  // Surveillance Drone camera state
};

// ── SensorInfo ────────────────────────────────────────────────
struct SensorInfo {
    int     id;
    QString name;
    QString type;
    float   temperature;
    float   humidity;
    float   pressure;
    float   signalStrength;
    bool    active;
    float   x;          // Coordinate X
    float   y;          // Coordinate Y
    float   z;          // Coordinate Z
};

// ── SimStats: aggregate numbers broadcast every tick ──────────
struct SimStats {
    int activeDevices;
    int activeSensors;
    int avgBattery;
    int alertCount;
    int signalStrength;
    int simTick;        // seconds elapsed since Start was pressed
};

// ============================================================
//  UIController
// ============================================================
class UIController : public QObject {
    Q_OBJECT

public:
    explicit UIController(QObject* parent = nullptr);

    // ── Authentication ───────────────────────────────────────
    bool authenticate(const QString& username, const QString& pass);

    // ── Simulation controls ──────────────────────────────────
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    // Speed multiplier: 1 = 1 tick/sec, 2 = 2 ticks/sec, etc.
    void setSimulationSpeed(int multiplier);
    bool isSimulationRunning() const { return simRunning; }

    // ── Device commands ──────────────────────────────────────
    void commandTakeoff(int droneId);
    void commandLand(int droneId);
    // dx/dy/dz are applied to X/Y/Z coordinates via Coordinate::translate()
    void commandMove(int droneId, float dx, float dy, float dz);
    void commandEmergencyStop(int droneId);
    void commandSetAltitude(int droneId, float targetAlt);
    void commandSetSpeed(int droneId, float targetSpeed);
    void commandToggleCamera(int droneId);
    void commandToggleTracking(int droneId);
    void commandCharge(int droneId);
    void commandDischarge(int droneId);

    // ── Data queries ─────────────────────────────────────────
    QVector<DeviceInfo> getAllDevices()   const;
    QVector<SensorInfo> getAllSensors()   const;
    SimStats            getSimStats()     const;
    DeviceInfo          getDevice(int id) const;
    SensorInfo          getSensor(int id) const;

    // ── Remove devices/sensors ───────────────────────────────
    void removeDevice(int id);
    void removeSensor(int id);

    // ── Add a device/sensor at runtime ───────────────────────
    void addDevice(const DeviceInfo& device);
    void addSensor(const SensorInfo& sensor);

    // ── Battery access (for MainWindow to sync its own list) ─
    // Returns battery values in the same order as getAllDevices()
    QVector<int> getBatteries() const;

signals:
    // Fired every tick — all windows subscribe to this one signal
    void simTickUpdated(const SimStats& stats);

    // Fired whenever one device changes (battery drain, move, etc.)
    void deviceUpdated(const DeviceInfo& device);

    // Fired whenever sensor readings change
    void sensorUpdated(const SensorInfo& sensor);

    // Fired for log messages (INFO / WARN / ERROR)
    void logEvent(const QString& message, const QString& level);

    // Fired when sim starts or pauses
    void simulationStateChanged(bool running);

private slots:
    // Internal: advances the simulation by one tick
    void onSimTick();

private:
    bool    simRunning  = false;
    int     simTick     = 0;
    int     alertCount  = 0;
    QTimer* tickTimer;

    QVector<DeviceInfo> devices;  // live device states
    QVector<SensorInfo> sensors;  // live sensor states
    QVector<bool>       alertFired; // true once the 15% alert has been sent

    void initDemoData();  // seeds devices/sensors for testing
};

#endif // UICONTROLLER_H
