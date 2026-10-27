#ifndef DRONECONTROLPANEL_H
#define DRONECONTROLPANEL_H

// ============================================================
//  DroneControlPanel.h
//  Control panel with Takeoff, Land, Emergency Stop,
//  Charge / Discharge (all drone types), and
//  Camera Toggle (Surveillance Drone only).
// ============================================================

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include "UIController.h"

class DroneControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit DroneControlPanel(UIController* ctrl,
                               int           droneId,
                               QWidget*      parent = nullptr);

    int getDroneId() const { return droneId; }

public slots:
    void onDeviceUpdated(const DeviceInfo& device);

signals:
    void logEvent(const QString& msg, const QString& level);

private slots:
    void onTakeoff();
    void onLand();
    void onEmergencyStop();
    void onCharge();
    void onDischarge();
    void onCameraToggle();

private:
    UIController* controller;
    int           droneId;
    DeviceInfo    currentState;
    bool          isSurveillance = false;

    // Telemetry display
    QLabel*       statusBadge;
    QLabel*       batteryVal;
    QProgressBar* batteryBar;

    // Flight buttons
    QPushButton*  btnTakeoff;
    QPushButton*  btnLand;
    QPushButton*  btnEmergency;

    // Battery management buttons (all drone types)
    QPushButton*  btnCharge;
    QPushButton*  btnDischarge;

    // Surveillance-only camera button (nullptr for Autonomous drones)
    QPushButton*  btnCamera = nullptr;

    // Command log
    QListWidget*  commandLog;

    void setupUi();
    void applyStyles();
    void refreshDisplay();
    void updateButtonStates();
    void updateStatusBadge();
    void updateBatteryColor();
    void addCommandLog(const QString& msg, const QString& type = "CMD");
};

#endif // DRONECONTROLPANEL_H
