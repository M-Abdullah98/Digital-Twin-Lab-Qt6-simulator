#include "UIController.h"
#include <QDateTime>
#include <QRandomGenerator>

// ============================================================
//  UIController.cpp
//  All simulation logic lives here. The GUI never touches
//  battery values or coordinates directly — it only calls
//  commands and reads signals.
// ============================================================

UIController::UIController(QObject *parent) : QObject(parent)
{
    tickTimer = new QTimer(this);
    tickTimer->setInterval(1000); // default: 1 tick per second
    connect(tickTimer, &QTimer::timeout, this, &UIController::onSimTick);
    initDemoData();
}

// ── Authentication ───────────────────────────────────────────
bool UIController::authenticate(const QString &user, const QString &pass)
{
    // TODO: replace with real credential check
    return (user == "admin" && pass == "1234");
}

// ── Simulation controls ──────────────────────────────────────

void UIController::startSimulation()
{
    if (simRunning)
        return;
    simRunning = true;
    tickTimer->start();
    emit simulationStateChanged(true);
    emit logEvent("Simulation started", "INFO");
}

void UIController::pauseSimulation()
{
    if (!simRunning)
        return;
    simRunning = false;
    tickTimer->stop();
    emit simulationStateChanged(false);
    emit logEvent("Simulation paused", "INFO");
}

void UIController::resetSimulation()
{
    pauseSimulation();
    simTick = 0;
    alertCount = 0;
    initDemoData();
    // Tell all windows to re-read fresh device data
    for (const auto &d : devices)
        emit deviceUpdated(d);
    emit simTickUpdated(getSimStats());
    emit logEvent("Simulation reset", "INFO");
}

void UIController::setSimulationSpeed(int multiplier)
{
    // multiplier=1 → 1000ms, multiplier=2 → 500ms, etc.
    int ms = 1000 / qMax(1, multiplier);
    tickTimer->setInterval(ms);
    if (simRunning)
        tickTimer->start(); // restart with new interval
}

// ── Device commands ──────────────────────────────────────────

void UIController::commandTakeoff(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        if (d.battery <= 10)
        {
            emit logEvent(d.name + " takeoff refused — battery too low", "WARN");
            return;
        }
        // Set Z (altitude) to 10m and apply to Coordinate
        d.altitude = 10.0f;
        d.z = 10.0f;
        d.speed = 5.0f;
        d.active = true;
        emit deviceUpdated(d);
        emit logEvent(d.name + " took off — altitude 10 m", "INFO");
        return;
    }
}

void UIController::commandLand(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        d.altitude = 0.0f;
        d.z = 0.0f;
        d.speed = 0.0f;
        d.active = false; // back to STANDBY — stop battery drain
        emit deviceUpdated(d);
        emit logEvent(d.name + " landed successfully", "INFO");
        return;
    }
}

void UIController::commandMove(int droneId, float dx, float dy, float dz)
{
    // Applies dx/dy/dz using the same logic as Coordinate::translate()
    for (auto &d : devices)
    {
        if (d.id != droneId || d.altitude <= 0)
            continue;
        d.x += dx;
        d.y += dy;
        d.z += dz;
        d.altitude = d.z; // keep altitude in sync with Z
        emit deviceUpdated(d);
        emit logEvent(QString("%1 → X:%2 Y:%3 Z:%4")
                          .arg(d.name)
                          .arg(d.x, 0, 'f', 1)
                          .arg(d.y, 0, 'f', 1)
                          .arg(d.z, 0, 'f', 1),
                      "INFO");
        return;
    }
}

void UIController::commandEmergencyStop(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        d.altitude = 0.0f;
        d.speed = 0.0f;
        d.z = 0.0f;
        d.active = false;
        emit deviceUpdated(d);
        emit logEvent("EMERGENCY STOP: " + d.name, "WARN");
        return;
    }
}

void UIController::commandSetAltitude(int droneId, float alt)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        d.altitude = alt;
        d.z = alt;
        emit deviceUpdated(d);
        return;
    }
}

void UIController::commandSetSpeed(int droneId, float spd)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        d.speed = spd;
        emit deviceUpdated(d);
        return;
    }
}

