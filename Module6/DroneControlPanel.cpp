#include "DroneControlPanel.h"
#include <QDateTime>
#include <QFrame>
#include <QMessageBox>

// ============================================================
//  Constructor
// ============================================================
DroneControlPanel::DroneControlPanel(UIController* ctrl, int id, QWidget* parent)
    : QWidget(parent), controller(ctrl), droneId(id)
{
    setWindowTitle(QString("Drone Control — %1").arg(id));
    setMinimumSize(480, 440);
    resize(620, 540);

    currentState  = controller->getDevice(droneId);
    isSurveillance = currentState.type.contains("Surveillance", Qt::CaseInsensitive);

    setupUi();
    applyStyles();
    refreshDisplay();

    connect(controller, &UIController::deviceUpdated,
            this, &DroneControlPanel::onDeviceUpdated);
}

// ============================================================
//  UI Construction
// ============================================================
void DroneControlPanel::setupUi() {
    // Outer layout: fills the widget with just a scroll area
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("panelScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    outerLayout->addWidget(scrollArea);

    // Inner content widget that holds all the panel controls
    QWidget* content = new QWidget();
    scrollArea->setWidget(content);

    QVBoxLayout* root = new QVBoxLayout(content);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    // ── Page header ──────────────────────────────────────────
    QLabel* pageTitle = new QLabel("Drone Control Panel", this);
    pageTitle->setObjectName("pageTitle");

    QString subtitle = QString("Controlling: %1  [%2]")
        .arg(currentState.name.isEmpty()
             ? QString("Drone #%1").arg(droneId)
             : currentState.name)
        .arg(currentState.type);
    QLabel* pageSub = new QLabel(subtitle, this);
    pageSub->setObjectName("pageSubtitle");

    root->addWidget(pageTitle);
    root->addWidget(pageSub);

    // ── Status badge ─────────────────────────────────────────
    statusBadge = new QLabel("○ STANDBY", this);
    statusBadge->setObjectName("statusBadgeStandby");
    statusBadge->setFixedHeight(36);
    statusBadge->setAlignment(Qt::AlignCenter);
    root->addWidget(statusBadge);

    // ── Battery display ──────────────────────────────────────
    QHBoxLayout* batRow = new QHBoxLayout();
    QLabel* batLbl = new QLabel("Battery:", this);
    batLbl->setObjectName("readoutLabel");
    batteryVal = new QLabel("--", this);
    batteryVal->setObjectName("readoutValue");
    QLabel* batUnit = new QLabel("%", this);
    batUnit->setObjectName("readoutUnit");
    batRow->addWidget(batLbl);
    batRow->addSpacing(8);
    batRow->addWidget(batteryVal);
    batRow->addWidget(batUnit);
    batRow->addStretch();
    root->addLayout(batRow);

    batteryBar = new QProgressBar(this);
    batteryBar->setObjectName("batteryBar");
    batteryBar->setRange(0, 100);
    batteryBar->setFixedHeight(8);
    batteryBar->setTextVisible(false);
    root->addWidget(batteryBar);

    // ── Divider ──────────────────────────────────────────────
    QFrame* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("sep");
    root->addWidget(sep);

    // ── Flight Controls ──────────────────────────────────────
    QLabel* flightLabel = new QLabel("FLIGHT CONTROLS", this);
    flightLabel->setObjectName("sectionLabel");
    root->addWidget(flightLabel);

    QHBoxLayout* flightRow = new QHBoxLayout();
    flightRow->setSpacing(12);

    btnTakeoff   = new QPushButton("▲  TAKE OFF",        this);
    btnLand      = new QPushButton("▼  LAND",            this);
    btnEmergency = new QPushButton("⬛  EMERGENCY STOP",  this);

    btnTakeoff->setObjectName("btnTakeoff");
    btnLand->setObjectName("btnLand");
    btnEmergency->setObjectName("btnEmergency");

    for (auto* b : {btnTakeoff, btnLand, btnEmergency}) {
        b->setFixedHeight(56);
        b->setCursor(Qt::PointingHandCursor);
        flightRow->addWidget(b);
    }
    root->addLayout(flightRow);

    connect(btnTakeoff,   &QPushButton::clicked, this, &DroneControlPanel::onTakeoff);
    connect(btnLand,      &QPushButton::clicked, this, &DroneControlPanel::onLand);
    connect(btnEmergency, &QPushButton::clicked, this, &DroneControlPanel::onEmergencyStop);

    // ── Battery Management ───────────────────────────────────
    QFrame* sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName("sep");
    root->addWidget(sep2);

    QLabel* batMgmtLabel = new QLabel("BATTERY MANAGEMENT", this);
    batMgmtLabel->setObjectName("sectionLabel");
    root->addWidget(batMgmtLabel);

    QHBoxLayout* batBtnRow = new QHBoxLayout();
    batBtnRow->setSpacing(12);

    btnCharge    = new QPushButton("⚡  CHARGE",    this);
    btnDischarge = new QPushButton("🔋  DISCHARGE", this);

    btnCharge->setObjectName("btnCharge");
    btnDischarge->setObjectName("btnDischarge");

    for (auto* b : {btnCharge, btnDischarge}) {
        b->setFixedHeight(48);
        b->setCursor(Qt::PointingHandCursor);
        batBtnRow->addWidget(b);
    }
    root->addLayout(batBtnRow);

    connect(btnCharge,    &QPushButton::clicked, this, &DroneControlPanel::onCharge);
    connect(btnDischarge, &QPushButton::clicked, this, &DroneControlPanel::onDischarge);

    // ── Surveillance-only: Camera Toggle ─────────────────────
    if (isSurveillance) {
        QFrame* sep3 = new QFrame(this);
        sep3->setFrameShape(QFrame::HLine);
        sep3->setObjectName("sep");
        root->addWidget(sep3);

        QLabel* camLabel = new QLabel("SURVEILLANCE", this);
        camLabel->setObjectName("sectionLabel");
        root->addWidget(camLabel);

        btnCamera = new QPushButton("📷  CAMERA OFF", this);
        btnCamera->setObjectName("btnCameraOff");
        btnCamera->setFixedHeight(48);
        btnCamera->setCursor(Qt::PointingHandCursor);

        QHBoxLayout* camRow = new QHBoxLayout();
        camRow->addWidget(btnCamera);
        camRow->addStretch();
        root->addLayout(camRow);

        connect(btnCamera, &QPushButton::clicked,
                this, &DroneControlPanel::onCameraToggle);
    }

    // ── Command log ──────────────────────────────────────────
    QFrame* sep4 = new QFrame(this);
    sep4->setFrameShape(QFrame::HLine);
    sep4->setObjectName("sep");
    root->addWidget(sep4);

    QLabel* logLabel = new QLabel("COMMAND LOG", this);
    logLabel->setObjectName("sectionLabel");
    root->addWidget(logLabel);

    commandLog = new QListWidget(this);
    commandLog->setObjectName("commandLog");
    commandLog->setFrameShape(QFrame::NoFrame);
    root->addWidget(commandLog, 1);
}

// ============================================================
//  Slots — Flight
// ============================================================
void DroneControlPanel::onTakeoff() {
    // Guard: no drone loaded
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone",
            "There is no drone assigned to this panel.\n"
            "Please add a drone first.");
        addCommandLog("TAKEOFF refused — no drone assigned", "WARN");
        return;
    }
    if (currentState.battery <= 10) {
        addCommandLog("TAKEOFF refused — battery too low", "WARN");
        return;
    }
    controller->commandTakeoff(droneId);
    addCommandLog("TAKEOFF command issued");
}

