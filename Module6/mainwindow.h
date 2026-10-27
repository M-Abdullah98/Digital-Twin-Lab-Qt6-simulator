#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QRandomGenerator>
#include <QStackedWidget>
#include <QTabWidget>
#include <QMap>
#include <QMessageBox>

#include "../Module5/Source/SimulationExceptions.h"
#include "UIController.h"
#include "SimulationControlWindow.h"

// ── StatCard ──────────────────────────────────────────────────
class StatCard : public QFrame
{
    Q_OBJECT
public:
    explicit StatCard(const QString &title, const QString &value,
                      const QString &unit, const QString &accent,
                      QWidget *parent = nullptr);
    void setValue(const QString &val);
    void setSubtext(const QString &sub);
    void pulse();

private:
    QLabel *valueLabel;
    QLabel *subtextLabel;
    QString accentColor;
};

// ── DashDeviceRow ─────────────────────────────────────────────
class DashDeviceRow : public QFrame
{
    Q_OBJECT
public:
    explicit DashDeviceRow(const QString &name, const QString &type,
                           int battery, bool active, QWidget *parent = nullptr);
    void updateBattery(int pct);
    void setActive(bool active);
    bool active() const { return isActive; }
    QString name() const { return nameLabel ? nameLabel->text() : QString(); }
    int deviceId() const { return m_deviceId; }
    void setDeviceId(int id) { m_deviceId = id; }
signals:
    void removeRequested(DashDeviceRow *self);

private:
    QLabel *statusDot;
    QLabel *nameLabel;
    QLabel *typeLabel;
    QProgressBar *batteryBar;
    QLabel *batteryLabel;
    QPushButton *removeBtn;
    bool isActive;
    int m_deviceId = -1;
};

// ── SensorRow ─────────────────────────────────────────────────
class SensorRow : public QFrame
{
    Q_OBJECT
public:
    explicit SensorRow(const QString &name, const QString &type,
                       QWidget *parent = nullptr);
    QString sensorName() const;
    QString sensorType() const;
    int sensorId() const { return m_sensorId; }
    void setSensorId(int id) { m_sensorId = id; }
signals:
    void removeRequested(SensorRow *self);

private:
    QLabel *nameLabel;
    QLabel *typeLabel;
    int m_sensorId = -1;
};

// ── LogEntry / LogPanel ───────────────────────────────────────
class LogEntry : public QFrame
{
    Q_OBJECT
public:
    explicit LogEntry(const QString &text, const QString &level,
                      QWidget *parent = nullptr);
};

class LogPanel : public QFrame
{
    Q_OBJECT
public:
    explicit LogPanel(const QString &title, bool showClearBtn,
                      QWidget *parent = nullptr);
    void addEntry(const QString &text, const QString &level);
    void clear();

private:
    QVBoxLayout *listLayout = nullptr;
    int maxEntries = 120;
};

// ── MainWindow ────────────────────────────────────────────────
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onSimTickUpdated(const SimStats &stats);
    void onDeviceUpdated(const DeviceInfo &device);
    void onSensorUpdated(const SensorInfo &sensor);
    void onLogEvent(const QString &msg, const QString &level);
    void onSimulationReset();

private:
    UIController *controller = nullptr;
    SimulationControlWindow *simPage = nullptr;

    // Stat cards
    StatCard *cardActiveDevices = nullptr;
    StatCard *cardActiveSensors = nullptr;
    StatCard *cardAvgBattery = nullptr;
    StatCard *cardAlerts = nullptr;
    StatCard *cardSimTime = nullptr;
    StatCard *cardSignal = nullptr;

    // Dashboard live device panel (grouped subheadings)
    QVBoxLayout *dashDroneLayout = nullptr;
    QVBoxLayout *dashSensorLayout = nullptr;
    QMap<int, QWidget *> dashSensorWidgets; // sensorId → widget in dashboard
    QLabel *countBadge = nullptr;
    QLabel *droneCountLabel = nullptr;
    QLabel *sensorCountLabel = nullptr;

    // Drones page
    QWidget *sharedDeviceWidget = nullptr;
    QVBoxLayout *deviceListLayout = nullptr;

    QVector<DashDeviceRow *> deviceRows;    // canonical list (Drones page)
    QVector<DashDeviceRow *> dashDroneRows; // dashboard mirror rows (display only)

    // Sensors page
    QVBoxLayout *sensorListLayout = nullptr;
    QVector<SensorRow *> sensorRows;

    // Telemetry sensor readouts
    QVBoxLayout *teleSensorLayout = nullptr;
    QVBoxLayout *teleDroneLayout = nullptr;

    // Navigation
    QStackedWidget *mainStack = nullptr;
    QVector<QPushButton *> navButtons;

    // Log panels
    LogPanel *dashLog = nullptr;
    LogPanel *logDevices = nullptr;
    LogPanel *logSensors = nullptr;
    LogPanel *logBattery = nullptr;
    LogPanel *logClimate = nullptr;
    LogPanel *logAlerts = nullptr;

    // Sim state
    int simTick = 0;
    int activeDevices = 0;
    int activeSensors = 0;
    int alertCount = 0;

    void setupUi();
    void applyStyles();
    void buildSidebar(QWidget *parent);
    void buildMainArea(QWidget *parent);
    QWidget *buildStatGrid();
    QWidget *buildDashDevicePanel();
    QWidget *buildDevicePanel();
    QWidget *buildSensorsPanel();
    QWidget *buildTelemetryPage();
    QWidget *buildAlertsPage();

    void addDevice(const QString &name, const QString &type,
                   int battery = 100, bool active = true, int id = -1);
    void removeDevice(DashDeviceRow *row);
    void addSensor(const QString &name, const QString &type, int id = -1);
    void removeSensor(SensorRow *row);
    void updateDashboardCounts();
    void addLog(const QString &msg, const QString &level = "INFO",
                const QString &category = "devices");
    QString formatTime(int ticks);
};

#endif // MAINWINDOW_H
