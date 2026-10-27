#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QSizePolicy>
#include <QInputDialog>
#include <QTabWidget>

// ════════════════════════════════════════════════════════════
//  StatCard
// ════════════════════════════════════════════════════════════
StatCard::StatCard(const QString &title, const QString &value,
                   const QString &unit, const QString &accent, QWidget *parent)
    : QFrame(parent), accentColor(accent)
{
    setObjectName("statCard");
    setMinimumSize(160, 110);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(20, 16, 20, 16);
    lay->setSpacing(4);

    QFrame *bar = new QFrame(this);
    bar->setFixedHeight(3);
    bar->setStyleSheet(QString("background:%1;border-radius:2px;").arg(accent));
    lay->addWidget(bar);
    lay->addSpacing(8);

    QLabel *titleLbl = new QLabel(title, this);
    titleLbl->setObjectName("cardTitle");
    lay->addWidget(titleLbl);

    QHBoxLayout *valRow = new QHBoxLayout();
    valRow->setSpacing(4);
    valRow->setContentsMargins(0, 0, 0, 0);
    valueLabel = new QLabel(value, this);
    valueLabel->setObjectName("cardValue");
    QLabel *unitLbl = new QLabel(unit, this);
    unitLbl->setObjectName("cardUnit");
    valRow->addWidget(valueLabel);
    valRow->addWidget(unitLbl, 0, Qt::AlignBottom);
    valRow->addStretch();
    lay->addLayout(valRow);

    subtextLabel = new QLabel("", this);
    subtextLabel->setObjectName("cardSubtext");
    lay->addWidget(subtextLabel);
}

void StatCard::setValue(const QString &v) { valueLabel->setText(v); }
void StatCard::setSubtext(const QString &s) { subtextLabel->setText(s); }
void StatCard::pulse()
{
    QString orig = styleSheet();
    setStyleSheet(styleSheet() +
                  QString("QFrame#statCard{border-color:%1;}").arg(accentColor));
    QTimer::singleShot(300, this, [this, orig]
                       { setStyleSheet(orig); });
}

// ════════════════════════════════════════════════════════════
//  DashDeviceRow
// ════════════════════════════════════════════════════════════
DashDeviceRow::DashDeviceRow(const QString &name, const QString &type,
                             int battery, bool active, QWidget *parent)
    : QFrame(parent), isActive(active)
{
    setObjectName("deviceRow");
    setFixedHeight(56);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 0, 16, 0);
    lay->setSpacing(12);

    statusDot = new QLabel(this);
    statusDot->setFixedSize(10, 10);
    statusDot->setObjectName(active ? "dotActive" : "dotInactive");

    QVBoxLayout *info = new QVBoxLayout();
    info->setSpacing(2);
    nameLabel = new QLabel(name, this);
    nameLabel->setObjectName("devName");
    typeLabel = new QLabel(type, this);
    typeLabel->setObjectName("devType");
    info->addWidget(nameLabel);
    info->addWidget(typeLabel);

    batteryBar = new QProgressBar(this);
    batteryBar->setObjectName("batteryBar");
    batteryBar->setRange(0, 100);
    batteryBar->setFixedWidth(80);
    batteryBar->setFixedHeight(6);
    batteryBar->setTextVisible(false);

    batteryLabel = new QLabel(this);
    batteryLabel->setObjectName("batteryLabel");
    batteryLabel->setFixedWidth(36);
    batteryLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    removeBtn = new QPushButton("✕", this);
    removeBtn->setObjectName("rowRemoveBtn");
    removeBtn->setFixedSize(22, 22);
    removeBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBtn, &QPushButton::clicked,
            this, [this]
            { emit removeRequested(this); });

    updateBattery(battery);

    lay->addWidget(statusDot, 0, Qt::AlignVCenter);
    lay->addLayout(info, 1);
    lay->addWidget(batteryBar, 0, Qt::AlignVCenter);
    lay->addWidget(batteryLabel, 0, Qt::AlignVCenter);
    lay->addSpacing(8);
    lay->addWidget(removeBtn, 0, Qt::AlignVCenter);
}

void DashDeviceRow::updateBattery(int pct)
{
    if (!batteryBar || !batteryLabel)
        return;
    batteryBar->setValue(pct);
    batteryLabel->setText(QString("%1%").arg(pct));
    QString color = pct > 50 ? "#3fb950" : pct > 20 ? "#d29922"
                                                    : "#f85149";
    QTimer::singleShot(0, this, [this, color]()
                       {
        if (batteryBar)
            batteryBar->setStyleSheet(
                QString("QProgressBar{background:#21262d;border-radius:3px;border:none;}"
                        "QProgressBar::chunk{background:%1;border-radius:3px;}").arg(color));
        if (batteryLabel)
            batteryLabel->setStyleSheet(
                QString("color:%1;font-size:11px;").arg(color)); });
}

void DashDeviceRow::setActive(bool active)
{
    isActive = active;
    statusDot->setObjectName(active ? "dotActive" : "dotInactive");
    statusDot->style()->unpolish(statusDot);
    statusDot->style()->polish(statusDot);
}

// ════════════════════════════════════════════════════════════
//  SensorRow
// ════════════════════════════════════════════════════════════
SensorRow::SensorRow(const QString &name, const QString &type, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("sensorRow");
    setFixedHeight(48);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 0, 16, 0);
    lay->setSpacing(12);

    QLabel *dot = new QLabel(this);
    dot->setFixedSize(8, 8);
    dot->setObjectName("dotActive");

    QVBoxLayout *info = new QVBoxLayout();
    info->setSpacing(2);
    nameLabel = new QLabel(name, this);
    nameLabel->setObjectName("devName");
    typeLabel = new QLabel(type, this);
    typeLabel->setObjectName("devType");
    info->addWidget(nameLabel);
    info->addWidget(typeLabel);

    QPushButton *removeBtn2 = new QPushButton("✕", this);
    removeBtn2->setObjectName("rowRemoveBtn");
    removeBtn2->setFixedSize(22, 22);
    removeBtn2->setCursor(Qt::PointingHandCursor);
    connect(removeBtn2, &QPushButton::clicked,
            this, [this]
            { emit removeRequested(this); });

    lay->addWidget(dot, 0, Qt::AlignVCenter);
    lay->addLayout(info, 1);
    lay->addWidget(removeBtn2, 0, Qt::AlignVCenter);
}

QString SensorRow::sensorName() const { return nameLabel ? nameLabel->text() : QString(); }
QString SensorRow::sensorType() const { return typeLabel ? typeLabel->text() : QString(); }

