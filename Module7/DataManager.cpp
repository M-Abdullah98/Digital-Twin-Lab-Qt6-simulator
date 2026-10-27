// ============================================================
//  DataManager.cpp  —  Module 7: File Persistence
//
//  File formats
//  ─────────────────────────────────────────────────────────
//  devices.dat
//    Line 0 : DTL_DEVICES_V1          (version/magic header)
//    Line 1+: id|name|type|battery|active|altitude|speed|x|y|z
//
//  sensors.dat
//    Line 0 : DTL_SENSORS_V1
//    Line 1+: id|name|type|temperature|humidity|pressure|signal|active|x|y|z
//
//  simulation_logs.txt
//    [2026-05-07 01:12:17] [INFO]  Simulation started
//    [2026-05-07 01:12:18] [WARN]  BATTERY LOW: Drone-Alpha at 15%
//    ...
// ============================================================

#include "DataManager.h"
#include <QCoreApplication>
#include <QFileInfo>

// ============================================================
//  Constructor
// ============================================================
DataManager::DataManager(const QString& dataDir) {
    if (dataDir.isEmpty()) {
        // Default: place data files next to the executable
        m_dataDir = QCoreApplication::applicationDirPath();
    } else {
        m_dataDir = dataDir;
    }
}

// ============================================================
//  Path helpers
// ============================================================
QString DataManager::devicesPath() const {
    return m_dataDir + QDir::separator() + "devices.dat";
}

QString DataManager::sensorsPath() const {
    return m_dataDir + QDir::separator() + "sensors.dat";
}

QString DataManager::logPath() const {
    return m_dataDir + QDir::separator() + "simulation_logs.txt";
}

// ============================================================
//  ensureDataDir  —  create the data directory if needed
// ============================================================
void DataManager::ensureDataDir() const {
    QDir dir(m_dataDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            throw DataManagerException(
                "Cannot create data directory: " + m_dataDir.toStdString());
        }
    }
}


// ============================================================
//  ── Serialisation helpers ────────────────────────────────
// ============================================================

// DeviceInfo  →  "id|name|type|battery|active|altitude|speed|x|y|z"
QString DataManager::deviceToLine(const DeviceInfo& d) const {
    return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10")
        .arg(d.id)
        .arg(d.name)
        .arg(d.type)
        .arg(d.battery)
        .arg(d.active ? 1 : 0)
        .arg(d.altitude, 0, 'f', 4)
        .arg(d.speed,    0, 'f', 4)
        .arg(d.x,        0, 'f', 4)
        .arg(d.y,        0, 'f', 4)
        .arg(d.z,        0, 'f', 4);
}

// "id|name|type|battery|active|altitude|speed|x|y|z"  →  DeviceInfo
DeviceInfo DataManager::lineToDevice(const QString& line, int lineNum) const {
    const QStringList f = line.split(kFieldSep);
    if (f.size() != 10) {
        throw FileParseException(
            "devices.dat line " + std::to_string(lineNum) +
            ": expected 10 fields, got " + std::to_string(f.size()));
    }

    bool ok = true;
    DeviceInfo d;
    d.id       = f[0].toInt(&ok);       if (!ok) goto bad;
    d.name     = f[1];
    d.type     = f[2];
    d.battery  = f[3].toInt(&ok);       if (!ok) goto bad;
    d.active   = (f[4].toInt(&ok) != 0); if (!ok) goto bad;
    d.altitude = f[5].toFloat(&ok);     if (!ok) goto bad;
    d.speed    = f[6].toFloat(&ok);     if (!ok) goto bad;
    d.x        = f[7].toFloat(&ok);     if (!ok) goto bad;
    d.y        = f[8].toFloat(&ok);     if (!ok) goto bad;
    d.z        = f[9].toFloat(&ok);     if (!ok) goto bad;
    return d;

bad:
    throw FileParseException(
        "devices.dat line " + std::to_string(lineNum) +
        ": numeric conversion failed near field '" + f.join("|").toStdString() + "'");
}

// SensorInfo  →  "id|name|type|temperature|humidity|pressure|signal|active|x|y|z"
QString DataManager::sensorToLine(const SensorInfo& s) const {
    return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
        .arg(s.id)
        .arg(s.name)
        .arg(s.type)
        .arg(s.temperature,    0, 'f', 4)
        .arg(s.humidity,       0, 'f', 4)
        .arg(s.pressure,       0, 'f', 4)
        .arg(s.signalStrength, 0, 'f', 4)
        .arg(s.active ? 1 : 0)
        .arg(s.x, 0, 'f', 4)
        .arg(s.y, 0, 'f', 4)
        .arg(s.z, 0, 'f', 4);
}

// "id|name|type|temperature|humidity|pressure|signal|active|x|y|z"  →  SensorInfo
SensorInfo DataManager::lineToSensor(const QString& line, int lineNum) const {
    const QStringList f = line.split(kFieldSep);
    if (f.size() != 11) {
        throw FileParseException(
            "sensors.dat line " + std::to_string(lineNum) +
            ": expected 11 fields, got " + std::to_string(f.size()));
    }

    bool ok = true;
    SensorInfo s;
    s.id             = f[0].toInt(&ok);        if (!ok) goto bad;
    s.name           = f[1];
    s.type           = f[2];
    s.temperature    = f[3].toFloat(&ok);      if (!ok) goto bad;
    s.humidity       = f[4].toFloat(&ok);      if (!ok) goto bad;
    s.pressure       = f[5].toFloat(&ok);      if (!ok) goto bad;
    s.signalStrength = f[6].toFloat(&ok);      if (!ok) goto bad;
    s.active         = (f[7].toInt(&ok) != 0); if (!ok) goto bad;
    s.x              = f[8].toFloat(&ok);      if (!ok) goto bad;
    s.y              = f[9].toFloat(&ok);      if (!ok) goto bad;
    s.z              = f[10].toFloat(&ok);     if (!ok) goto bad;
    return s;

bad:
    throw FileParseException(
        "sensors.dat line " + std::to_string(lineNum) +
        ": numeric conversion failed near field '" + f.join("|").toStdString() + "'");
}

