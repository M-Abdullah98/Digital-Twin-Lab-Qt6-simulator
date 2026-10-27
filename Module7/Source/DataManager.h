#ifndef DATAMANAGER_H
#define DATAMANAGER_H

// ============================================================
//  DataManager.h  —  Module 7: File Persistence
//
//  Responsibilities:
//    • Save / load DeviceInfo records  →  devices.dat
//    • Save / load SensorInfo records  →  sensors.dat
//    • Append timestamped simulation events → simulation_logs.txt
//
//  File format (both .dat files):
//    Plain-text, one record per line, fields separated by '|'.
//    First line is a version/header tag so we can detect stale files.
//
//  Usage from UIController (or MainWindow):
//    DataManager dm;                       // default paths
//    dm.saveDevices(controller->getAllDevices());
//    dm.saveSensors(controller->getAllSensors());
//    dm.log("Simulation started", "INFO");
//
//    auto devs = dm.loadDevices();         // QVector<DeviceInfo>
//    auto sens = dm.loadSensors();         // QVector<SensorInfo>
// ============================================================

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <stdexcept>

// Forward-include the shared structs from the GUI layer.
// DataManager lives in Module7 but only depends on the plain-old-data
// structs — it never touches Qt widgets.
#include "../../Module6/UIController.h"

// ── Custom exceptions ─────────────────────────────────────────

class DataManagerException : public std::runtime_error {
public:
    explicit DataManagerException(const std::string& msg)
        : std::runtime_error("[DataManager] " + msg) {}
};

class FileOpenException : public DataManagerException {
public:
    explicit FileOpenException(const std::string& path)
        : DataManagerException("Cannot open file: " + path) {}
};

class FileParseException : public DataManagerException {
public:
    explicit FileParseException(const std::string& detail)
        : DataManagerException("Parse error — " + detail) {}
};

class FileWriteException : public DataManagerException {
public:
    explicit FileWriteException(const std::string& path)
        : DataManagerException("Write failed: " + path) {}
};

// ============================================================
//  DataManager
// ============================================================
class DataManager {
public:
    // ── Constructor ──────────────────────────────────────────
    // dataDir  — directory where .dat and .txt files are written.
    //            Defaults to the application's working directory.
    explicit DataManager(const QString& dataDir = QString());

    // ── Device persistence ───────────────────────────────────

    // Serialise every DeviceInfo in the vector to devices.dat.
    // Overwrites any previous file.
    // Throws FileOpenException / FileWriteException on I/O failure.
    void saveDevices(const QVector<DeviceInfo>& devices);

    // Deserialise devices.dat and return all stored DeviceInfo records.
    // Returns an empty vector (not an exception) when the file does not
    // exist yet — that is the normal first-run case.
    // Throws FileParseException if the file is present but malformed.
    QVector<DeviceInfo> loadDevices();

    // ── Sensor persistence ───────────────────────────────────

    // Serialise every SensorInfo in the vector to sensors.dat.
    void saveSensors(const QVector<SensorInfo>& sensors);

    // Deserialise sensors.dat and return all stored SensorInfo records.
    QVector<SensorInfo> loadSensors();

    // ── Simulation log ───────────────────────────────────────

    // Append one timestamped entry to simulation_logs.txt.
    // level should be "INFO", "WARN", "ERROR", or "CLIMATE".
    // Never throws — log failures are silently ignored so they cannot
    // crash the simulation.
    void log(const QString& message, const QString& level = "INFO");

    // Write a section separator to simulation_logs.txt
    // (used when a new simulation session begins).
    void logSessionStart();

    // Read all lines from simulation_logs.txt and return them.
    // Throws FileOpenException if the file cannot be opened.
    QStringList readLog();

    // Delete the log file, starting fresh.
    void clearLog();

    // ── Path helpers (public so UI can display them) ─────────
    QString devicesPath() const;
    QString sensorsPath()  const;
    QString logPath()      const;

    // ── File-format version string ───────────────────────────
    static constexpr const char* kDeviceHeader  = "DTL_DEVICES_V1";
    static constexpr const char* kSensorHeader  = "DTL_SENSORS_V1";
    static constexpr const char* kFieldSep      = "|";

private:
    QString m_dataDir;

    // ── Internal helpers ─────────────────────────────────────
    // Serialise / deserialise one DeviceInfo ↔ one text line
    QString    deviceToLine(const DeviceInfo&  d) const;
    DeviceInfo lineToDevice(const QString& line, int lineNum) const;

    QString    sensorToLine(const SensorInfo&  s) const;
    SensorInfo lineToSensor(const QString& line, int lineNum) const;

    // Ensure m_dataDir exists; throws DataManagerException if it cannot
    // be created
    void ensureDataDir() const;
};

#endif // DATAMANAGER_H
