#ifndef SIMULATIONCONTROLWINDOW_H
#define SIMULATIONCONTROLWINDOW_H

// ============================================================
//  SimulationControlWindow.h
//  Gives the user full control over the simulation engine:
//  start, pause, reset, and adjust the simulation speed.
//  Also shows a scrolling event log and live aggregate stats.
// ============================================================

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include "UIController.h"
#include "DroneControlPanel.h"

class SimulationControlWindow : public QWidget {
    Q_OBJECT

public:
    explicit SimulationControlWindow(UIController* ctrl, QWidget* parent = nullptr);

public slots:
    // Receives the aggregate stats broadcast every simulation tick.
    void onSimTickUpdated(const SimStats& stats);

    // Receives individual log messages from any part of the system.
    void onLogEvent(const QString& msg, const QString& level);

    // Called when the simulation starts or stops.
    void onSimStateChanged(bool running);

    // Called when a drone is added or removed — keeps tab widget in sync.
    void addDroneTab(const DeviceInfo& device);
    void removeDroneTab(int deviceId);

signals:
    // Not used internally yet — placeholder for a future scheduler feature.
    void scheduleEvent(const QString& description, int atTick);
    void resetRequested();
private slots:
    void onStartClicked();    // "Start" button pressed
    void onPauseClicked();    // "Pause" button pressed
    void onResetClicked();    // "Reset" button pressed
    void onSpeedChanged(int value);  // speed multiplier slider moved

private:
    UIController* controller;   // shared; not owned

    // ── Status display ────────────────────────────────────────
    QLabel*      simStatusLabel;   // "Running" / "Paused"
    QLabel*      simTimeLabel;     // elapsed simulation time mm:ss

    // ── Stat readouts ─────────────────────────────────────────
    QLabel*      valActiveDevices;
    QLabel*      valActiveSensors;
    QLabel*      valAvgBattery;
    QLabel*      valAlerts;
    QLabel*      valSignal;

    // ── Control buttons ───────────────────────────────────────
    QPushButton* btnStart;
    QPushButton* btnPause;
    QPushButton* btnReset;

    // ── Speed slider ──────────────────────────────────────────
    QSlider*     speedSlider;
    QLabel*      speedLabel;       // shows "1×", "2×" etc.

    // ── Event log ─────────────────────────────────────────────
    QListWidget* eventLog;

    // ── Drone Control Panel (embedded, one per drone) ─────────
    QTabWidget*  droneTabWidget = nullptr;
    QVBoxLayout* innerLayout    = nullptr;  // root layout of the inner scroll widget

    bool isRunning = false;

    void setupUi();
    void applyStyles();
    void updateButtonStates();
    QString formatTime(int ticks);

    // Helper: adds one timestamped entry to the event log.
    void appendLog(const QString& msg, const QString& level);
};

#endif // SIMULATIONCONTROLWINDOW_H