// ============================================================
//  saveDevices
// ============================================================
void DataManager::saveDevices(const QVector<DeviceInfo>& devices) {
    ensureDataDir();

    QFile file(devicesPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        throw FileOpenException(devicesPath().toStdString());
    }

    QTextStream out(&file);
    out << kDeviceHeader << "\n";   // version line

    for (const auto& d : devices) {
        out << deviceToLine(d) << "\n";
    }

    // Flush and verify
    out.flush();
    if (file.error() != QFile::NoError) {
        throw FileWriteException(devicesPath().toStdString());
    }

    file.close();
    log(QString("Saved %1 device(s) to devices.dat").arg(devices.size()), "INFO");
}

// ============================================================
//  loadDevices
// ============================================================
QVector<DeviceInfo> DataManager::loadDevices() {
    QVector<DeviceInfo> result;
    const QString path = devicesPath();

    // First-run: file doesn't exist yet — return empty list, not an error
    if (!QFileInfo::exists(path)) {
        return result;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw FileOpenException(path.toStdString());
    }

    QTextStream in(&file);
    int lineNum = 0;

    // Validate magic header
    const QString header = in.readLine().trimmed();
    lineNum++;
    if (header != kDeviceHeader) {
        throw FileParseException(
            "devices.dat: unrecognised header '" + header.toStdString() +
            "' (expected " + kDeviceHeader + ")");
    }

    // Parse data lines
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        lineNum++;
        if (line.isEmpty()) continue;   // skip blank lines
        result << lineToDevice(line, lineNum);
    }

    file.close();
    log(QString("Loaded %1 device(s) from devices.dat").arg(result.size()), "INFO");
    return result;
}

// ============================================================
//  saveSensors
// ============================================================
void DataManager::saveSensors(const QVector<SensorInfo>& sensors) {
    ensureDataDir();

    QFile file(sensorsPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        throw FileOpenException(sensorsPath().toStdString());
    }

    QTextStream out(&file);
    out << kSensorHeader << "\n";

    for (const auto& s : sensors) {
        out << sensorToLine(s) << "\n";
    }

    out.flush();
    if (file.error() != QFile::NoError) {
        throw FileWriteException(sensorsPath().toStdString());
    }

    file.close();
    log(QString("Saved %1 sensor(s) to sensors.dat").arg(sensors.size()), "INFO");
}

// ============================================================
//  loadSensors
// ============================================================
QVector<SensorInfo> DataManager::loadSensors() {
    QVector<SensorInfo> result;
    const QString path = sensorsPath();

    if (!QFileInfo::exists(path)) {
        return result;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw FileOpenException(path.toStdString());
    }

    QTextStream in(&file);
    int lineNum = 0;

    const QString header = in.readLine().trimmed();
    lineNum++;
    if (header != kSensorHeader) {
        throw FileParseException(
            "sensors.dat: unrecognised header '" + header.toStdString() +
            "' (expected " + kSensorHeader + ")");
    }

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        lineNum++;
        if (line.isEmpty()) continue;
        result << lineToSensor(line, lineNum);
    }

    file.close();
    log(QString("Loaded %1 sensor(s) from sensors.dat").arg(result.size()), "INFO");
    return result;
}

// ============================================================
//  log  —  append one timestamped line to simulation_logs.txt
// ============================================================
void DataManager::log(const QString& message, const QString& level) {
    // Log failures must NEVER crash the simulation — catch everything.
    try {
        ensureDataDir();

        QFile file(logPath());
        if (!file.open(QIODevice::Append | QIODevice::Text)) return;

        QTextStream out(&file);
        const QString timestamp =
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        // Pad level to 7 chars for neat column alignment:  [INFO   ]
        const QString paddedLevel = QString("[%1]").arg(level, -7);

        out << "[" << timestamp << "] " << paddedLevel << " " << message << "\n";
        file.close();
    } catch (...) {
        // silently swallow — log errors must not interrupt simulation
    }
}

// ============================================================
//  logSessionStart  —  write a visible separator line
// ============================================================
void DataManager::logSessionStart() {
    try {
        ensureDataDir();

        QFile file(logPath());
        if (!file.open(QIODevice::Append | QIODevice::Text)) return;

        QTextStream out(&file);
        const QString timestamp =
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        out << "\n"
            << "============================================================\n"
            << "  SESSION STARTED  " << timestamp << "\n"
            << "============================================================\n";
        file.close();
    } catch (...) {}
}

// ============================================================
//  readLog  —  return all lines from simulation_logs.txt
// ============================================================
QStringList DataManager::readLog() {
    const QString path = logPath();
    if (!QFileInfo::exists(path)) return {};

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw FileOpenException(path.toStdString());
    }

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.close();
    return lines;
}

// ============================================================
//  clearLog
// ============================================================
void DataManager::clearLog() {
    QFile file(logPath());
    if (file.exists()) file.remove();
}