void DroneControlPanel::onLand() {
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone",
            "There is no drone assigned to this panel.\n"
            "Please add a drone first.");
        addCommandLog("LAND refused — no drone assigned", "WARN");
        return;
    }
    controller->commandLand(droneId);
    addCommandLog("LAND command issued");
}

void DroneControlPanel::onEmergencyStop() {
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone",
            "There is no drone assigned to this panel.");
        return;
    }
    controller->commandEmergencyStop(droneId);
    addCommandLog("!!! EMERGENCY STOP !!!", "WARN");
}

// ============================================================
//  Slots — Battery Management
// ============================================================
void DroneControlPanel::onCharge() {
    // Exception: no drone registered for this panel
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone to Charge",
            "There is no drone assigned to this panel.\n"
            "Please add a drone before attempting to charge.");
        addCommandLog("CHARGE refused — no drone assigned", "WARN");
        return;
    }
    controller->commandCharge(droneId);
    addCommandLog("CHARGE command issued");
}

void DroneControlPanel::onDischarge() {
    // Exception: no drone registered for this panel
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone to Discharge",
            "There is no drone assigned to this panel.\n"
            "Please add a drone before attempting to discharge.");
        addCommandLog("DISCHARGE refused — no drone assigned", "WARN");
        return;
    }
    controller->commandDischarge(droneId);
    addCommandLog("DISCHARGE command issued");
}

