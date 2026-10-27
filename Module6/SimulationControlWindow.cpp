#include "SimulationControlWindow.h"
#include <QDateTime>
#include <QMessageBox>
#include <QScrollArea>

// ============================================================
//  Constructor
// ============================================================
SimulationControlWindow::SimulationControlWindow(UIController* ctrl, QWidget* parent)
    : QWidget(parent), controller(ctrl)
{
    setupUi();
    applyStyles();
    updateButtonStates();

    // Subscribe to controller signals.
    connect(controller, &UIController::simTickUpdated,
            this, &SimulationControlWindow::onSimTickUpdated);
    connect(controller, &UIController::logEvent,
            this, &SimulationControlWindow::onLogEvent);
    connect(controller, &UIController::simulationStateChanged,
            this, &SimulationControlWindow::onSimStateChanged);
}

// ============================================================
//  UI Construction
// ============================================================
void SimulationControlWindow::setupUi() {
    // ── Outer layout: just holds the scroll area ──────────────
    QVBoxLayout* outerLay = new QVBoxLayout(this);
    outerLay->setContentsMargins(0, 0, 0, 0);
    outerLay->setSpacing(0);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setObjectName("simScrollArea");

    QWidget* inner = new QWidget();
    inner->setObjectName("simInnerWidget");
    QVBoxLayout* root = new QVBoxLayout(inner);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(24);
    innerLayout = root;  // save for dynamic tab insertion

    scroll->setWidget(inner);
    outerLay->addWidget(scroll);

    // ── Page header ───────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout();
    QVBoxLayout* titleCol = new QVBoxLayout();
    QLabel* title = new QLabel("Simulation Control", inner);
    title->setObjectName("pageTitle");
    QLabel* sub = new QLabel("Start, pause, and configure the simulation engine", inner);
    sub->setObjectName("pageSubtitle");
    titleCol->addWidget(title);
    titleCol->addWidget(sub);
    titleCol->setSpacing(2);

    // Big status pill in the top-right corner.
    simStatusLabel = new QLabel("● PAUSED", inner);
    simStatusLabel->setObjectName("simStatusPaused");
    simStatusLabel->setFixedHeight(36);
    simStatusLabel->setMinimumWidth(100);
    simStatusLabel->setAlignment(Qt::AlignCenter);

    header->addLayout(titleCol);
    header->addStretch();
    header->addWidget(simStatusLabel);
    root->addLayout(header);

    // ── Stat cards row ────────────────────────────────────────
    QWidget* statsRow = new QWidget(inner);
    statsRow->setObjectName("statsRow");
    QHBoxLayout* statsLay = new QHBoxLayout(statsRow);
    statsLay->setContentsMargins(0, 0, 0, 0);
    statsLay->setSpacing(12);

    // Helper lambda: creates a small stat card.
    auto makeStatCard = [&](const QString& label, QLabel*& valPtr,
                            const QString& color) {
        QWidget* card = new QWidget(statsRow);
        card->setObjectName("miniCard");
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(14, 10, 14, 10);
        cl->setSpacing(4);

        // Coloured accent bar at the top.
        QFrame* bar = new QFrame(card);
        bar->setFixedHeight(3);
        bar->setStyleSheet(QString("background:%1; border-radius:2px;").arg(color));
        cl->addWidget(bar);

        QLabel* lbl = new QLabel(label, card);
        lbl->setObjectName("cardTitle");
        valPtr = new QLabel("--", card);
        valPtr->setObjectName("cardValue");
        cl->addWidget(lbl);
        cl->addWidget(valPtr);
        statsLay->addWidget(card, 1);
    };

    makeStatCard("Active Devices",  valActiveDevices, "#3fb950");
    makeStatCard("Active Sensors",  valActiveSensors, "#388bfd");
    makeStatCard("Avg Battery (%)", valAvgBattery,    "#d29922");
    makeStatCard("Alerts",          valAlerts,         "#f85149");
    makeStatCard("Signal (%)",      valSignal,         "#bc8cff");

    root->addWidget(statsRow);

    // ── Control panel ─────────────────────────────────────────
    QWidget* ctrlPanel = new QWidget(inner);
    ctrlPanel->setObjectName("panel");
    QVBoxLayout* ctrlLay = new QVBoxLayout(ctrlPanel);
    ctrlLay->setContentsMargins(0, 0, 0, 0);
    ctrlLay->setSpacing(0);

    QWidget* ctrlHeader = new QWidget(ctrlPanel);
    ctrlHeader->setObjectName("panelHeader");
    ctrlHeader->setFixedHeight(48);
    QHBoxLayout* chLay = new QHBoxLayout(ctrlHeader);
    chLay->setContentsMargins(20, 0, 20, 0);
    QLabel* ctrlTitle = new QLabel("Engine Controls", ctrlHeader);
    ctrlTitle->setObjectName("panelTitle");

    // Simulation time displayed in the panel header.
    simTimeLabel = new QLabel("00:00", ctrlHeader);
    simTimeLabel->setObjectName("simTimeLabel");

    chLay->addWidget(ctrlTitle);
    chLay->addStretch();
    chLay->addWidget(simTimeLabel);
    ctrlLay->addWidget(ctrlHeader);

    // Buttons and speed slider inside the panel.
    QWidget* ctrlBody = new QWidget(ctrlPanel);
    ctrlBody->setFixedHeight(150);  // fixed height prevents expansion in fullscreen
    QVBoxLayout* bodyLay = new QVBoxLayout(ctrlBody);
    bodyLay->setContentsMargins(20, 16, 20, 16);
    bodyLay->setSpacing(10);

    // Start / Pause / Reset buttons in a row.
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    btnStart = new QPushButton("▷  Start Simulation",  ctrlBody);
    btnPause = new QPushButton("⏸  Pause Simulation",  ctrlBody);
    btnReset = new QPushButton("↺  Reset Simulation",  ctrlBody);

    btnStart->setObjectName("btnStart");
    btnPause->setObjectName("btnPause");
    btnReset->setObjectName("btnReset");

    for (auto* b : {btnStart, btnPause, btnReset}) {
        b->setFixedHeight(48);
        b->setCursor(Qt::PointingHandCursor);
        btnRow->addWidget(b);
    }

    connect(btnStart, &QPushButton::clicked, this, &SimulationControlWindow::onStartClicked);
    connect(btnPause, &QPushButton::clicked, this, &SimulationControlWindow::onPauseClicked);
    connect(btnReset, &QPushButton::clicked, this, &SimulationControlWindow::onResetClicked);
    bodyLay->addLayout(btnRow);

    // Speed slider (1× to 5×).
    QLabel* speedSectionLabel = new QLabel("SIMULATION SPEED", ctrlBody);
    speedSectionLabel->setObjectName("sectionLabel");
    bodyLay->addWidget(speedSectionLabel);

    QHBoxLayout* speedRow = new QHBoxLayout();
    QLabel* slowLbl = new QLabel("1×", ctrlBody);
    slowLbl->setObjectName("readoutLabel");
    speedSlider = new QSlider(Qt::Horizontal, ctrlBody);
    speedSlider->setRange(1, 5);
    speedSlider->setValue(1);
    speedSlider->setObjectName("paramSlider");
    speedLabel = new QLabel("1×", ctrlBody);
    speedLabel->setObjectName("sliderVal");
    speedLabel->setFixedWidth(28);

    connect(speedSlider, &QSlider::valueChanged,
            this, &SimulationControlWindow::onSpeedChanged);

    speedRow->addWidget(slowLbl);
    speedRow->addWidget(speedSlider, 1);
    speedRow->addWidget(speedLabel);
    bodyLay->addLayout(speedRow);

    ctrlLay->addWidget(ctrlBody);
    root->addWidget(ctrlPanel);

    // ── Event log ─────────────────────────────────────────────
    QWidget* logPanel = new QWidget(inner);
    logPanel->setObjectName("panel");
    QVBoxLayout* logLay = new QVBoxLayout(logPanel);
    logLay->setContentsMargins(0, 0, 0, 0);
    logLay->setSpacing(0);

    QWidget* logHeader = new QWidget(logPanel);
    logHeader->setObjectName("panelHeader");
    logHeader->setFixedHeight(48);
    QHBoxLayout* lhLay = new QHBoxLayout(logHeader);
    lhLay->setContentsMargins(20, 0, 20, 0);
    QLabel* logTitle = new QLabel("Event Log", logHeader);
    logTitle->setObjectName("panelTitle");
    QPushButton* clearBtn = new QPushButton("Clear", logHeader);
    clearBtn->setObjectName("clearBtn");
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, [this]{ eventLog->clear(); });
    lhLay->addWidget(logTitle);
    lhLay->addStretch();
    lhLay->addWidget(clearBtn);
    logLay->addWidget(logHeader);

    eventLog = new QListWidget(logPanel);
    eventLog->setObjectName("logList");
    eventLog->setFrameShape(QFrame::NoFrame);
    logLay->addWidget(eventLog);
    eventLog->setFixedHeight(180);

    root->addWidget(logPanel);

    // ── Drone Control Panels (one tab per drone) ───────────────
    QLabel* dtLabel = new QLabel("Drone Control Panels", inner);
    dtLabel->setObjectName("panelTitle");
    root->addWidget(dtLabel);

    droneTabWidget = new QTabWidget(inner);
    droneTabWidget->setObjectName("logTabs");
    for (const auto& d : controller->getAllDevices()) {
        auto* panel = new DroneControlPanel(controller, d.id, droneTabWidget);
        droneTabWidget->addTab(panel, QString("🚁  %1").arg(d.name));
    }
    root->addWidget(droneTabWidget);
    root->addStretch();
}