// ════════════════════════════════════════════════════════════
//  LogEntry
// ════════════════════════════════════════════════════════════
LogEntry::LogEntry(const QString &text, const QString &level, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("logEntry");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QString color = (level == "WARN") ? "#d29922" : (level == "ERROR") ? "#f85149"
                                                                       : "#7d8590";
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 5, 16, 5);
    QLabel *lbl = new QLabel(text, this);
    lbl->setObjectName("logEntryText");
    lbl->setStyleSheet(QString("color:%1;font-family:'Cascadia Code','Consolas',monospace;"
                               "font-size:11px;")
                           .arg(color));
    lbl->setWordWrap(false);
    lay->addWidget(lbl);
}

// ════════════════════════════════════════════════════════════
//  LogPanel
// ════════════════════════════════════════════════════════════
LogPanel::LogPanel(const QString &title, bool showClearBtn, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("panel");
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    QWidget *ph = new QWidget(this);
    ph->setObjectName("panelHeader");
    ph->setFixedHeight(48);
    QHBoxLayout *phLay = new QHBoxLayout(ph);
    phLay->setContentsMargins(20, 0, 20, 0);
    QLabel *tl = new QLabel(title, ph);
    tl->setObjectName("panelTitle");
    phLay->addWidget(tl);
    phLay->addStretch();
    if (showClearBtn)
    {
        QPushButton *clr = new QPushButton("Clear", ph);
        clr->setObjectName("clearBtn");
        clr->setCursor(Qt::PointingHandCursor);
        connect(clr, &QPushButton::clicked, this, &LogPanel::clear);
        phLay->addWidget(clr);
    }
    outer->addWidget(ph);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("logScroll");

    QWidget *container = new QWidget();
    container->setObjectName("logContainer");
    listLayout = new QVBoxLayout(container);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);
    listLayout->addStretch();

    scroll->setWidget(container);
    outer->addWidget(scroll, 1);
}

void LogPanel::addEntry(const QString &text, const QString &level)
{
    auto *entry = new LogEntry(text, level, nullptr);
    listLayout->insertWidget(0, entry);
    while (listLayout->count() - 1 > maxEntries)
    {
        QLayoutItem *item = listLayout->takeAt(listLayout->count() - 2);
        if (item)
        {
            delete item->widget();
            delete item;
        }
    }
}

void LogPanel::clear()
{
    while (listLayout->count() > 1)
    {
        QLayoutItem *item = listLayout->takeAt(0);
        if (item)
        {
            delete item->widget();
            delete item;
        }
    }
}

// ════════════════════════════════════════════════════════════
//  MainWindow — constructor
// ════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Digital Twin Laboratory — Dashboard");
    setMinimumSize(1100, 700);
    resize(1280, 780);

    controller = new UIController(this);

    setupUi();
    applyStyles();

    connect(controller, &UIController::simTickUpdated,
            this, &MainWindow::onSimTickUpdated);
    connect(controller, &UIController::deviceUpdated,
            this, &MainWindow::onDeviceUpdated);
    connect(controller, &UIController::sensorUpdated,
            this, &MainWindow::onSensorUpdated);
    connect(controller, &UIController::logEvent,
            this, &MainWindow::onLogEvent);

    addLog("Simulation engine ready — press Start on the Simulation page", "INFO", "devices");
    addLog(QString("%1 drones registered").arg(deviceRows.size()), "INFO", "devices");
    addLog(QString("%1 sensors online").arg(sensorRows.size()), "INFO", "sensors");
}

// ════════════════════════════════════════════════════════════
//  UIController slots
// ════════════════════════════════════════════════════════════
void MainWindow::onSimTickUpdated(const SimStats &stats)
{
    simTick = stats.simTick;
    activeDevices = stats.activeDevices;
    activeSensors = stats.activeSensors;
    alertCount = stats.alertCount;

    if (cardSimTime)
        cardSimTime->setValue(formatTime(simTick));
    if (cardActiveDevices)
        cardActiveDevices->setValue(QString::number(activeDevices));
    if (cardActiveSensors)
        cardActiveSensors->setValue(QString::number(activeSensors));
    if (countBadge)
        countBadge->setText(QString::number(deviceRows.size() + sensorRows.size()));

    if (cardAvgBattery)
    {
        cardAvgBattery->setValue(QString::number(stats.avgBattery));
        cardAvgBattery->setSubtext(
            stats.avgBattery < 20 ? "⚠ Low battery" : stats.avgBattery < 50 ? "Moderate"
                                                                            : "Nominal");
    }
    if (cardAlerts)
    {
        cardAlerts->setValue(QString::number(alertCount));
        cardAlerts->setSubtext(alertCount > 0 ? "⚠ Check devices" : "No issues");
        if (alertCount > 0)
            cardAlerts->pulse();
    }
    if (cardSignal)
    {
        if (stats.signalStrength == 0)
        {
            cardSignal->setValue("0");
            cardSignal->setSubtext("⚠ Signal Lost");
        }
        else
        {
            cardSignal->setValue(QString::number(stats.signalStrength));
            cardSignal->setSubtext(stats.signalStrength > 50 ? "Stable" : "⚠ Weak");
        }
    }
}

void MainWindow::onDeviceUpdated(const DeviceInfo &device)
{
    // Update canonical Drones page rows
    for (int i = 0; i < deviceRows.size(); ++i)
    {
        if (deviceRows[i]->name() == device.name)
        {
            deviceRows[i]->updateBattery(device.battery);
            bool alive = device.active && device.battery > 0;
            deviceRows[i]->setActive(alive);
            // If battery died, log signal loss
            if (device.battery == 0)
            {
                addLog(QString("Signal lost: %1 — battery depleted").arg(device.name),
                       "ERROR", "battery");
            }
            break;
        }
    }
    // Mirror updates to dashboard display rows
    for (int i = 0; i < dashDroneRows.size(); ++i)
    {
        if (dashDroneRows[i]->name() == device.name)
        {
            dashDroneRows[i]->updateBattery(device.battery);
            bool alive = device.active && device.battery > 0;
            dashDroneRows[i]->setActive(alive);
            break;
        }
    }
}

void MainWindow::onSensorUpdated(const SensorInfo &sensor)
{
    // Sensor data is forwarded to the Sensors telemetry tab via logEvent(CLIMATE)
    // Nothing else needed here; the climate log is emitted by UIController
    Q_UNUSED(sensor);
}

void MainWindow::onLogEvent(const QString &msg, const QString &level)
{
    // Route CLIMATE events to the climate tab
    if (level == "CLIMATE")
    {
        addLog(msg, "INFO", "climate");
        return;
    }
    QString category = (level == "WARN" || level == "ERROR") ? "battery" : "devices";
    addLog(msg, level, category);
}

