#include "DeviceDashboard.h"

// ============================================================
//  DeviceRow
// ============================================================
DeviceRow::DeviceRow(const DeviceInfo& info, QWidget* parent)
    : QFrame(parent), id(info.id), active(info.active)
{
    setObjectName("deviceRow");
    setFixedHeight(56);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 0, 16, 0);
    lay->setSpacing(12);

    // Green/grey dot shows whether the device is online.
    statusDot = new QLabel(this);
    statusDot->setFixedSize(10, 10);
    statusDot->setObjectName(active ? "dotActive" : "dotInactive");

    // Device name and type stacked vertically.
    QVBoxLayout* info_col = new QVBoxLayout();
    info_col->setSpacing(2);
    nameLabel = new QLabel(info.name, this);
    nameLabel->setObjectName("devName");
    typeLabel = new QLabel(info.type, this);
    typeLabel->setObjectName("devType");
    info_col->addWidget(nameLabel);
    info_col->addWidget(typeLabel);

    // Battery progress bar (colour-coded green/amber/red).
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

    // "Open" button — lets the user drill into the drone control panel.
    openButton = new QPushButton("Open", this);
    openButton->setObjectName("openBtn");
    openButton->setFixedSize(52, 26);
    openButton->setCursor(Qt::PointingHandCursor);
    connect(openButton, &QPushButton::clicked,
            this, [this]{ emit openRequested(id); });

    lay->addWidget(statusDot,   0, Qt::AlignVCenter);
    lay->addLayout(info_col,    1);
    lay->addWidget(batteryBar,  0, Qt::AlignVCenter);
    lay->addWidget(batteryLabel,0, Qt::AlignVCenter);
    lay->addWidget(openButton,  0, Qt::AlignVCenter);

    refresh(info);  // apply initial values
}

void DeviceRow::refresh(const DeviceInfo& info) {
    active = info.active;
    statusDot->setObjectName(active ? "dotActive" : "dotInactive");
    statusDot->style()->unpolish(statusDot);
    statusDot->style()->polish(statusDot);

    batteryBar->setValue(info.battery);
    batteryLabel->setText(QString("%1%").arg(info.battery));
    applyBatteryColor(info.battery);
}

QString DeviceRow::deviceName() const {
    return nameLabel ? nameLabel->text() : QString();
}

void DeviceRow::applyBatteryColor(int pct) {
    // Choose colour based on remaining charge.
    QString color = pct > 50 ? "#3fb950" : pct > 20 ? "#d29922" : "#f85149";

    // Defer stylesheet application to avoid constructor-time crashes.
    QTimer::singleShot(0, this, [this, color]() {
        if (batteryBar)
            batteryBar->setStyleSheet(
                QString("QProgressBar { background:#21262d; border-radius:3px; border:none; }"
                        "QProgressBar::chunk { background:%1; border-radius:3px; }").arg(color));
        if (batteryLabel)
            batteryLabel->setStyleSheet(
                QString("color:%1; font-size:11px;").arg(color));
    });
}

// ============================================================
//  DeviceDashboard
// ============================================================
DeviceDashboard::DeviceDashboard(UIController* ctrl, QWidget* parent)
    : QWidget(parent), controller(ctrl)
{
    setupUi();
    applyStyles();
    populateList();

    // Listen for live updates from the controller.
    connect(controller, &UIController::deviceUpdated,
            this, &DeviceDashboard::onDeviceUpdated);
    connect(controller, &UIController::simTickUpdated,
            this, &DeviceDashboard::onSimTickUpdated);
}

