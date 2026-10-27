#ifndef DEVICEDASHBOARD_H
#define DEVICEDASHBOARD_H

// ============================================================
//  DeviceDashboard.h
//  Shows a scrollable list of all registered devices.
//  The user can add, remove, and inspect each device.
//  Sits inside the MainWindow's stacked-page for "Devices".
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QProgressBar>
#include <QTimer>
#include "UIController.h"

// ── One row in the device list ────────────────────────────────
// Displays name, type, battery bar, status dot, and action buttons.
class DeviceRow : public QFrame {
    Q_OBJECT
public:
    explicit DeviceRow(const DeviceInfo& info, QWidget* parent = nullptr);

    // Call this every tick to refresh the battery level and status.
    void refresh(const DeviceInfo& info);

    int  deviceId()   const { return id; }
    bool isActive()   const { return active; }
    QString deviceName() const;

signals:
    // Emitted when the user clicks "Open" on this row.
    void openRequested(int deviceId);

private:
    int          id;
    bool         active;
    QLabel*      statusDot;
    QLabel*      nameLabel;
    QLabel*      typeLabel;
    QProgressBar* batteryBar;
    QLabel*      batteryLabel;
    QPushButton* openButton;

    void applyBatteryColor(int pct);
};

// ── DeviceDashboard page ──────────────────────────────────────
class DeviceDashboard : public QWidget {
    Q_OBJECT

public:
    // Pass the shared controller so the panel can call commands.
    explicit DeviceDashboard(UIController* ctrl, QWidget* parent = nullptr);

public slots:
    // Connected to UIController::deviceUpdated — refreshes one row.
    void onDeviceUpdated(const DeviceInfo& device);

    // Connected to UIController::simTickUpdated — refreshes the count badge.
    void onSimTickUpdated(const SimStats& stats);

signals:
    // Tell the parent window to open the Drone Control Panel for this drone.
    void openDronePanel(int droneId);

    // Forward log messages up to MainWindow.
    void logEvent(const QString& msg, const QString& level);

private slots:
    void onAddDevice();      // "+ Add" button clicked
    void onRemoveDevice();   // "- Remove" button clicked
    void onOpenDevice(int deviceId);  // "Open" button on a row clicked

private:
    UIController*        controller;    // shared; not owned
    QVBoxLayout*         listLayout;    // layout inside the scroll area
    QVector<DeviceRow*>  rows;          // one entry per device
    QLabel*              countBadge;    // green pill showing active count

    void setupUi();
    void applyStyles();
    void populateList();    // builds initial rows from controller data
};

#endif // DEVICEDASHBOARD_H