// ════════════════════════════════════════════════════════════
//  setupUi
// ════════════════════════════════════════════════════════════
void MainWindow::setupUi()
{
    QWidget *root = new QWidget(this);
    setCentralWidget(root);
    QHBoxLayout *rootLay = new QHBoxLayout(root);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);

    QWidget *sidebar = new QWidget(root);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);
    buildSidebar(sidebar);
    rootLay->addWidget(sidebar);

    mainStack = new QStackedWidget(root);
    mainStack->setObjectName("mainStack");

    // Page 0 — Dashboard
    QWidget *dashPage = new QWidget(mainStack);
    dashPage->setObjectName("dashboardPage");
    buildMainArea(dashPage);
    mainStack->addWidget(dashPage);

    // Page 1 — Drones (was Devices)
    QWidget *devPage = new QWidget(mainStack);
    QVBoxLayout *devLay = new QVBoxLayout(devPage);
    devLay->setContentsMargins(32, 28, 32, 28);
    devLay->addWidget(buildDevicePanel());
    mainStack->addWidget(devPage);

    // Page 2 — Sensors
    mainStack->addWidget(buildSensorsPanel());

    // Page 3 — Simulation Control
    simPage = new SimulationControlWindow(controller, mainStack);
    mainStack->addWidget(simPage);
    connect(simPage, &SimulationControlWindow::resetRequested,
            this, &MainWindow::onSimulationReset);

    // Page 4 — Telemetry
    mainStack->addWidget(buildTelemetryPage());

    // Page 5 — Alerts
    mainStack->addWidget(buildAlertsPage());

    rootLay->addWidget(mainStack, 1);
}

// ════════════════════════════════════════════════════════════
//  buildSidebar — "Devices" renamed to "Drones"
// ════════════════════════════════════════════════════════════
void MainWindow::buildSidebar(QWidget *parent)
{
    QVBoxLayout *lay = new QVBoxLayout(parent);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    QWidget *logoArea = new QWidget(parent);
    logoArea->setObjectName("logoArea");
    logoArea->setFixedHeight(72);
    QHBoxLayout *logoLay = new QHBoxLayout(logoArea);
    logoLay->setContentsMargins(20, 0, 20, 0);
    QLabel *logoIcon = new QLabel("⬡", logoArea);
    logoIcon->setObjectName("logoIcon");
    QLabel *logoText = new QLabel("DTLab", logoArea);
    logoText->setObjectName("logoText");
    logoLay->addWidget(logoIcon);
    logoLay->addWidget(logoText);
    logoLay->addStretch();
    lay->addWidget(logoArea);

    QFrame *div = new QFrame(parent);
    div->setFrameShape(QFrame::HLine);
    div->setObjectName("sideDiv");
    lay->addWidget(div);

    // "Devices" → "Drones"
    struct NavItem
    {
        QString icon;
        QString label;
    };
    QVector<NavItem> items = {
        {"⊞", "Dashboard"}, {"◈", "Drones"}, {"◉", "Sensors"}, {"▷", "Simulation"}, {"◫", "Telemetry"}, {"⚐", "Alerts"}};
    for (int i = 0; i < items.size(); ++i)
    {
        QPushButton *btn = new QPushButton(
            QString("  %1   %2").arg(items[i].icon, items[i].label), parent);
        btn->setObjectName(i == 0 ? "navBtnActive" : "navBtn");
        btn->setFixedHeight(44);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);
        navButtons.append(btn);
        lay->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, [this, i]()
                {
            mainStack->setCurrentIndex(i);
            for (int j=0;j<navButtons.size();++j)
                navButtons[j]->setObjectName(j==i?"navBtnActive":"navBtn");
            for (auto b:navButtons){b->style()->unpolish(b);b->style()->polish(b);} });
    }

    lay->addStretch();

    QFrame *div2 = new QFrame(parent);
    div2->setFrameShape(QFrame::HLine);
    div2->setObjectName("sideDiv");
    lay->addWidget(div2);

    QWidget *statusArea = new QWidget(parent);
    statusArea->setFixedHeight(60);
    QHBoxLayout *statusLay = new QHBoxLayout(statusArea);
    statusLay->setContentsMargins(20, 0, 20, 0);
    QLabel *dot = new QLabel(statusArea);
    dot->setObjectName("dotActive");
    dot->setFixedSize(8, 8);
    QLabel *statusTxt = new QLabel("Simulation Live", statusArea);
    statusTxt->setObjectName("statusTxt");
    statusLay->addWidget(dot, 0, Qt::AlignVCenter);
    statusLay->addSpacing(8);
    statusLay->addWidget(statusTxt);
    statusLay->addStretch();
    lay->addWidget(statusArea);
}

// ════════════════════════════════════════════════════════════
//  buildMainArea — dashboard page
// ════════════════════════════════════════════════════════════
void MainWindow::buildMainArea(QWidget *parent)
{
    QVBoxLayout *lay = new QVBoxLayout(parent);
    lay->setContentsMargins(32, 28, 32, 28);
    lay->setSpacing(24);

    QHBoxLayout *header = new QHBoxLayout();
    QVBoxLayout *headerText = new QVBoxLayout();
    QLabel *pageTitle = new QLabel("Dashboard", parent);
    pageTitle->setObjectName("pageTitle");
    QLabel *pageSub = new QLabel("Real-time simulation overview", parent);
    pageSub->setObjectName("pageSubtitle");
    headerText->addWidget(pageTitle);
    headerText->addWidget(pageSub);
    headerText->setSpacing(2);
    cardSimTime = new StatCard("Simulation Time", "00:00", "", "#388bfd", parent);
    cardSimTime->setFixedWidth(180);
    header->addLayout(headerText);
    header->addStretch();
    header->addWidget(cardSimTime);
    lay->addLayout(header);

    lay->addWidget(buildStatGrid());

    QHBoxLayout *panels = new QHBoxLayout();
    panels->setSpacing(24);
    panels->addWidget(buildDashDevicePanel(), 1);
    dashLog = new LogPanel("Simulation Log", true, parent);
    panels->addWidget(dashLog, 1);
    lay->addLayout(panels, 1);
}

// ════════════════════════════════════════════════════════════
//  buildStatGrid
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildStatGrid()
{
    QWidget *grid = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(grid);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(16);

    cardActiveDevices = new StatCard("Active Drones", "0", "", "#3fb950", grid);
    cardActiveSensors = new StatCard("Active Sensors", "0", "", "#388bfd", grid);
    cardAvgBattery = new StatCard("Avg Battery", "--", "%", "#d29922", grid);
    cardAlerts = new StatCard("Alerts", "0", "", "#f85149", grid);
    cardSignal = new StatCard("Signal Strength", "--", "%", "#bc8cff", grid);

    cardActiveDevices->setSubtext("Press Start on Simulation page");
    cardActiveSensors->setSubtext("Monitoring");
    cardAvgBattery->setSubtext("Waiting for sim...");
    cardAlerts->setSubtext("No issues");
    cardSignal->setSubtext("Waiting...");

    lay->addWidget(cardActiveDevices);
    lay->addWidget(cardActiveSensors);
    lay->addWidget(cardAvgBattery);
    lay->addWidget(cardAlerts);
    lay->addWidget(cardSignal);
    return grid;
}