void DeviceDashboard::setupUi() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    // ── Page header ───────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout();
    QVBoxLayout* titleCol = new QVBoxLayout();
    QLabel* title = new QLabel("Device Dashboard", this);
    title->setObjectName("pageTitle");
    QLabel* sub = new QLabel("Manage and monitor all registered devices", this);
    sub->setObjectName("pageSubtitle");
    titleCol->addWidget(title);
    titleCol->addWidget(sub);
    titleCol->setSpacing(2);

    // Active-device count badge (top-right).
    countBadge = new QLabel("0", this);
    countBadge->setObjectName("countBadge");

    header->addLayout(titleCol);
    header->addStretch();
    header->addWidget(countBadge);
    root->addLayout(header);

    // ── Toolbar: Add / Remove ─────────────────────────────────
    QHBoxLayout* toolbar = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("+ Add Device", this);
    addBtn->setObjectName("toolbarBtn");
    addBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* removeBtn = new QPushButton("- Remove Last", this);
    removeBtn->setObjectName("toolbarBtnDanger");
    removeBtn->setCursor(Qt::PointingHandCursor);

    connect(addBtn,    &QPushButton::clicked, this, &DeviceDashboard::onAddDevice);
    connect(removeBtn, &QPushButton::clicked, this, &DeviceDashboard::onRemoveDevice);

    toolbar->addWidget(addBtn);
    toolbar->addWidget(removeBtn);
    toolbar->addStretch();
    root->addLayout(toolbar);

    // ── Scrollable device list ────────────────────────────────
    QWidget* panel = new QWidget(this);
    panel->setObjectName("panel");
    QVBoxLayout* panelLay = new QVBoxLayout(panel);
    panelLay->setContentsMargins(0, 0, 0, 0);
    panelLay->setSpacing(0);

    // Panel header bar with column labels.
    QWidget* panelHeader = new QWidget(panel);
    panelHeader->setObjectName("panelHeader");
    panelHeader->setFixedHeight(36);
    QHBoxLayout* phLay = new QHBoxLayout(panelHeader);
    phLay->setContentsMargins(16, 0, 16, 0);
    auto makeColLabel = [&](const QString& t) {
        QLabel* l = new QLabel(t, panelHeader);
        l->setObjectName("colLabel");
        return l;
    };
    phLay->addWidget(makeColLabel(""),         0);          // dot
    phLay->addSpacing(10);
    phLay->addWidget(makeColLabel("NAME"),      1);
    phLay->addWidget(makeColLabel("BATTERY"),   0);
    phLay->addSpacing(40);
    phLay->addWidget(makeColLabel(""),          0);          // open btn
    panelLay->addWidget(panelHeader);

    // The scroll area holds the list of DeviceRow widgets.
    QScrollArea* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("deviceScroll");

    QWidget* listContainer = new QWidget();
    listContainer->setObjectName("listContainer");
    listLayout = new QVBoxLayout(listContainer);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(1);
    listLayout->addStretch();   // stretch at bottom keeps rows packed to the top

    scroll->setWidget(listContainer);
    panelLay->addWidget(scroll, 1);

    root->addWidget(panel, 1);
}

void DeviceDashboard::populateList() {
    // Build one DeviceRow for each device the controller knows about.
    QWidget* container = listLayout->parentWidget();
    for (const auto& info : controller->getAllDevices()) {
        auto* row = new DeviceRow(info, container);
        connect(row, &DeviceRow::openRequested,
                this, &DeviceDashboard::onOpenDevice);
        rows.append(row);
        // Insert before the stretch at the end.
        listLayout->insertWidget(listLayout->count() - 1, row);
    }

    // Update the badge with the initial active count.
    SimStats st = controller->getSimStats();
    countBadge->setText(QString::number(st.activeDevices));
}

// ── Slots ─────────────────────────────────────────────────────
void DeviceDashboard::onDeviceUpdated(const DeviceInfo& device) {
    // Find the row for this device and refresh it.
    for (auto* row : rows) {
        if (row->deviceId() == device.id) {
            row->refresh(device);
            return;
        }
    }
}

void DeviceDashboard::onSimTickUpdated(const SimStats& stats) {
    // Keep the count badge in sync with the live active-device count.
    countBadge->setText(QString::number(stats.activeDevices));
}