// ============================================================
//  Slot implementations
// ============================================================
void SimulationControlWindow::onStartClicked() {
    // Check if any devices exist — throw exception if none
    if (controller->getAllDevices().isEmpty()) {
        appendLog("ERROR: No drones found — add a drone before starting simulation", "ERROR");
        QMessageBox::critical(this,
            "Simulation Error",
            "No drones registered in the simulation.\n\n"
            "Please add at least one drone on the Drones page before starting.");
        return;
    }
    controller->startSimulation();
}

void SimulationControlWindow::onPauseClicked() {
    controller->pauseSimulation();
}

void SimulationControlWindow::onResetClicked() {
    controller->resetSimulation();
    simTimeLabel->setText("00:00");
    // Clear stat cards.
    for (auto* lbl : {valActiveDevices, valActiveSensors,
                      valAvgBattery, valAlerts, valSignal})
        lbl->setText("0");
        emit resetRequested();
}

void SimulationControlWindow::onSpeedChanged(int value) {
    speedLabel->setText(QString("%1×").arg(value));
    controller->setSimulationSpeed(value);
    appendLog(QString("Simulation speed set to %1×").arg(value), "INFO");
}

void SimulationControlWindow::onSimTickUpdated(const SimStats& stats) {
    // Refresh all stat readouts.
    simTimeLabel->setText(formatTime(stats.simTick));
    valActiveDevices->setText(QString::number(stats.activeDevices));
    valActiveSensors->setText(QString::number(stats.activeSensors));
    valAvgBattery->setText(QString::number(stats.avgBattery));
    valAlerts->setText(QString::number(stats.alertCount));
    valSignal->setText(QString::number(stats.signalStrength));
}