// ════════════════════════════════════════════════════════════
//  buildDashDevicePanel — grouped Drones / Sensors, NO add btn
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildDashDevicePanel()
{
    QWidget *panel = new QWidget();
    panel->setObjectName("panel");
    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Header — title + count badge (NO add button)
    QWidget *ph = new QWidget(panel);
    ph->setObjectName("panelHeader");
    ph->setFixedHeight(48);
    QHBoxLayout *phLay = new QHBoxLayout(ph);
    phLay->setContentsMargins(20, 0, 20, 0);
    QLabel *title = new QLabel("Live Devices", ph);
    title->setObjectName("panelTitle");
    countBadge = new QLabel("0", ph);
    countBadge->setObjectName("countBadge");
    phLay->addWidget(title);
    phLay->addStretch();
    phLay->addWidget(countBadge);
    lay->addWidget(ph);

    QScrollArea *scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setObjectName("deviceScroll");
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *container = new QWidget();
    container->setObjectName("listContainer");
    QVBoxLayout *containerLay = new QVBoxLayout(container);
    containerLay->setContentsMargins(0, 0, 0, 0);
    containerLay->setSpacing(0);

    // ── Drones subheading ──────────────────────────────────
    QLabel *droneHeading = new QLabel("  🚁  Drones", container);
    droneHeading->setObjectName("subheading");
    droneHeading->setFixedHeight(32);
    containerLay->addWidget(droneHeading);

    QWidget *droneContainer = new QWidget(container);
    dashDroneLayout = new QVBoxLayout(droneContainer);
    dashDroneLayout->setContentsMargins(0, 0, 0, 0);
    dashDroneLayout->setSpacing(1);
    // Populate drones from controller — display-only rows (canonical list is on Drones page)
    for (const auto &d : controller->getAllDevices())
    {
        auto *row = new DashDeviceRow(d.name, d.type, d.battery, d.active, droneContainer);
        row->setDeviceId(d.id);
        dashDroneRows.append(row);
        dashDroneLayout->addWidget(row);
    }
    containerLay->addWidget(droneContainer);

    // ── Sensors subheading ─────────────────────────────────
    QLabel *sensorHeading = new QLabel("  📡  Sensors", container);
    sensorHeading->setObjectName("subheading");
    sensorHeading->setFixedHeight(32);
    containerLay->addWidget(sensorHeading);

    QWidget *sensorContainer = new QWidget(container);
    dashSensorLayout = new QVBoxLayout(sensorContainer);
    dashSensorLayout->setContentsMargins(0, 0, 0, 0);
    dashSensorLayout->setSpacing(1);
    // Sensor rows are added via addSensor() during buildSensorsPanel — not here.
    // This avoids duplication since buildSensorsPanel calls addSensor() which
    // already populates dashSensorLayout and registers in dashSensorWidgets.
    containerLay->addWidget(sensorContainer);
    containerLay->addStretch();

    scroll->setWidget(container);
    lay->addWidget(scroll, 1);

    updateDashboardCounts();
    return panel;
}

// ════════════════════════════════════════════════════════════
//  buildDevicePanel — DRONES page with add button
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildDevicePanel()
{
    if (sharedDeviceWidget)
        return sharedDeviceWidget;

    QWidget *panel = new QWidget();
    panel->setObjectName("panel");
    sharedDeviceWidget = panel;

    QVBoxLayout *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Header with "Drones" title and Add button
    QWidget *ph = new QWidget(panel);
    ph->setObjectName("panelHeader");
    ph->setFixedHeight(48);
    QHBoxLayout *phLay = new QHBoxLayout(ph);
    phLay->setContentsMargins(20, 0, 20, 0);
    QLabel *title = new QLabel("Registered Drones", ph);
    title->setObjectName("panelTitle");
    phLay->addWidget(title);
    phLay->addStretch();

    // Add Drone button — visible on this page
    QPushButton *addDroneBtn = new QPushButton("+ Add Drone", ph);
    addDroneBtn->setObjectName("addDroneBtn");
    addDroneBtn->setFixedHeight(30);
    addDroneBtn->setCursor(Qt::PointingHandCursor);
    phLay->addWidget(addDroneBtn);

    lay->addWidget(ph);

    QScrollArea *scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setObjectName("deviceScroll");
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *container = new QWidget();
    container->setObjectName("listContainer");
    deviceListLayout = new QVBoxLayout(container);
    deviceListLayout->setContentsMargins(0, 0, 0, 0);
    deviceListLayout->setSpacing(1);

    // Show existing drone rows — create new display widgets so dashboard rows are not re-parented
    for (const auto &d : controller->getAllDevices())
    {
        auto *row = new DashDeviceRow(d.name, d.type, d.battery, d.active, container);
        row->setDeviceId(d.id);
        connect(row, &DashDeviceRow::removeRequested, this, &MainWindow::removeDevice);
        deviceRows.append(row);
        deviceListLayout->addWidget(row);
    }
    deviceListLayout->addStretch();

    scroll->setWidget(container);
    lay->addWidget(scroll, 1);

    connect(addDroneBtn, &QPushButton::clicked, this, [this]()
            {
        try {
            bool ok=false;
            QString name = QInputDialog::getText(
                this,"Add Drone","Drone name:",QLineEdit::Normal,"",&ok);
            if (!ok || name.trimmed().isEmpty()) return;
            QStringList types={"Surveillance Drone","Autonomous Drone"};
            QString type = QInputDialog::getItem(
                this,"Add Drone","Drone type:",types,0,false,&ok);
            if (!ok || type.trimmed().isEmpty()) return;
            // Assign a new unique ID
            int newId = 100 + deviceRows.size();
            addDevice(name.trimmed(), type, 100, true, newId);
        } catch (const SimulationException& e) {
            QMessageBox::warning(this,"Error",QString::fromStdString(e.what()));
        } });

    return panel;
}