void UIController::commandToggleCamera(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        d.cameraOn = !d.cameraOn;
        emit deviceUpdated(d);
        emit logEvent(d.name + ": camera " + (d.cameraOn ? "ON" : "OFF"), "INFO");
        return;
    }
}

void UIController::commandCharge(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        if (d.battery >= 100)
        {
            emit logEvent(d.name + ": already fully charged", "WARN");
            return;
        }
        d.battery = qMin(100, d.battery + 20);
        emit deviceUpdated(d);
        emit logEvent(QString("%1 charged → battery at %2%").arg(d.name).arg(d.battery), "INFO");
        return;
    }
}

void UIController::commandDischarge(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        if (d.battery <= 0)
        {
            emit logEvent(d.name + ": battery already empty", "WARN");
            return;
        }
        d.battery = qMax(0, d.battery - 20);
        emit deviceUpdated(d);
        emit logEvent(QString("%1 discharged → battery at %2%").arg(d.name).arg(d.battery), "INFO");
        return;
    }
}

void UIController::commandToggleTracking(int droneId)
{
    for (auto &d : devices)
    {
        if (d.id != droneId)
            continue;
        emit logEvent(d.name + ": tracking toggled", "INFO");
        return;
    }
}

// ── Data queries ─────────────────────────────────────────────

QVector<DeviceInfo> UIController::getAllDevices() const { return devices; }
QVector<SensorInfo> UIController::getAllSensors() const { return sensors; }

DeviceInfo UIController::getDevice(int id) const
{
    for (const auto &d : devices)
        if (d.id == id)
            return d;
    return {};
}

SensorInfo UIController::getSensor(int id) const
{
    for (const auto &s : sensors)
        if (s.id == id)
            return s;
    return {};
}

QVector<int> UIController::getBatteries() const
{
    QVector<int> bats;
    for (const auto &d : devices)
        bats << d.battery;
    return bats;
}

void UIController::addDevice(const DeviceInfo &device)
{
    // Avoid duplicate IDs
    for (const auto &d : devices)
        if (d.id == device.id)
            return;
    devices.append(device);
    alertFired.append(false);
    emit logEvent(QString("Drone added to simulation: %1").arg(device.name), "INFO");
    emit simTickUpdated(getSimStats());
}

void UIController::addSensor(const SensorInfo &sensor)
{
    for (const auto &s : sensors)
        if (s.id == sensor.id)
            return;
    sensors.append(sensor);
    emit logEvent(QString("Sensor added to simulation: %1").arg(sensor.name), "INFO");
    emit simTickUpdated(getSimStats());
}

void UIController::removeDevice(int id)
{
    for (int i = 0; i < devices.size(); ++i)
    {
        if (devices[i].id == id)
        {
            QString name = devices[i].name;
            devices.removeAt(i);
            if (i < alertFired.size())
                alertFired.removeAt(i);
            emit logEvent("Device removed from simulation: " + name, "WARN");
            emit simTickUpdated(getSimStats());
            return;
        }
    }
}

void UIController::removeSensor(int id)
{
    for (int i = 0; i < sensors.size(); ++i)
    {
        if (sensors[i].id == id)
        {
            QString name = sensors[i].name;
            sensors.removeAt(i);
            emit logEvent("Sensor removed from simulation: " + name, "WARN");
            emit simTickUpdated(getSimStats());
            return;
        }
    }
}

SimStats UIController::getSimStats() const
{
    SimStats st;
    st.simTick = simTick;
    st.alertCount = alertCount;
    st.activeDevices = 0;
    int totalBat = 0;

    for (const auto &d : devices)
    {
        if (d.active && d.battery > 0)
            st.activeDevices++;
        totalBat += d.battery;
    }
    st.activeSensors = 0;
    for (const auto &s : sensors)
        if (s.active)
            st.activeSensors++;

    st.avgBattery = devices.isEmpty() ? 0 : totalBat / devices.size();

    // Signal is lost when all drones are dead AND no active sensors remain
    bool anyAlive = (st.activeDevices > 0) || (st.activeSensors > 0);
    st.signalStrength = anyAlive ? (85 + QRandomGenerator::global()->bounded(15)) : 0;
    return st;
}

// ── Internal simulation tick ─────────────────────────────────