// ============================================================
//  Slot — Camera Toggle (Surveillance only)
// ============================================================
void DroneControlPanel::onCameraToggle() {
    if (currentState.id == 0) {
        QMessageBox::warning(this, "No Drone",
            "There is no drone assigned to this panel.");
        return;
    }
    // Read current state BEFORE the toggle to log the new state correctly
    bool willBeOn = !currentState.cameraOn;
    controller->commandToggleCamera(droneId);
    addCommandLog(QString("CAMERA %1").arg(willBeOn ? "ON" : "OFF"));
}

// ============================================================
//  Device update
// ============================================================
void DroneControlPanel::onDeviceUpdated(const DeviceInfo& device) {
    if (device.id != droneId) return;
    currentState = device;
    refreshDisplay();
}

// ============================================================
//  Display helpers
// ============================================================
void DroneControlPanel::refreshDisplay() {
    batteryVal->setText(QString::number(currentState.battery));
    batteryBar->setValue(currentState.battery);
    updateStatusBadge();
    updateButtonStates();
    updateBatteryColor();

    // Update camera button label/style for surveillance drone
    if (btnCamera) {
        bool on = currentState.cameraOn;
        btnCamera->setText(on ? "📷  CAMERA ON" : "📷  CAMERA OFF");
        btnCamera->setObjectName(on ? "btnCameraOn" : "btnCameraOff");
        btnCamera->style()->unpolish(btnCamera);
        btnCamera->style()->polish(btnCamera);
    }
}

void DroneControlPanel::updateStatusBadge() {
    bool flying  = currentState.altitude > 0.1f;
    bool offline = currentState.battery <= 0;

    if (offline) {
        statusBadge->setText("✕ OFFLINE");
        statusBadge->setObjectName("statusBadgeOffline");
    } else if (flying) {
        statusBadge->setText("● FLYING");
        statusBadge->setObjectName("statusBadgeFlying");
    } else {
        statusBadge->setText("○ STANDBY");
        statusBadge->setObjectName("statusBadgeStandby");
    }
    statusBadge->style()->unpolish(statusBadge);
    statusBadge->style()->polish(statusBadge);
}

void DroneControlPanel::updateButtonStates() {
    bool flying   = currentState.altitude > 0.1f;
    bool haspower = currentState.battery > 10;
    bool hasDrone = currentState.id != 0;

    btnTakeoff->setEnabled(hasDrone && !flying && haspower);
    btnLand->setEnabled(hasDrone && flying);
    btnEmergency->setEnabled(hasDrone && (flying || currentState.active));

    // Charge: available when drone exists and not fully charged
    btnCharge->setEnabled(hasDrone && currentState.battery < 100);
    // Discharge: available when drone exists and has charge remaining
    btnDischarge->setEnabled(hasDrone && currentState.battery > 0);
}

void DroneControlPanel::updateBatteryColor() {
    int pct = currentState.battery;
    QString color = pct > 50 ? "#3fb950" : pct > 20 ? "#d29922" : "#f85149";
    batteryBar->setStyleSheet(
        QString("QProgressBar{background:#21262d;border-radius:4px;border:none;}"
                "QProgressBar::chunk{background:%1;border-radius:4px;}").arg(color));
    batteryVal->setStyleSheet(
        QString("color:%1;font-size:14px;font-weight:700;").arg(color));
}

void DroneControlPanel::addCommandLog(const QString& msg, const QString& type) {
    QString time   = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix = (type == "WARN") ? "⚠" : "▶";
    auto* item     = new QListWidgetItem(
        QString("[%1] %2 %3").arg(time, prefix, msg));
    item->setForeground(type == "WARN" ? QColor("#d29922") : QColor("#3fb950"));
    commandLog->insertItem(0, item);
    while (commandLog->count() > 50)
        delete commandLog->takeItem(commandLog->count() - 1);
    emit logEvent(msg, type == "WARN" ? "WARN" : "INFO");
}