// ════════════════════════════════════════════════════════════
//  buildSensorsPanel
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildSensorsPanel()
{
    QWidget *page = new QWidget();
    QVBoxLayout *outer = new QVBoxLayout(page);
    outer->setContentsMargins(32, 28, 32, 28);
    outer->setSpacing(16);

    QLabel *title = new QLabel("Sensors", page);
    title->setObjectName("pageTitle");
    outer->addWidget(title);

    QWidget *card = new QWidget(page);
    card->setObjectName("panel");
    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    QWidget *ph = new QWidget(card);
    ph->setObjectName("panelHeader");
    ph->setFixedHeight(48);
    QHBoxLayout *phLay = new QHBoxLayout(ph);
    phLay->setContentsMargins(20, 0, 20, 0);
    QLabel *phTitle = new QLabel("Registered Sensors", ph);
    phTitle->setObjectName("panelTitle");
    QPushButton *addSensorBtn = new QPushButton("+ Add Sensor", ph);
    addSensorBtn->setObjectName("addDroneBtn");
    addSensorBtn->setFixedHeight(30);
    addSensorBtn->setCursor(Qt::PointingHandCursor);
    phLay->addWidget(phTitle);
    phLay->addStretch();
    phLay->addWidget(addSensorBtn);
    lay->addWidget(ph);

    QWidget *listContainer = new QWidget(card);
    sensorListLayout = new QVBoxLayout(listContainer);
    sensorListLayout->setContentsMargins(0, 0, 0, 0);
    sensorListLayout->setSpacing(1);
    sensorListLayout->addStretch(); // stretch first so insertWidget(count-1) works

    // Populate ALL sensors from controller — addSensor() handles both the
    // Sensors page row AND the dashboard Live Devices row in one place.
    for (const auto &s : controller->getAllSensors())
    {
        addSensor(s.name, s.type, s.id);
    }
    lay->addWidget(listContainer, 1);
    outer->addWidget(card, 1);

    connect(addSensorBtn, &QPushButton::clicked, this, [this]()
            {
        bool ok=false;
        QString name=QInputDialog::getText(
            this,"Add Sensor","Sensor name:",QLineEdit::Normal,"",&ok);
        if (!ok || name.trimmed().isEmpty()) return;
        QStringList types={"Climate Sensor","Environmental Sensor",
                           "Pressure Sensor","Humidity Sensor","Generic"};
        QString type=QInputDialog::getItem(
            this,"Add Sensor","Sensor type:",types,0,false,&ok);
        if (!ok || type.trimmed().isEmpty()) return;
        int newId = 200 + sensorRows.size();
        addSensor(name.trimmed(), type, newId); });
    return page;
}

// ════════════════════════════════════════════════════════════
//  buildTelemetryPage — Sensors tab shows ALL sensors + climate
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildTelemetryPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 28, 32, 28);
    lay->setSpacing(16);
    QLabel *title = new QLabel("Telemetry & Logs", page);
    title->setObjectName("pageTitle");
    lay->addWidget(title);

    QTabWidget *tabs = new QTabWidget(page);
    tabs->setObjectName("logTabs");

    // Sensors tab — shows all sensors with live climate readouts
    QWidget *sensorsTab = new QWidget();
    QVBoxLayout *sensorsLay = new QVBoxLayout(sensorsTab);
    sensorsLay->setContentsMargins(0, 0, 0, 0);
    sensorsLay->setSpacing(0);

    QScrollArea *sensorScroll = new QScrollArea();
    sensorScroll->setWidgetResizable(true);
    sensorScroll->setFrameShape(QFrame::NoFrame);
    QWidget *sensorListWidget = new QWidget();
    teleSensorLayout = new QVBoxLayout(sensorListWidget);
    teleSensorLayout->setContentsMargins(16, 8, 16, 8);
    teleSensorLayout->setSpacing(8);

    // Build a readout card for every sensor
    for (const auto &s : controller->getAllSensors())
    {
        QFrame *card = new QFrame(sensorListWidget);
        card->setObjectName("sensorTeleCard");
        card->setProperty("sensorId", s.id);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 12, 16, 12);
        cl->setSpacing(6);

        QHBoxLayout *hdr = new QHBoxLayout();
        QLabel *dot = new QLabel(card);
        dot->setFixedSize(8, 8);
        dot->setObjectName("dotActive");
        QLabel *nm = new QLabel(s.name, card);
        nm->setObjectName("devName");
        QLabel *tp = new QLabel(s.type, card);
        tp->setObjectName("devType");
        hdr->addWidget(dot, 0, Qt::AlignVCenter);
        hdr->addSpacing(8);
        hdr->addWidget(nm);
        hdr->addSpacing(8);
        hdr->addWidget(tp);
        hdr->addStretch();
        cl->addLayout(hdr);

        QHBoxLayout *vals = new QHBoxLayout();
        vals->setSpacing(24);
        auto makeVal = [&](const QString &lbl, const QString &val, const QString &unit)
        {
            QVBoxLayout *v = new QVBoxLayout();
            QLabel *l = new QLabel(lbl, card);
            l->setObjectName("cardTitle");
            QHBoxLayout *r = new QHBoxLayout();
            QLabel *vl = new QLabel(val, card);
            vl->setObjectName("teleValue");
            QLabel *ul = new QLabel(unit, card);
            ul->setObjectName("devType");
            r->addWidget(vl);
            r->addWidget(ul);
            r->addStretch();
            v->addWidget(l);
            v->addLayout(r);
            vals->addLayout(v);
        };
        makeVal("Temperature", QString::number(s.temperature, 'f', 1), "°C");
        makeVal("Humidity", QString::number(s.humidity, 'f', 1), "%");
        makeVal("Pressure", QString::number(s.pressure, 'f', 1), "hPa");
        makeVal("Signal", QString::number(s.signalStrength, 'f', 0), "%");
        cl->addLayout(vals);
        teleSensorLayout->addWidget(card);
    }
    teleSensorLayout->addStretch();
    sensorScroll->setWidget(sensorListWidget);
    sensorsLay->addWidget(sensorScroll, 1);

    // Update sensor cards live when sensorUpdated fires
    connect(controller, &UIController::sensorUpdated, this, [this](const SensorInfo &s)
            {
        if (!teleSensorLayout) return;
        for (int i = 0; i < teleSensorLayout->count(); ++i) {
            QLayoutItem* item = teleSensorLayout->itemAt(i);
            if (!item || !item->widget()) continue;
            QFrame* card = qobject_cast<QFrame*>(item->widget());
            if (!card || card->property("sensorId").toInt() != s.id) continue;
            // Find the teleValue labels inside this card
            QList<QLabel*> labels = card->findChildren<QLabel*>("teleValue");
            if (labels.size() >= 4) {
                labels[0]->setText(QString::number(s.temperature,   'f',1));
                labels[1]->setText(QString::number(s.humidity,      'f',1));
                labels[2]->setText(QString::number(s.pressure,      'f',1));
                labels[3]->setText(QString::number(s.signalStrength,'f',0));
            }
        } });

    // ── Drones tab — live telemetry cards for each drone ──────
    QWidget *dronesTab = new QWidget();
    QVBoxLayout *dronesTabLay = new QVBoxLayout(dronesTab);
    dronesTabLay->setContentsMargins(0, 0, 0, 0);
    dronesTabLay->setSpacing(0);

    QScrollArea *droneScroll = new QScrollArea();
    droneScroll->setWidgetResizable(true);
    droneScroll->setFrameShape(QFrame::NoFrame);
    QWidget *droneListWidget = new QWidget();
    teleDroneLayout = new QVBoxLayout(droneListWidget);
    teleDroneLayout->setContentsMargins(16, 8, 16, 8);
    teleDroneLayout->setSpacing(8);

    // Build a telemetry card for every drone
    for (const auto &d : controller->getAllDevices())
    {
        QFrame *card = new QFrame(droneListWidget);
        card->setObjectName("sensorTeleCard");
        card->setProperty("droneId", d.id);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 12, 16, 12);
        cl->setSpacing(6);

        QHBoxLayout *hdr = new QHBoxLayout();
        QLabel *dot = new QLabel(card);
        dot->setFixedSize(8, 8);
        dot->setObjectName(d.active ? "dotActive" : "dotInactive");
        dot->setProperty("droneDot", true);
        QLabel *nm = new QLabel(d.name, card);
        nm->setObjectName("devName");
        QLabel *tp = new QLabel(d.type, card);
        tp->setObjectName("devType");
        hdr->addWidget(dot, 0, Qt::AlignVCenter);
        hdr->addSpacing(8);
        hdr->addWidget(nm);
        hdr->addSpacing(8);
        hdr->addWidget(tp);
        hdr->addStretch();
        cl->addLayout(hdr);

        QHBoxLayout *vals = new QHBoxLayout();
        vals->setSpacing(24);
        auto makeDroneVal = [&](const QString &lbl, const QString &val, const QString &unit)
        {
            QVBoxLayout *v = new QVBoxLayout();
            QLabel *l = new QLabel(lbl, card);
            l->setObjectName("cardTitle");
            QHBoxLayout *r = new QHBoxLayout();
            QLabel *vl = new QLabel(val, card);
            vl->setObjectName("teleValue");
            QLabel *ul = new QLabel(unit, card);
            ul->setObjectName("devType");
            r->addWidget(vl);
            r->addWidget(ul);
            r->addStretch();
            v->addWidget(l);
            v->addLayout(r);
            vals->addLayout(v);
        };
        makeDroneVal("Altitude", QString::number(d.altitude, 'f', 1), "m");
        makeDroneVal("Speed", QString::number(d.speed, 'f', 1), "m/s");
        makeDroneVal("Battery", QString::number(d.battery), "%");
        makeDroneVal("X", QString::number(d.x, 'f', 1), "m");
        makeDroneVal("Y", QString::number(d.y, 'f', 1), "m");
        cl->addLayout(vals);
        teleDroneLayout->addWidget(card);
    }
    teleDroneLayout->addStretch();
    droneScroll->setWidget(droneListWidget);
    dronesTabLay->addWidget(droneScroll, 1);

    // Update drone cards live when deviceUpdated fires
    connect(controller, &UIController::deviceUpdated, this, [this](const DeviceInfo &d)
            {
        if (!teleDroneLayout) return;
        for (int i = 0; i < teleDroneLayout->count(); ++i) {
            QLayoutItem* item = teleDroneLayout->itemAt(i);
            if (!item || !item->widget()) continue;
            QFrame* card = qobject_cast<QFrame*>(item->widget());
            if (!card || card->property("droneId").toInt() != d.id) continue;
            QList<QLabel*> labels = card->findChildren<QLabel*>("teleValue");
            if (labels.size() >= 5) {
                labels[0]->setText(QString::number(d.altitude,'f',1));
                labels[1]->setText(QString::number(d.speed,   'f',1));
                labels[2]->setText(QString::number(d.battery));
                labels[3]->setText(QString::number(d.x,       'f',1));
                labels[4]->setText(QString::number(d.y,       'f',1));
            }
            // Update status dot
            for (auto* lbl : card->findChildren<QLabel*>()) {
                if (lbl->property("droneDot").toBool()) {
                    bool alive = d.active && d.battery > 0;
                    lbl->setObjectName(alive ? "dotActive" : "dotInactive");
                    lbl->style()->unpolish(lbl);
                    lbl->style()->polish(lbl);
                }
            }
        } });

    logSensors = new LogPanel("Sensor Events", false, nullptr); // hidden log buffer
    logDevices = new LogPanel("Drone Events", false, nullptr);  // hidden log buffer
    logBattery = new LogPanel("Battery Status", false, tabs);
    logClimate = new LogPanel("Climate Data", false, tabs);

    tabs->addTab(dronesTab, "🖥  Drones");
    tabs->addTab(sensorsTab, "📡  Sensors");
    tabs->addTab(logBattery, "🔋  Battery");
    tabs->addTab(logClimate, "🌡  Climate");

    lay->addWidget(tabs, 1);
    return page;
}