void DeviceDashboard::onAddDevice() {
    // Placeholder: in a real build this opens a dialog to enter device details.
    int newId = rows.size() + 10;
    DeviceInfo newDev{newId,
                      QString("Device-%1").arg(newId),
                      "Generic", 100, true, 0.0f, 0.0f};
    QWidget* container = listLayout->parentWidget();
    auto* row = new DeviceRow(newDev, container);
    connect(row, &DeviceRow::openRequested,
            this, &DeviceDashboard::onOpenDevice);
    rows.append(row);
    listLayout->insertWidget(listLayout->count() - 1, row);
    emit logEvent(QString("Device added: %1").arg(newDev.name), "INFO");
}

void DeviceDashboard::onRemoveDevice() {
    if (rows.isEmpty()) return;
    auto* row = rows.takeLast();
    emit logEvent(QString("Device removed: %1").arg(row->deviceName()), "INFO");
    listLayout->removeWidget(row);
    delete row;
}

void DeviceDashboard::onOpenDevice(int deviceId) {
    // Forward to the parent window which will show the Drone Control Panel.
    emit openDronePanel(deviceId);
}

void DeviceDashboard::applyStyles() {
    // Reuses the same palette as MainWindow so everything looks consistent.
    setStyleSheet(R"(
        QWidget { background:#0d1117; color:#e6edf3;
                  font-family:"Segoe UI",sans-serif; font-size:13px; }

        QLabel#pageTitle    { font-size:22px; font-weight:700; color:#e6edf3; }
        QLabel#pageSubtitle { color:#7d8590; font-size:13px; }
        QLabel#colLabel     { color:#484f58; font-size:10px; font-weight:700;
                              letter-spacing:1px; }

        QLabel#countBadge {
            background:#238636; color:#ffffff;
            font-size:11px; font-weight:700;
            padding:2px 10px; border-radius:10px;
        }

        QPushButton#toolbarBtn {
            background:#1c2128; color:#e6edf3;
            border:1px solid #30363d; border-radius:6px;
            padding:5px 14px; font-size:12px;
        }
        QPushButton#toolbarBtn:hover { background:#238636; border-color:#238636; }

        QPushButton#toolbarBtnDanger {
            background:#1c2128; color:#7d8590;
            border:1px solid #30363d; border-radius:6px;
            padding:5px 14px; font-size:12px;
        }
        QPushButton#toolbarBtnDanger:hover { background:#6e2020; color:#f85149;
                                             border-color:#6e2020; }

        QWidget#panel { background:#161b22; border:1px solid #21262d; border-radius:10px; }
        QWidget#panelHeader {
            background:#1c2128;
            border-top-left-radius:10px; border-top-right-radius:10px;
            border-bottom:1px solid #21262d;
        }

        QFrame#deviceRow { background:#161b22; border-bottom:1px solid #21262d; }
        QFrame#deviceRow:hover { background:#1c2128; }
        QLabel#devName { font-size:13px; color:#e6edf3; font-weight:500; }
        QLabel#devType { font-size:11px; color:#484f58; }

        QLabel#dotActive   { background:#3fb950; border-radius:5px;
                             min-width:10px; min-height:10px;
                             max-width:10px; max-height:10px; }
        QLabel#dotInactive { background:#484f58; border-radius:5px;
                             min-width:10px; min-height:10px;
                             max-width:10px; max-height:10px; }

        QPushButton#openBtn {
            background:#1c2128; color:#388bfd;
            border:1px solid #30363d; border-radius:5px;
            font-size:11px;
        }
        QPushButton#openBtn:hover { background:#1f6feb; color:#ffffff; }

        QScrollBar:vertical { background:#161b22; width:6px; border:none; }
        QScrollBar::handle:vertical { background:#30363d; border-radius:3px; min-height:20px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
    )");
}