// ============================================================
//  Styles
// ============================================================
void DroneControlPanel::applyStyles() {
    setStyleSheet(R"(
        QWidget { background:#0d1117; color:#e6edf3;
                  font-family:"Segoe UI",sans-serif; font-size:13px; }

        QScrollArea#panelScrollArea { background:#0d1117; border:none; }
        QScrollArea#panelScrollArea > QWidget > QWidget { background:#0d1117; }

        QScrollBar:vertical { background:#0d1117; width:6px; border:none; }
        QScrollBar::handle:vertical { background:#30363d; border-radius:3px; min-height:20px; }
        QScrollBar::handle:vertical:hover { background:#484f58; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }

        QLabel#pageTitle    { font-size:22px; font-weight:700; color:#e6edf3; }
        QLabel#pageSubtitle { color:#7d8590; font-size:13px; }
        QLabel#sectionLabel { color:#484f58; font-size:10px; font-weight:700;
                              letter-spacing:1px; }
        QLabel#readoutLabel { color:#7d8590; font-size:13px; }
        QLabel#readoutUnit  { color:#484f58; font-size:12px; }

        QFrame#sep { color:#21262d; background:#21262d; max-height:1px; }

        QLabel#statusBadgeStandby {
            background:#1c2128; color:#7d8590;
            border:1px solid #30363d; border-radius:8px;
            font-size:13px; font-weight:600; }
        QLabel#statusBadgeFlying {
            background:#0d2a0d; color:#3fb950;
            border:1px solid #238636; border-radius:8px;
            font-size:13px; font-weight:600; }
        QLabel#statusBadgeOffline {
            background:#2d0d0d; color:#f85149;
            border:1px solid #6e2020; border-radius:8px;
            font-size:13px; font-weight:600; }

        QPushButton#btnTakeoff {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #238636,stop:1 #196127);
            color:#fff; border:none; border-radius:10px;
            font-size:15px; font-weight:700; }
        QPushButton#btnTakeoff:hover    { background:#2ea043; }
        QPushButton#btnTakeoff:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnLand {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #1f6feb,stop:1 #155ab2);
            color:#fff; border:none; border-radius:10px;
            font-size:15px; font-weight:700; }
        QPushButton#btnLand:hover    { background:#388bfd; }
        QPushButton#btnLand:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnEmergency {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #b91c1c,stop:1 #7f1d1d);
            color:#fff; border:none; border-radius:10px;
            font-size:15px; font-weight:700; }
        QPushButton#btnEmergency:hover    { background:#dc2626; }
        QPushButton#btnEmergency:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnCharge {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #9a3fcf,stop:1 #6e2d99);
            color:#fff; border:none; border-radius:10px;
            font-size:14px; font-weight:700; }
        QPushButton#btnCharge:hover    { background:#b34de6; }
        QPushButton#btnCharge:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnDischarge {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #d29922,stop:1 #9a6e0a);
            color:#fff; border:none; border-radius:10px;
            font-size:14px; font-weight:700; }
        QPushButton#btnDischarge:hover    { background:#e3b341; }
        QPushButton#btnDischarge:disabled { background:#21262d; color:#484f58; }

        QPushButton#btnCameraOff {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #30363d,stop:1 #21262d);
            color:#7d8590; border:1px solid #30363d; border-radius:10px;
            font-size:14px; font-weight:700; min-width:180px; }
        QPushButton#btnCameraOff:hover { background:#3d444d; color:#e6edf3; }

        QPushButton#btnCameraOn {
            background:qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #0d7a5f,stop:1 #0a5a46);
            color:#fff; border:1px solid #0d7a5f; border-radius:10px;
            font-size:14px; font-weight:700; min-width:180px; }
        QPushButton#btnCameraOn:hover { background:#0ea47e; }

        QProgressBar#batteryBar { background:#21262d; border-radius:4px; border:none; }

        QListWidget#commandLog {
            background:#161b22; border:1px solid #21262d; border-radius:8px;
            font-family:"Cascadia Code","Consolas",monospace; font-size:11px; }
        QListWidget#commandLog::item { padding:4px 16px; }
        QListWidget#commandLog::item:selected { background:#1c2128; }
    )");
}