// ════════════════════════════════════════════════════════════
//  buildAlertsPage
// ════════════════════════════════════════════════════════════
QWidget *MainWindow::buildAlertsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 28, 32, 28);
    lay->setSpacing(16);
    QLabel *title = new QLabel("Alerts", page);
    title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("All warnings and errors from the simulation", page);
    sub->setObjectName("pageSubtitle");
    lay->addWidget(title);
    lay->addWidget(sub);
    logAlerts = new LogPanel("Alert Log", true, page);
    lay->addWidget(logAlerts, 1);
    return page;
}

// ════════════════════════════════════════════════════════════
//  addDevice / removeDevice
// ════════════════════════════════════════════════════════════
void MainWindow::addDevice(const QString &name, const QString &type,
                           int battery, bool active, int id)
{
    // Create canonical row for the Drones page
    auto *row = new DashDeviceRow(name, type, battery, active, nullptr);
    row->setDeviceId(id);
    connect(row, &DashDeviceRow::removeRequested, this, &MainWindow::removeDevice);
    deviceRows.append(row);
    if (deviceListLayout)
        deviceListLayout->insertWidget(qMax(0, deviceListLayout->count() - 1), row);

    // Create a separate mirror row for the dashboard Live Devices panel
    auto *dashRow = new DashDeviceRow(name, type, battery, active, nullptr);
    dashRow->setDeviceId(id);
    dashDroneRows.append(dashRow);
    if (dashDroneLayout)
        dashDroneLayout->addWidget(dashRow);

    updateDashboardCounts();
    addLog(QString("Drone added: %1 (%2)").arg(name, type), "INFO", "devices");

    // Register with UIController so the simulation engine tracks it
    DeviceInfo d;
    d.id = id;
    d.name = name;
    d.type = type;
    d.battery = battery;
    d.active = active;
    d.altitude = 0;
    d.speed = 0;
    d.x = 0;
    d.y = 0;
    d.z = 0;
    d.cameraOn = false;
    controller->addDevice(d);

    // Add a tab to the Simulation Control page drone panel
    if (simPage)
        simPage->addDroneTab(d);
}

void MainWindow::removeDevice(DashDeviceRow *row)
{
    int idx = deviceRows.indexOf(row);
    if (idx < 0)
        return;
    deviceRows.removeAt(idx);

    if (deviceListLayout)
        deviceListLayout->removeWidget(row);

    // Remove the corresponding dashboard mirror row by matching device ID
    int devId = row->deviceId();
    for (int i = 0; i < dashDroneRows.size(); ++i)
    {
        if (dashDroneRows[i]->deviceId() == devId)
        {
            if (dashDroneLayout)
                dashDroneLayout->removeWidget(dashDroneRows[i]);
            dashDroneRows[i]->deleteLater();
            dashDroneRows.removeAt(i);
            break;
        }
    }

    // Also remove from UIController so simulation detects no device
    if (devId >= 0)
        controller->removeDevice(devId);

    // Remove tab from Simulation Control page
    if (simPage)
        simPage->removeDroneTab(devId);

    QString n = row->name();
    row->deleteLater();
    updateDashboardCounts();
    addLog(QString("Drone removed: %1").arg(n), "WARN", "devices");
}

