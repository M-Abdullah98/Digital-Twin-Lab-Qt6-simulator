#ifndef SIMULATION_EXCEPTIONS_H
#define SIMULATION_EXCEPTIONS_H

// SimulationExceptions.h
// Centralised exception hierarchy for the Digital Twin Lab simulation.
// All exceptions derive from SimulationException (root), which itself
// derives from std::runtime_error so they integrate with standard C++ catch blocks.
//
// Usage pattern throughout the codebase:
//
//   try {
//       throw SomeException(...);
//   } catch (const SpecificException& e) {
//       // handle specific case
//   } catch (const SimulationException& e) {
//       // fallback for any simulation error
//   } catch (const std::exception& e) {
//       // last resort
//   }

#include <stdexcept>
#include <string>

// ═══════════════════════════════════════════════════════════════════════════════
//  ROOT
//  All simulation exceptions derive from this.
//  Catching SimulationException catches the entire hierarchy.
// ═══════════════════════════════════════════════════════════════════════════════
class SimulationException : public std::runtime_error {
public:
    explicit SimulationException(const std::string& msg)
        : std::runtime_error("[Simulation] " + msg) {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  VALIDATION EXCEPTIONS
//  Thrown when user input or configuration data fails validation checks
//  before any object is created or modified.
//
//  Example usage:
//
//    void addDevice(const std::string& name, const std::string& type) {
//        try {
//            if (name.empty())   throw InvalidNameException("Device");
//            if (type.empty())   throw InvalidTypeException("Device");
//            // ... proceed to create device
//        } catch (const InvalidNameException& e) {
//            logError(e.what());
//        } catch (const InvalidTypeException& e) {
//            logError(e.what());
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class ValidationException : public SimulationException {
public:
    explicit ValidationException(const std::string& msg)
        : SimulationException("[Validation] " + msg) {}
};

class InvalidNameException : public ValidationException {
public:
    // entity: what was being named, e.g. "Device", "Sensor"
    explicit InvalidNameException(const std::string& entity)
        : ValidationException("Name cannot be empty for: " + entity) {}
};

class InvalidTypeException : public ValidationException {
public:
    // entity: what type was being set, e.g. "Device", "Sensor"
    explicit InvalidTypeException(const std::string& entity)
        : ValidationException("Type cannot be empty for: " + entity) {}
};

class InvalidValueException : public ValidationException {
public:
    // field: the field name, e.g. "battery capacity", "speed"
    // value: the bad value as a string
    explicit InvalidValueException(const std::string& field, const std::string& value)
        : ValidationException("Invalid value '" + value + "' for field: " + field) {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  BATTERY EXCEPTIONS
//  Thrown during battery simulation: charging, discharging, health checks.
//
//  Example usage:
//
//    void Battery::discharge(float amount) {
//        try {
//            if (currentLevel <= 0.0f)
//                throw BatteryDepletedException(ownerName);
//            if (currentLevel - amount <= 15.0f)
//                throw BatteryLowException(ownerName, currentLevel - amount);
//            currentLevel -= amount;
//        } catch (const BatteryDepletedException& e) {
//            setActive(false);
//            logError(e.what());
//        } catch (const BatteryLowException& e) {
//            triggerAlert(e.what());
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class BatteryException : public SimulationException {
public:
    explicit BatteryException(const std::string& msg)
        : SimulationException("[Battery] " + msg) {}
};

class BatteryLowException : public BatteryException {
public:
    // deviceName: name of the device whose battery is low
    // pct: current battery percentage at time of throw
    explicit BatteryLowException(const std::string& deviceName, int pct)
        : BatteryException("Battery critically low on '" + deviceName +
                           "' — " + std::to_string(pct) + "% remaining") {}
};

class BatteryDepletedException : public BatteryException {
public:
    // deviceName: name of the device that has run out of power
    explicit BatteryDepletedException(const std::string& deviceName)
        : BatteryException("Battery fully depleted on '" + deviceName +
                           "' — device going offline") {}
};

class BatteryOverchargeException : public BatteryException {
public:
    explicit BatteryOverchargeException(const std::string& deviceName)
        : BatteryException("Battery overcharged beyond safe limit on '" +
                           deviceName + "'") {}
};

class BatteryFailureException : public BatteryException {
public:
    explicit BatteryFailureException(const std::string& deviceName)
        : BatteryException("Battery malfunction detected on '" + deviceName + "'") {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  DEVICE EXCEPTIONS
//  Thrown when device lookup, state, or operational constraints are violated.
//
//  Example usage:
//
//    Device* DeviceController::findDevice(int id) {
//        try {
//            auto it = deviceMap.find(id);
//            if (it == deviceMap.end())
//                throw DeviceNotFoundException(id);
//            if (!it->second->isActive())
//                throw DeviceInactiveException(it->second->getName());
//            return it->second;
//        } catch (const DeviceNotFoundException& e) {
//            logError(e.what());
//            return nullptr;
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class DeviceException : public SimulationException {
public:
    explicit DeviceException(const std::string& msg)
        : SimulationException("[Device] " + msg) {}
};

class DeviceNotFoundException : public DeviceException {
public:
    // id-based lookup failure
    explicit DeviceNotFoundException(int id)
        : DeviceException("Device ID " + std::to_string(id) + " not found") {}
    // context-based (e.g. index mismatch in a vector)
    explicit DeviceNotFoundException(const std::string& context)
        : DeviceException("Device not found — " + context) {}
};

class DeviceInactiveException : public DeviceException {
public:
    explicit DeviceInactiveException(const std::string& deviceName)
        : DeviceException("Device '" + deviceName +
                          "' is inactive and cannot perform this operation") {}
};

class DeviceOverloadException : public DeviceException {
public:
    explicit DeviceOverloadException(const std::string& deviceName)
        : DeviceException("Device '" + deviceName +
                          "' overloaded — too many concurrent tasks") {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  SENSOR EXCEPTIONS
//  Thrown during sensor registration, data reads, calibration, and disconnection.
//
//  Example usage:
//
//    float EnvironmentalSensor::readData() {
//        try {
//            if (!isConnected())
//                throw SensorDisconnectedException(sensorId);
//            if (!isCalibrated())
//                throw SensorCalibrationException(sensorId);
//            return getRawReading();
//        } catch (const SensorDisconnectedException& e) {
//            logError(e.what());
//            return -1.0f;
//        } catch (const SensorCalibrationException& e) {
//            attemptRecalibration();
//            throw;  // rethrow if recalibration fails
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class SensorException : public SimulationException {
public:
    explicit SensorException(const std::string& msg)
        : SimulationException("[Sensor] " + msg) {}
};

class SensorDataException : public SensorException {
public:
    // Thrown when sensor data cannot be produced (e.g. no sensors registered)
    explicit SensorDataException(const std::string& msg)
        : SensorException("Sensor data error — " + msg) {}
};

class SensorFailureException : public SensorException {
public:
    explicit SensorFailureException(const std::string& sensorId)
        : SensorException("Sensor '" + sensorId + "' malfunction detected") {}
};

class SensorCalibrationException : public SensorException {
public:
    explicit SensorCalibrationException(const std::string& sensorId)
        : SensorException("Sensor '" + sensorId + "' calibration failed") {}
};

class SensorDisconnectedException : public SensorException {
public:
    explicit SensorDisconnectedException(const std::string& sensorId)
        : SensorException("Sensor '" + sensorId + "' disconnected from system") {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  COORDINATE EXCEPTIONS
//  Thrown when drone/device positions fall outside valid ranges or are malformed.
//
//  Example usage:
//
//    void Drone::moveTo(const Coordinate& target) {
//        try {
//            if (!target.isValid())
//                throw InvalidCoordinateException(target.toString());
//            if (!bounds.contains(target))
//                throw OutOfBoundsException(target.toString(), bounds.toString());
//            position = target;
//        } catch (const OutOfBoundsException& e) {
//            logWarning(e.what());
//        } catch (const InvalidCoordinateException& e) {
//            logError(e.what());
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class CoordinateException : public SimulationException {
public:
    explicit CoordinateException(const std::string& msg)
        : SimulationException("[Coordinate] " + msg) {}
};

class InvalidCoordinateException : public CoordinateException {
public:
    explicit InvalidCoordinateException(const std::string& coord)
        : CoordinateException("Invalid coordinate: " + coord) {}
};

class OutOfBoundsException : public CoordinateException {
public:
    explicit OutOfBoundsException(const std::string& coord,
                                  const std::string& bounds = "simulation area")
        : CoordinateException("Coordinate " + coord +
                              " is outside bounds: " + bounds) {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  TELEMETRY EXCEPTIONS
//  Thrown when telemetry data cannot be received, parsed, or is corrupted.
//
//  Example usage:
//
//    TelemetryData TelemetryViewer::fetchLatest(int deviceId) {
//        try {
//            if (isTimedOut(deviceId))
//                throw TelemetryTimeoutException(deviceId);
//            TelemetryData data = decode(rawBuffer);
//            if (!data.isValid())
//                throw TelemetryDataCorruptException(deviceId);
//            return data;
//        } catch (const TelemetryTimeoutException& e) {
//            logWarning(e.what());
//            return TelemetryData::empty();
//        } catch (const TelemetryDataCorruptException& e) {
//            logError(e.what());
//            return TelemetryData::empty();
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class TelemetryException : public SimulationException {
public:
    explicit TelemetryException(const std::string& msg)
        : SimulationException("[Telemetry] " + msg) {}
};

class TelemetryDataCorruptException : public TelemetryException {
public:
    explicit TelemetryDataCorruptException(int deviceId)
        : TelemetryException("Telemetry data corrupted for device ID " +
                             std::to_string(deviceId)) {}
};

class TelemetryTimeoutException : public TelemetryException {
public:
    explicit TelemetryTimeoutException(int deviceId)
        : TelemetryException("Telemetry timeout — no data received from device ID " +
                             std::to_string(deviceId) + " within time limit") {}
};


// ═══════════════════════════════════════════════════════════════════════════════
//  SIMULATION ENGINE EXCEPTIONS
//  Thrown by SimulationEngine/SimulationController for high-level failures:
//  state corruption, timeout, persistence errors, empty-state ticks.
//
//  Example usage:
//
//    void SimulationEngine::tick() {
//        try {
//            if (devices.empty())
//                throw SimulationStateException("no devices registered");
//            if (elapsedTime > maxTime)
//                throw SimulationTimeoutException(elapsedTime);
//            runOneTick();
//        } catch (const SimulationStateException& e) {
//            logWarning(e.what());
//            return;  // skip tick gracefully
//        } catch (const SimulationTimeoutException& e) {
//            logError(e.what());
//            stop();
//        } catch (const SimulationException& e) {
//            logError(e.what());  // catch-all for any simulation error
//        }
//    }
// ═══════════════════════════════════════════════════════════════════════════════
class SimulationEngineException : public SimulationException {
public:
    explicit SimulationEngineException(const std::string& msg)
        : SimulationException("[Engine] " + msg) {}
};

class SimulationStateException : public SimulationEngineException {
public:
    // Thrown when the simulation is in an invalid state to proceed
    explicit SimulationStateException(const std::string& reason)
        : SimulationEngineException("Invalid state — " + reason) {}
};

class SimulationTimeoutException : public SimulationEngineException {
public:
    explicit SimulationTimeoutException(double elapsedSeconds = 0.0)
        : SimulationEngineException(
              "Simulation exceeded time limit" +
              (elapsedSeconds > 0.0
                   ? " (" + std::to_string(static_cast<int>(elapsedSeconds)) + "s elapsed)"
                   : "")) {}
};

class SimulationDataPersistenceException : public SimulationEngineException {
public:
    // operation: "save" or "load"
    // filename: the file that failed
    explicit SimulationDataPersistenceException(const std::string& operation,
                                                const std::string& filename)
        : SimulationEngineException("Failed to " + operation +
                                    " simulation data: " + filename) {}
};

#endif // SIMULATION_EXCEPTIONS_H