void SimulationControlWindow::onLogEvent(const QString& msg, const QString& level) {
    appendLog(msg, level);
}

void SimulationControlWindow::onSimStateChanged(bool running) {
    isRunning = running;
    simStatusLabel->setText(running ? "● RUNNING" : "● PAUSED");
    simStatusLabel->setObjectName(running ? "simStatusRunning" : "simStatusPaused");
    simStatusLabel->style()->unpolish(simStatusLabel);
    simStatusLabel->style()->polish(simStatusLabel);
    updateButtonStates();
}

// ============================================================
//  Helpers
// ============================================================
void SimulationControlWindow::updateButtonStates() {
    btnStart->setEnabled(!isRunning);
    btnPause->setEnabled(isRunning);
    // Reset is always available.
}

QString SimulationControlWindow::formatTime(int ticks) {
    return QString("%1:%2")
        .arg(ticks / 60, 2, 10, QChar('0'))
        .arg(ticks % 60, 2, 10, QChar('0'));
}

// ============================================================
//  Dynamic drone tab management
// ============================================================
void SimulationControlWindow::addDroneTab(const DeviceInfo& device) {
    if (!droneTabWidget) return;
    // Check not already present
    for (int i = 0; i < droneTabWidget->count(); ++i) {
        auto* p = qobject_cast<DroneControlPanel*>(droneTabWidget->widget(i));
        if (p && p->getDroneId() == device.id) return;
    }
    auto* panel = new DroneControlPanel(controller, device.id, droneTabWidget);
    droneTabWidget->addTab(panel, QString("🚁  %1").arg(device.name));
}

void SimulationControlWindow::removeDroneTab(int deviceId) {
    if (!droneTabWidget) return;
    for (int i = 0; i < droneTabWidget->count(); ++i) {
        auto* p = qobject_cast<DroneControlPanel*>(droneTabWidget->widget(i));
        if (p && p->getDroneId() == deviceId) {
            droneTabWidget->removeTab(i);
            p->deleteLater();
            return;
        }
    }
}