// ════════════════════════════════════════════════════════════
//  onSimulationReset — clears all devices/sensors and rebuilds from controller
// ════════════════════════════════════════════════════════════
void MainWindow::onSimulationReset()
{
    // ── Clear drone UI rows ───────────────────────────────
    for (auto *row : deviceRows)
    {
        if (deviceListLayout)
            deviceListLayout->removeWidget(row);
        row->deleteLater();
    }
    deviceRows.clear();
    for (auto *row : dashDroneRows)
    {
        if (dashDroneLayout)
            dashDroneLayout->removeWidget(row);
        row->deleteLater();
    }
    dashDroneRows.clear();

    // ── Clear sensor UI rows ──────────────────────────────
    for (auto *row : sensorRows)
    {
        if (sensorListLayout)
            sensorListLayout->removeWidget(row);
        row->deleteLater();
    }
    sensorRows.clear();
    for (auto it = dashSensorWidgets.begin(); it != dashSensorWidgets.end(); ++it)
    {
        if (dashSensorLayout)
            dashSensorLayout->removeWidget(it.value());
        it.value()->deleteLater();
    }
    dashSensorWidgets.clear();

    // ── Rebuild from fresh controller data ────────────────
    QWidget *droneContainer = dashDroneLayout ? dashDroneLayout->parentWidget() : nullptr;
    for (const auto &d : controller->getAllDevices())
    {
        auto *row = new DashDeviceRow(d.name, d.type, d.battery, d.active, droneContainer);
        row->setDeviceId(d.id);
        connect(row, &DashDeviceRow::removeRequested, this, &MainWindow::removeDevice);
        deviceRows.append(row);
        if (deviceListLayout)
            deviceListLayout->insertWidget(qMax(0, deviceListLayout->count() - 1), row);
        auto *dashRow = new DashDeviceRow(d.name, d.type, d.battery, d.active, droneContainer);
        dashRow->setDeviceId(d.id);
        dashDroneRows.append(dashRow);
        if (dashDroneLayout)
            dashDroneLayout->addWidget(dashRow);
        if (simPage)
            simPage->addDroneTab(d);
    }
    for (const auto &s : controller->getAllSensors())
    {
        addSensor(s.name, s.type, s.id);
    }

    updateDashboardCounts();
    addLog("Simulation reset — devices restored", "INFO", "devices");
}

// ════════════════════════════════════════════════════════════
//  addSensor / removeSensor
// ════════════════════════════════════════════════════════════
void MainWindow::addSensor(const QString &name, const QString &type, int id)
{
    QWidget *container = sensorListLayout ? sensorListLayout->parentWidget() : nullptr;
    auto *row = new SensorRow(name, type, container);
    row->setSensorId(id);
    connect(row, &SensorRow::removeRequested, this, &MainWindow::removeSensor);
    sensorRows.append(row);
    if (sensorListLayout)
        sensorListLayout->insertWidget(sensorListLayout->count() - 1, row);
    if (cardActiveSensors)
        cardActiveSensors->setValue(QString::number(sensorRows.size()));

    // Add a matching row to the dashboard Live Devices panel (only once per sensor id)
    if (dashSensorLayout && !dashSensorWidgets.contains(id))
    {
        QWidget *sensorContainer = dashSensorLayout->parentWidget();
        QFrame *sRow = new QFrame(sensorContainer);
        sRow->setObjectName("sensorRow");
        sRow->setFixedHeight(44);
        QHBoxLayout *sLay = new QHBoxLayout(sRow);
        sLay->setContentsMargins(16, 0, 16, 0);
        sLay->setSpacing(12);
        QLabel *sDot = new QLabel(sRow);
        sDot->setFixedSize(8, 8);
        sDot->setObjectName("dotActive");
        QVBoxLayout *sInfo = new QVBoxLayout();
        sInfo->setSpacing(1);
        QLabel *sName = new QLabel(name, sRow);
        sName->setObjectName("devName");
        QLabel *sType2 = new QLabel(type, sRow);
        sType2->setObjectName("devType");
        sInfo->addWidget(sName);
        sInfo->addWidget(sType2);
        sLay->addWidget(sDot, 0, Qt::AlignVCenter);
        sLay->addLayout(sInfo, 1);
        dashSensorLayout->addWidget(sRow);
        dashSensorWidgets[id] = sRow;
    }

    updateDashboardCounts();

    // Only register with UIController if not already tracked (avoids double-registration
    // when buildSensorsPanel populates the initial sensors that UIController already has)
    bool alreadyInController = false;
    for (const auto &s : controller->getAllSensors())
        if (s.id == id)
        {
            alreadyInController = true;
            break;
        }

    if (!alreadyInController)
    {
        SensorInfo s;
        s.id = id;
        s.name = name;
        s.type = type;
        s.temperature = 22.0f;
        s.humidity = 55.0f;
        s.pressure = 1013.0f;
        s.signalStrength = 90.0f;
        s.active = true;
        s.x = 0;
        s.y = 0;
        s.z = 0;
        controller->addSensor(s);
        addLog(QString("Sensor added: %1 (%2)").arg(name, type), "INFO", "sensors");
    }
}

void MainWindow::removeSensor(SensorRow *row)
{
    int idx = sensorRows.indexOf(row);
    if (idx < 0)
        return;
    sensorRows.removeAt(idx);
    if (sensorListLayout)
        sensorListLayout->removeWidget(row);

    // Remove from UIController — signal will be lost if no sensors remain either
    int sid = row->sensorId();
    if (sid >= 0)
        controller->removeSensor(sid);

    // Remove from dashboard Live Devices panel
    if (dashSensorWidgets.contains(sid))
    {
        QWidget *dashWidget = dashSensorWidgets.take(sid);
        if (dashSensorLayout)
            dashSensorLayout->removeWidget(dashWidget);
        dashWidget->deleteLater();
    }

    QString nm = row->sensorName();
    row->deleteLater();
    if (cardActiveSensors)
        cardActiveSensors->setValue(QString::number(sensorRows.size()));
    updateDashboardCounts();
    addLog(QString("Sensor removed: %1 — signal may be affected").arg(nm), "WARN", "sensors");
}