void UIController::onSimTick()
{
    simTick++;

    // Drain battery and drift position for every ACTIVE device
    for (int i = 0; i < devices.size(); ++i)
    {
        auto &d = devices[i];
        if (!d.active || d.battery <= 0)
            continue;

        // ── Battery drain (1–2 % per tick) ───────────────────
        int drain = QRandomGenerator::global()->bounded(1, 3);
        d.battery = qMax(0, d.battery - drain);

        // ── Device goes offline when battery hits 0 ───────────
        if (d.battery == 0)
        {
            d.active = false;
            d.altitude = 0.0f;
            d.x = 0.0f;
            d.y = 0.0f;
            d.z = 0.0f;
            d.speed = 0.0f;
            emit logEvent("DEVICE OFFLINE: " + d.name + " battery depleted", "ERROR");
        }

        // ── Low battery warning fires once per device at 15 % ─
        if (i < alertFired.size() && d.battery > 0 && d.battery <= 15 && !alertFired[i])
        {
            alertFired[i] = true;
            alertCount++;
            emit logEvent(QString("BATTERY LOW: %1 at %2%")
                              .arg(d.name)
                              .arg(d.battery),
                          "WARN");
        }

        // ── Position drift using X/Y/Z (like Coordinate::translate) ─
        if (d.altitude > 0.0f)
        {
            // Small random drift each tick to simulate real flight
            d.x += (QRandomGenerator::global()->bounded(20) - 10) * 0.1f;
            d.y += (QRandomGenerator::global()->bounded(20) - 10) * 0.1f;
            // Clamp X and Y to non-negative — coordinates are never below ground origin
            if (d.x < 0.0f)
                d.x = 0.0f;
            if (d.y < 0.0f)
                d.y = 0.0f;
            // Altitude (Z) fluctuates slightly but never goes negative
            d.z += (QRandomGenerator::global()->bounded(10) - 5) * 0.1f;
            if (d.z < 0.0f)
                d.z = 0.0f;
            d.altitude = d.z;
        }

        emit deviceUpdated(d);
    }

    // Simulate sensor readings fluctuating each tick
    for (auto &s : sensors)
    {
        if (!s.active)
            continue;
        s.temperature += (QRandomGenerator::global()->bounded(20) - 10) * 0.05f;
        s.humidity += (QRandomGenerator::global()->bounded(10) - 5) * 0.05f;
        s.pressure += (QRandomGenerator::global()->bounded(6) - 3) * 0.02f;
        emit sensorUpdated(s);
        // Emit climate data log so the Climate tab gets populated
        emit logEvent(
            QString("[%1] Temp:%2°C  Hum:%3%  Pres:%4hPa")
                .arg(s.name)
                .arg(s.temperature, 0, 'f', 1)
                .arg(s.humidity, 0, 'f', 1)
                .arg(s.pressure, 0, 'f', 1),
            "CLIMATE");
    }

    // Broadcast aggregate stats — MainWindow and SimulationControlWindow
    // both listen for this and update their stat cards
    emit simTickUpdated(getSimStats());
}

// ── Demo data initialiser ─────────────────────────────────────

void UIController::initDemoData()
{
    devices.clear();
    sensors.clear();
    alertFired.clear();

    // id, name, type, battery, active, altitude, speed, x, y, z
    // active = false on init: drones start on STANDBY; battery only drains when flying
    devices << DeviceInfo{1, "Drone-Alpha", "Surveillance Drone", 100, false, 0, 0, 0.0f, 0.0f, 0.0f};
    devices << DeviceInfo{2, "Drone-Beta", "Autonomous Drone", 100, false, 0, 0, 0.0f, 0.0f, 0.0f};

    alertFired.fill(false, devices.size());

    // id, name, type, temperature, humidity, pressure, signal, active, x, y, z
    // Start with realistic baseline readings so first tick doesn't push values negative
    sensors << SensorInfo{1, "ClimateNode-01", "Climate Sensor", 22.0f, 55.0f, 1013.0f, 90.0f, true, 0, 0, 0};
    sensors << SensorInfo{2, "EnvSensor-02", "Environmental Sensor", 20.0f, 60.0f, 1011.0f, 85.0f, true, 10, 5, 0};
}