void SimulationControlWindow::appendLog(const QString& msg, const QString& level) {
    QString time   = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix = (level == "WARN") ? "⚠" : (level == "ERROR") ? "✕" : "●";
    auto* item     = new QListWidgetItem(
        QString("[%1] %2 %3").arg(time, prefix, msg));

    if      (level == "WARN")  item->setForeground(QColor("#d29922"));
    else if (level == "ERROR") item->setForeground(QColor("#f85149"));
    else                       item->setForeground(QColor("#7d8590"));

    eventLog->insertItem(0, item);
    while (eventLog->count() > 100)
        delete eventLog->takeItem(eventLog->count() - 1);
}

void SimulationControlWindow::applyStyles() {
    setStyleSheet(R"(
        QWidget { background:#0d1117; color:#e6edf3;
                  font-family:"Segoe UI",sans-serif; font-size:13px; }

        QLabel#pageTitle    { font-size:22px; font-weight:700; color:#e6edf3; }
        QLabel#pageSubtitle { color:#7d8590; font-size:13px; }
        QLabel#sectionLabel { color:#484f58; font-size:10px; font-weight:700;
                              letter-spacing:1px; }
        QLabel#readoutLabel { color:#7d8590; font-size:12px; }
        QLabel#sliderVal    { color:#388bfd; font-size:12px; }
        QLabel#simTimeLabel { color:#388bfd; font-size:18px; font-weight:700;
                              font-family:"Cascadia Code","Consolas",monospace; }

        /* Sim status pill */
        QLabel#simStatusRunning {
            background:#0d2a0d; color:#3fb950;
            border:1px solid #238636; border-radius:8px;
            font-size:12px; font-weight:700; padding:0 12px; }
        QLabel#simStatusPaused {
            background:#1c2128; color:#7d8590;
            border:1px solid #30363d; border-radius:8px;
            font-size:12px; font-weight:700; padding:0 12px; }

        /* Mini stat cards */
        QWidget#miniCard {
            background:#161b22; border:1px solid #21262d; border-radius:8px; }
        QLabel#cardTitle { color:#7d8590; font-size:11px; }
        QLabel#cardValue { color:#e6edf3; font-size:22px; font-weight:700; }

        /* Main panel */
        QWidget#panel { background:#161b22; border:1px solid #21262d; border-radius:10px; }
        QWidget#panelHeader {
            background:#1c2128;
            border-top-left-radius:10px; border-top-right-radius:10px;
            border-bottom:1px solid #21262d; }
        QLabel#panelTitle { font-size:14px; font-weight:600; color:#e6edf3; }

        /* Control buttons */
        QPushButton#btnStart {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #238636,stop:1 #196127);
            color:#fff; border:none; border-radius:8px;
            font-size:14px; font-weight:700; }
        QPushButton#btnStart:hover    { background:#2ea043; }
        QPushButton#btnStart:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnPause {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #1f6feb,stop:1 #155ab2);
            color:#fff; border:none; border-radius:8px;
            font-size:14px; font-weight:700; }
        QPushButton#btnPause:hover    { background:#388bfd; }
        QPushButton#btnPause:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnReset {
            background:#1c2128; color:#7d8590;
            border:1px solid #30363d; border-radius:8px;
            font-size:14px; font-weight:600; }
        QPushButton#btnReset:hover { color:#e6edf3; }

        QPushButton#clearBtn {
            background:#21262d; color:#7d8590;
            border:1px solid #30363d; border-radius:6px;
            padding:3px 10px; font-size:11px; }
        QPushButton#clearBtn:hover { color:#e6edf3; }

        /* Speed slider */
        QSlider#paramSlider::groove:horizontal {
            background:#21262d; height:6px; border-radius:3px; }
        QSlider#paramSlider::handle:horizontal {
            background:#388bfd; width:14px; height:14px;
            margin:-4px 0; border-radius:7px; }
        QSlider#paramSlider::sub-page:horizontal {
            background:#1f6feb; border-radius:3px; }

        /* Event log */
        QListWidget#logList {
            background:#0d1117; border:none;
            font-family:"Cascadia Code","Consolas",monospace; font-size:11px; }
        QListWidget#logList::item { padding:4px 16px; }
        QListWidget#logList::item:selected { background:#1c2128; }

        /* Scrollbars */
        QScrollBar:vertical { background:#161b22; width:6px; border:none; }
        QScrollBar::handle:vertical { background:#30363d; border-radius:3px; min-height:20px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
    )");
}