// ════════════════════════════════════════════════════════════
//  updateDashboardCounts — keeps badge and counts dynamic
// ════════════════════════════════════════════════════════════
void MainWindow::updateDashboardCounts()
{
    int total = deviceRows.size() + sensorRows.size();
    if (countBadge)
        countBadge->setText(QString::number(total));
    if (cardActiveDevices)
        cardActiveDevices->setValue(QString::number(deviceRows.size()));
    if (cardActiveSensors)
        cardActiveSensors->setValue(QString::number(sensorRows.size()));
}

// ════════════════════════════════════════════════════════════
//  addLog
// ════════════════════════════════════════════════════════════
void MainWindow::addLog(const QString &msg, const QString &level,
                        const QString &category)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString prefix = (level == "WARN") ? "⚠" : (level == "ERROR") ? "✕"
                                                                  : "●";
    QString text = QString("[%1] %2 %3").arg(time, prefix, msg);

    if (dashLog)
        dashLog->addEntry(text, level);

    LogPanel *target = logDevices;
    if (category == "sensors")
        target = logSensors;
    else if (category == "battery")
        target = logBattery;
    else if (category == "climate")
        target = logClimate;
    if (target)
        target->addEntry(text, level);

    if ((level == "WARN" || level == "ERROR") && logAlerts)
        logAlerts->addEntry(text, level);
}

// ════════════════════════════════════════════════════════════
//  formatTime
// ════════════════════════════════════════════════════════════
QString MainWindow::formatTime(int t)
{
    return QString("%1:%2").arg(t / 60, 2, 10, QChar('0')).arg(t % 60, 2, 10, QChar('0'));
}

// ════════════════════════════════════════════════════════════
//  applyStyles
// ════════════════════════════════════════════════════════════
void MainWindow::applyStyles()
{
    setStyleSheet(R"(
        QMainWindow,QWidget{background:#0d1117;color:#e6edf3;
            font-family:"Segoe UI",sans-serif;font-size:13px;}
        QWidget#sidebar{background:#161b22;border-right:1px solid #21262d;}
        QWidget#logoArea{background:transparent;}
        QLabel#logoIcon{font-size:22px;color:#3fb950;}
        QLabel#logoText{font-size:16px;font-weight:700;color:#e6edf3;}
        QFrame#sideDiv{color:#21262d;background:#21262d;max-height:1px;}
        QLabel#statusTxt{color:#7d8590;font-size:12px;}
        QPushButton#navBtn{text-align:left;padding-left:20px;color:#7d8590;
            background:transparent;border:none;font-size:13px;}
        QPushButton#navBtn:hover{background:#1c2128;color:#e6edf3;}
        QPushButton#navBtnActive{text-align:left;padding-left:20px;color:#e6edf3;
            background:#1c2128;border-left:3px solid #3fb950;
            border-top:none;border-right:none;border-bottom:none;
            font-size:13px;font-weight:600;}
        QLabel#pageTitle{font-size:22px;font-weight:700;color:#e6edf3;}
        QLabel#pageSubtitle{color:#7d8590;font-size:13px;}
        QLabel#subheading{color:#484f58;font-size:11px;font-weight:700;
            letter-spacing:1px;background:#0d1117;padding-left:16px;}
        QFrame#statCard{background:#161b22;border:1px solid #21262d;border-radius:10px;}
        QLabel#cardTitle{color:#7d8590;font-size:12px;}
        QLabel#cardValue{color:#e6edf3;font-size:28px;font-weight:700;}
        QLabel#cardUnit{color:#7d8590;font-size:14px;padding-bottom:4px;}
        QLabel#cardSubtext{color:#484f58;font-size:11px;}
        QWidget#panel{background:#161b22;border:1px solid #21262d;border-radius:10px;}
        QFrame#panel{background:#161b22;border:1px solid #21262d;border-radius:10px;}
        QWidget#panelHeader{background:#1c2128;
            border-top-left-radius:10px;border-top-right-radius:10px;
            border-bottom:1px solid #21262d;}
        QLabel#panelTitle{font-size:14px;font-weight:600;color:#e6edf3;}
        QLabel#countBadge{background:#238636;color:#fff;
            font-size:11px;font-weight:700;padding:2px 8px;border-radius:10px;}
        QPushButton#addDroneBtn{background:#238636;color:#fff;
            border:none;border-radius:6px;padding:0 14px;font-size:12px;font-weight:600;}
        QPushButton#addDroneBtn:hover{background:#2ea043;}
        QPushButton#smallBtn{background:#21262d;color:#7d8590;
            border:1px solid #30363d;border-radius:5px;font-size:14px;font-weight:700;}
        QPushButton#smallBtn:hover{background:#238636;color:#fff;border-color:#238636;}
        QPushButton#clearBtn{background:#21262d;color:#7d8590;
            border:1px solid #30363d;border-radius:6px;padding:3px 10px;font-size:11px;}
        QPushButton#clearBtn:hover{color:#e6edf3;}
        QFrame#deviceRow{background:#161b22;border-bottom:1px solid #21262d;}
        QFrame#deviceRow:hover{background:#1c2128;}
        QLabel#devName{font-size:13px;color:#e6edf3;font-weight:500;}
        QLabel#devType{font-size:11px;color:#484f58;}
        QLabel#teleValue{font-size:16px;font-weight:700;color:#e6edf3;}
        QLabel#dotActive{background:#3fb950;border-radius:5px;
            min-width:10px;min-height:10px;max-width:10px;max-height:10px;}
        QLabel#dotInactive{background:#484f58;border-radius:5px;
            min-width:10px;min-height:10px;max-width:10px;max-height:10px;}
        QPushButton#rowRemoveBtn{background:transparent;color:#484f58;
            border:1px solid transparent;border-radius:4px;font-size:11px;font-weight:700;}
        QPushButton#rowRemoveBtn:hover{background:#6e2020;color:#f85149;border-color:#6e2020;}
        QFrame#sensorRow{background:#161b22;border-bottom:1px solid #21262d;}
        QFrame#sensorRow:hover{background:#1c2128;}
        QFrame#sensorTeleCard{background:#161b22;border:1px solid #21262d;
            border-radius:8px;margin:2px 0;}
        QFrame#logEntry{background:#0d1117;border-bottom:1px solid #161b22;}
        QFrame#logEntry:hover{background:#161b22;}
        QWidget#logContainer{background:#0d1117;}
        QTabWidget#logTabs::pane{border:1px solid #21262d;
            border-radius:0 0 10px 10px;background:#0d1117;}
        QTabBar::tab{background:#1c2128;color:#7d8590;
            padding:6px 16px;border:1px solid #21262d;border-bottom:none;
            margin-right:2px;border-top-left-radius:6px;border-top-right-radius:6px;
            font-size:12px;}
        QTabBar::tab:selected{background:#161b22;color:#e6edf3;}
        QTabBar::tab:hover{color:#e6edf3;}
        QScrollBar:vertical{background:#161b22;width:6px;border:none;}
        QScrollBar::handle:vertical{background:#30363d;border-radius:3px;min-height:20px;}
        QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}
    )");
}
