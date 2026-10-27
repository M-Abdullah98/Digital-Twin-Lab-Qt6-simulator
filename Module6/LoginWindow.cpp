#include "LoginWindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QStringList>

// ════════════════════════════════════════════════════════════
//  Boot sequence marquee tags
// ════════════════════════════════════════════════════════════
const QStringList LoginWindow::BOOT_TAGS = {
    "DTLAB-ARCHIVE", "QUANTUM KEY AUTH", "GEOSPATIAL DATABASE",
    "SECURE-KERNEL LOAD", "TELEMETRY INIT", "DEVICE REGISTRY",
    "SENSOR ARRAY", "FLIGHT CONTROLLER", "SIMULATION ENGINE",
    "NEURAL MESH SYNC"};

// ════════════════════════════════════════════════════════════
//  TerminalManager
// ════════════════════════════════════════════════════════════
void TerminalManager::styleConsole(QWidget *console)
{
    console->setStyleSheet(
        "background-color: rgba(6, 12, 20, 0.88);"
        "border: 1px solid rgba(0,188,212,0.30);"
        "border-radius: 4px;");
}

void TerminalManager::styleField(QLineEdit *field)
{
    field->setStyleSheet(
        "QLineEdit {"
        "  background: rgba(4, 10, 18, 0.7);"
        "  color: #e0f7fa;"
        "  border: 1px solid rgba(0,188,212,0.35);"
        "  border-radius: 4px;"
        "  font-family: 'Segoe UI', sans-serif;"
        "  font-size: 15px;"
        "  padding: 0 14px;"
        "  selection-background-color: #00bcd4;"
        "  selection-color: #000;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #00bcd4;"
        "  background: rgba(0,188,212,0.06);"
        "}");
}

void TerminalManager::styleButton(QPushButton *btn)
{
    btn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "    stop:0 rgba(0,188,212,0.22), stop:1 rgba(0,100,140,0.22));"
        "  color: #00e5ff;"
        "  border: 1px solid #00bcd4;"
        "  border-radius: 4px;"
        "  font-family: 'Cascadia Code','Consolas',monospace;"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "  letter-spacing: 2px;"
        "  padding: 0;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(0,188,212,0.32);"
        "  color: #ffffff;"
        "  border-color: #00e5ff;"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(0,188,212,0.50);"
        "}"
        "QPushButton:disabled {"
        "  background: rgba(0,0,0,0.3);"
        "  color: #37474f;"
        "  border-color: #263238;"
        "}");
}

// ════════════════════════════════════════════════════════════
//  Constructor
// ════════════════════════════════════════════════════════════
LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(900, 520);
    setWindowTitle("Digital Twin Laboratory — Secure Access");
    setAttribute(Qt::WA_StyledBackground, true);

    setupUi();

    // Scan line — steady sweep
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &LoginWindow::tickScanLine);
    scanTimer->start(28);

    // Boot progress bar — ticks up unevenly to feel real
    loadBarTimer = new QTimer(this);
    connect(loadBarTimer, &QTimer::timeout, this, &LoginWindow::tickLoadBar);
    loadBarTimer->start(120);

    // Right data feed scroll
    feedTimer = new QTimer(this);
    connect(feedTimer, &QTimer::timeout, this, &LoginWindow::tickScrollLog);
    feedTimer->start(60);
}

// ════════════════════════════════════════════════════════════
//  paintEvent — full scene: background, emblem, grid, scan line
// ════════════════════════════════════════════════════════════
void LoginWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int W = width(), H = height();

    // ── Deep space background ──────────────────────────────────────
    QLinearGradient bgGrad(0, 0, W, H);
    bgGrad.setColorAt(0.0, QColor(4, 8, 16));
    bgGrad.setColorAt(0.5, QColor(6, 12, 22));
    bgGrad.setColorAt(1.0, QColor(3, 7, 14));
    p.fillRect(rect(), bgGrad);

    // ── Fine HUD grid ──────────────────────────────────────────────
    p.setPen(QPen(QColor(0, 188, 212, 11), 1));
    for (int x = 0; x < W; x += 30)
        p.drawLine(x, 0, x, H);
    for (int y = 0; y < H; y += 30)
        p.drawLine(0, y, W, y);

    // ── Emblem drawing (DT-Lab symbol — Assassin's Creed-style) ───
    // Check for asset first; fall back to a procedurally drawn emblem
    QPixmap emblemPx(":/assets/emblem.png");
    int esz = 340;
    int ex = (W - esz) / 2;
    int ey = (H - esz) / 2 - 20;

    // ── DT-Lab hexagon badge emblem ──────────────────────────────
    // Drawn procedurally to match the reference: concentric hexagons,
    // neural-network node mesh, "DT" letterform in centre.
    // If :/assets/emblem.png is registered it overrides this fallback.
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);

    QPointF C(W / 2.0, H / 2.0 - 10); // centre of emblem
    qreal R = 150.0;                  // outer hexagon radius

    // Helper lambda — returns the 6 vertices of a regular hexagon
    // with flat-top orientation (first vertex at top-right)
    auto hexPts = [&](qreal r, QPointF off = {0, 0}) -> QPolygonF
    {
        QPolygonF pts;
        for (int i = 0; i < 6; ++i)
        {
            qreal a = M_PI / 180.0 * (60.0 * i - 30.0);
            pts << QPointF(C.x() + off.x() + r * qCos(a),
                           C.y() + off.y() + r * qSin(a));
        }
        return pts;
    };

    if (!emblemPx.isNull())
    {
        // Asset path — draw at full opacity handled by outer opacity block
        p.setOpacity(0.10);
        p.drawPixmap((int)(C.x() - esz / 2), (int)(C.y() - esz / 2),
                     emblemPx.scaled(esz, esz,
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        // ── Outer ring: 3 concentric hexagons (thick border badge) ──
        QColor ringCol(0, 188, 212);
        for (int ring = 0; ring < 4; ++ring)
        {
            qreal rr = R - ring * 10.0;
            QPen rp(ringCol, ring == 0 ? 2.5 : 1.2);
            rp.setCosmetic(true);
            p.setPen(rp);
            p.setBrush(Qt::NoBrush);
            p.setOpacity(ring == 0 ? 0.55 : 0.20 - ring * 0.03);
            p.drawPolygon(hexPts(rr));
        }

        // ── Inner hexagon fill (dark navy, like the reference) ──────
        p.setOpacity(0.22);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(10, 24, 48));
        p.drawPolygon(hexPts(R * 0.62));

        // ── Perspective hex grid lines (3D cube-face look) ──────────
        p.setOpacity(0.18);
        p.setPen(QPen(QColor(0, 188, 212), 0.8));
        p.setBrush(Qt::NoBrush);
        // Draw 4 nested smaller hexagons inside the inner hex
        for (int i = 1; i <= 4; ++i)
            p.drawPolygon(hexPts(R * 0.62 * i / 4.0));
        // Spoke lines from centre to each vertex of inner hex
        QPolygonF inner6 = hexPts(R * 0.62);
        for (const QPointF &v : inner6)
            p.drawLine(C, v);

        // ── Neural mesh: nodes + connecting lines ────────────────────
        // 8 nodes scattered in the inner region
        static const qreal nodeOffsets[][2] = {
            {-0.28, -0.30}, {0.05, -0.38}, {0.30, -0.12}, {-0.38, 0.04}, {0.10, 0.06}, {-0.10, 0.28}, {0.32, 0.25}, {-0.20, 0.40}};
        const int NODE_N = 8;
        QPointF nodes[NODE_N];
        for (int i = 0; i < NODE_N; ++i)
            nodes[i] = QPointF(C.x() + nodeOffsets[i][0] * R,
                               C.y() + nodeOffsets[i][1] * R);

        // Connection edges (index pairs)
        static const int edges[][2] = {
            {0, 1}, {1, 2}, {0, 3}, {1, 4}, {2, 4}, {3, 4}, {4, 5}, {4, 6}, {5, 7}, {3, 5}, {6, 7}, {2, 6}};
        p.setOpacity(0.38);
        p.setPen(QPen(QColor(180, 235, 255), 0.9));
        for (auto &e : edges)
            p.drawLine(nodes[e[0]], nodes[e[1]]);

        // Node dots
        p.setOpacity(0.70);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(220, 245, 255));
        for (int i = 0; i < NODE_N; ++i)
            p.drawEllipse(nodes[i], 4.2, 4.2);
        // Outer glow ring on each node
        p.setOpacity(0.20);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(0, 229, 255), 1.0));
        for (int i = 0; i < NODE_N; ++i)
            p.drawEllipse(nodes[i], 7.0, 7.0);

        // ── "DT" letterform in centre (ghosted) ─────────────────────
        p.setOpacity(0.28);
        p.setPen(QPen(QColor(0, 188, 212), 1.8));
        p.setBrush(Qt::NoBrush);
        QFont dtFont("Segoe UI", 28, QFont::Bold);
        p.setFont(dtFont);
        QRectF dtRect(C.x() - 28, C.y() - 20, 56, 40);
        p.drawText(dtRect, Qt::AlignCenter, "DT");

        // ── Outer badge white rim (like the sticker in the reference) ─
        p.setOpacity(0.14);
        p.setPen(QPen(QColor(255, 255, 255), 6.0));
        p.setBrush(Qt::NoBrush);
        p.drawPolygon(hexPts(R + 12));
    }
    p.restore();

    // ── Ambient radial glow (behind the console) ───────────────────
    QRadialGradient glow(QPointF(W / 2.0, H / 2.0), 260);
    glow.setColorAt(0.0, QColor(0, 188, 212, 22));
    glow.setColorAt(1.0, QColor(0, 188, 212, 0));
    p.fillRect(rect(), glow);

    // ── Scan line ─────────────────────────────────────────────────
    QLinearGradient scanGrad(0, scanLineY - 4, 0, scanLineY + 4);
    scanGrad.setColorAt(0.0, QColor(0, 229, 255, 0));
    scanGrad.setColorAt(0.5, QColor(0, 229, 255, 28));
    scanGrad.setColorAt(1.0, QColor(0, 229, 255, 0));
    p.fillRect(0, scanLineY - 4, W, 8, scanGrad);

    // ── Window border — cyan glow edge ─────────────────────────────
    p.setPen(QPen(QColor(0, 188, 212, 60), 1));
    p.drawRect(rect().adjusted(0, 0, -1, -1));

    // ── Corner bracket decorations ─────────────────────────────────
    p.setPen(QPen(QColor(0, 188, 212, 200), 2));
    const int b = 22, m = 10;
    // TL
    p.drawLine(m, m + b, m, m);
    p.drawLine(m, m, m + b, m);
    // TR
    p.drawLine(W - m - b, m, W - m, m);
    p.drawLine(W - m, m, W - m, m + b);
    // BL
    p.drawLine(m, H - m - b, m, H - m);
    p.drawLine(m, H - m, m + b, H - m);
    // BR
    p.drawLine(W - m - b, H - m, W - m, H - m);
    p.drawLine(W - m, H - m, W - m, H - m - b);
}

// ════════════════════════════════════════════════════════════
//  resizeEvent
// ════════════════════════════════════════════════════════════
void LoginWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

// ════════════════════════════════════════════════════════════
//  tickScanLine
// ════════════════════════════════════════════════════════════
void LoginWindow::tickScanLine()
{
    scanLineY = (scanLineY + 2) % (height() + 8);
    update();
}

// ════════════════════════════════════════════════════════════
//  tickLoadBar — advances the boot progress bar unevenly
// ════════════════════════════════════════════════════════════
void LoginWindow::tickLoadBar()
{
    if (!loadBar)
        return;
    if (loadBarValue >= 99)
        return; // hold at 99 until login

    // Stutter: advance by 1-3 per tick, pause occasionally
    int step = QRandomGenerator::global()->bounded(1, 4);
    loadBarValue = qMin(99, loadBarValue + step);
    loadBar->setValue(loadBarValue);
    if (loadPctLabel)
        loadPctLabel->setText(QString::number(loadBarValue) + "%");

    // Cycle the marquee tag every ~15%
    if (loadBarValue % 10 == 0 && loadTagLabel)
    {
        bootTagIdx = (bootTagIdx + 1) % BOOT_TAGS.size();
        QString tags = "LOADING . . .";
        for (int i = 0; i <= qMin(bootTagIdx, (int)BOOT_TAGS.size() - 1); ++i)
            tags += "  //  " + BOOT_TAGS[i];
        loadTagLabel->setText(tags);
    }
}

// ════════════════════════════════════════════════════════════
//  tickScrollLog — scrolls the right side data feed
// ════════════════════════════════════════════════════════════
void LoginWindow::tickScrollLog()
{
    if (!dataFeedLabel)
        return;
    feedOffset = (feedOffset + 1) % 120;

    // Pseudo-random terminal data lines — cycle through them
    static const QStringList lines = {
        "<00:333>  LOGIN:",
        "<00:333>  PASSWORD:>",
        "<00:343>  [SYSTEM ACCESS KEY]",
        "<00:343>  ~seg.~loudQ4A",
        "<00:347>  AUTH_VECTOR: 8x8C7V",
        "<00:347>  SECURE-KERNEL LOAD",
        "<00:347>  GEOSPATIAL DATABASE",
        "<00:348>  FLIGHT CTRL INIT",
        "<00:349>  SENSOR ARRAY READY",
        "<00:350>  DEVICE REGISTRY OK",
        "<00:351>  TELEMETRY ONLINE",
        "<00:352>  SIM ENGINE READY",
        "<00:353>  NEURAL MESH SYNC",
        "<00:354>  DTLAB-ARCHIVE MOUNT",
        "<00:355>  QUANTUM KEY AUTH",
        "<00:356>  SECURE-KERNEL LOAD",
    };

    // Show a window of 8 lines, sliding every few frames
    int start = (feedOffset / 8) % lines.size();
    QString text;
    for (int i = 0; i < 9; ++i)
    {
        int idx = (start + i) % lines.size();
        // Dim older lines, full brightness for newest
        if (i < 6)
            text += lines[idx] + "\n";
        else
            text += "<span style='color:#37474f;'>" + lines[idx] + "</span>\n";
    }
    dataFeedLabel->setText(text.trimmed());
}

// ════════════════════════════════════════════════════════════
//  setupUi
// ════════════════════════════════════════════════════════════
void LoginWindow::setupUi()
{
    // Full window base layout
    QVBoxLayout *rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);

    // ── TOP STRIP: "AUTHORIZED PERSONNEL ONLY  |  SIGN IN" ────────
    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(38);
    topBar->setStyleSheet(
        "background: rgba(0,0,0,0.55);"
        "border-bottom: 1px solid rgba(0,188,212,0.25);");
    QHBoxLayout *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(24, 0, 24, 0);

    QLabel *topLeft = new QLabel("AUTHORIZED PERSONNEL ONLY", topBar);
    topLeft->setStyleSheet(
        "color:#7ecfe0;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:11px;letter-spacing:3px;background:transparent;");

    QLabel *topRight = new QLabel("SIGN IN", topBar);
    topRight->setStyleSheet(
        "color:#ffffff;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:13px;font-weight:700;letter-spacing:3px;background:transparent;");

    topLay->addWidget(topLeft);
    topLay->addStretch();
    topLay->addWidget(topRight);
    rootLay->addWidget(topBar);

    // ── MAIN BODY: left divider | center console | right feed ──────
    QWidget *body = new QWidget(this);
    body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout *bodyLay = new QHBoxLayout(body);
    bodyLay->setContentsMargins(0, 0, 0, 0);
    bodyLay->setSpacing(0);
    rootLay->addWidget(body, 1);

    // ── LEFT PANEL (decorative circuit lines) ─────────────────────
    QWidget *leftPanel = new QWidget(body);
    leftPanel->setFixedWidth(160);
    leftPanel->setStyleSheet("background:transparent;");
    QVBoxLayout *leftLay = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(16, 20, 8, 20);
    leftLay->setSpacing(6);

    // DTL branding block in the left panel
    QLabel *dtlLabel = new QLabel("DT-LAB", leftPanel);
    dtlLabel->setStyleSheet(
        "color:#00bcd4;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:18px;font-weight:700;letter-spacing:4px;background:transparent;");
    QLabel *dtlSub = new QLabel("DIGITAL TWIN\nLABORATORY", leftPanel);
    dtlSub->setStyleSheet(
        "color:#37474f;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;letter-spacing:2px;line-height:1.6;background:transparent;");
    QFrame *leftDiv = new QFrame(leftPanel);
    leftDiv->setFrameShape(QFrame::HLine);
    leftDiv->setStyleSheet("background:rgba(0,188,212,0.20);max-height:1px;border:none;");

    QLabel *sysVer = new QLabel("SYS  v1.0.0", leftPanel);
    sysVer->setStyleSheet(
        "color:#263238;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;letter-spacing:2px;background:transparent;");

    leftLay->addSpacing(10);
    leftLay->addWidget(dtlLabel);
    leftLay->addWidget(dtlSub);
    leftLay->addSpacing(8);
    leftLay->addWidget(leftDiv);
    leftLay->addSpacing(6);
    leftLay->addWidget(sysVer);
    leftLay->addStretch();

    bodyLay->addWidget(leftPanel);

    // Vertical divider line
    QFrame *vDiv1 = new QFrame(body);
    vDiv1->setFrameShape(QFrame::VLine);
    vDiv1->setStyleSheet("background:rgba(0,188,212,0.18);max-width:1px;border:none;");
    bodyLay->addWidget(vDiv1);

    // ── CENTER CONSOLE ─────────────────────────────────────────────
    QWidget *console = new QWidget(body);
    console->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    console->setStyleSheet("background:transparent;");
    QVBoxLayout *conLay = new QVBoxLayout(console);
    conLay->setContentsMargins(40, 30, 40, 24);
    conLay->setSpacing(14);
    bodyLay->addWidget(console, 1);

    // Username field
    usernameField = new QLineEdit(console);
    usernameField->setFixedHeight(46);
    usernameField->setPlaceholderText("username");
    TerminalManager::styleField(usernameField);
    conLay->addWidget(usernameField);

    // Password field
    passwordField = new QLineEdit(console);
    passwordField->setFixedHeight(46);
    passwordField->setPlaceholderText("password");
    passwordField->setEchoMode(QLineEdit::Password);
    TerminalManager::styleField(passwordField);
    conLay->addWidget(passwordField);

    // Status label
    statusLabel = new QLabel("", console);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedHeight(18);
    statusLabel->setStyleSheet(
        "color:#f85149;font-family:'Segoe UI',sans-serif;"
        "font-size:11px;background:transparent;");
    conLay->addWidget(statusLabel);

    // Sign in button
    loginButton = new QPushButton("AUTHENTICATE", console);
    loginButton->setFixedHeight(48);
    loginButton->setCursor(Qt::PointingHandCursor);
    TerminalManager::styleButton(loginButton);
    conLay->addWidget(loginButton);
    conLay->addStretch();

    // Vertical divider
    QFrame *vDiv2 = new QFrame(body);
    vDiv2->setFrameShape(QFrame::VLine);
    vDiv2->setStyleSheet("background:rgba(0,188,212,0.18);max-width:1px;border:none;");
    bodyLay->addWidget(vDiv2);

    // ── RIGHT DATA FEED ────────────────────────────────────────────
    QWidget *rightPanel = new QWidget(body);
    rightPanel->setFixedWidth(200);
    rightPanel->setStyleSheet("background:transparent;");
    QVBoxLayout *rightLay = new QVBoxLayout(rightPanel);
    rightLay->setContentsMargins(10, 16, 16, 16);
    rightLay->setSpacing(0);

    QLabel *feedTitle = new QLabel("SYS LOG", rightPanel);
    feedTitle->setStyleSheet(
        "color:#00bcd4;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;letter-spacing:3px;background:transparent;");
    rightLay->addWidget(feedTitle);
    rightLay->addSpacing(6);

    QFrame *feedDiv = new QFrame(rightPanel);
    feedDiv->setFrameShape(QFrame::HLine);
    feedDiv->setStyleSheet("background:rgba(0,188,212,0.20);max-height:1px;border:none;");
    rightLay->addWidget(feedDiv);
    rightLay->addSpacing(8);

    dataFeedLabel = new QLabel(rightPanel);
    dataFeedLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dataFeedLabel->setTextFormat(Qt::RichText);
    dataFeedLabel->setWordWrap(false);
    dataFeedLabel->setStyleSheet(
        "color:#00bcd4;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;line-height:1.7;background:transparent;letter-spacing:0.5px;");
    rightLay->addWidget(dataFeedLabel);
    rightLay->addStretch();

    bodyLay->addWidget(rightPanel);

    // ── BOTTOM LOADING STRIP ───────────────────────────────────────
    QWidget *bottomStrip = new QWidget(this);
    bottomStrip->setFixedHeight(72);
    bottomStrip->setStyleSheet(
        "background: rgba(0,0,0,0.70);"
        "border-top: 1px solid rgba(0,188,212,0.35);");
    QVBoxLayout *botLay = new QVBoxLayout(bottomStrip);
    botLay->setContentsMargins(20, 8, 20, 8);
    botLay->setSpacing(4);

    // Marquee text row
    loadTagLabel = new QLabel("LOADING . . .", bottomStrip);
    loadTagLabel->setStyleSheet(
        "color:#00bcd4;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:10px;font-weight:700;letter-spacing:2px;background:transparent;");
    botLay->addWidget(loadTagLabel);

    // Progress bar row
    QHBoxLayout *barRow = new QHBoxLayout();
    barRow->setSpacing(10);

    loadBar = new QProgressBar(bottomStrip);
    loadBar->setRange(0, 100);
    loadBar->setValue(0);
    loadBar->setFixedHeight(6);
    loadBar->setTextVisible(false);
    loadBar->setStyleSheet(
        "QProgressBar {"
        "  background: rgba(0,188,212,0.10);"
        "  border: none;"
        "  border-radius: 3px;"
        "}"
        "QProgressBar::chunk {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "    stop:0 #00838f, stop:0.6 #00bcd4, stop:1 #00e5ff);"
        "  border-radius: 3px;"
        "}");

    loadPctLabel = new QLabel("0%", bottomStrip);
    loadPctLabel->setFixedWidth(36);
    loadPctLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    loadPctLabel->setStyleSheet(
        "color:#7ecfe0;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:10px;background:transparent;");

    barRow->addWidget(loadBar, 1);
    barRow->addWidget(loadPctLabel);
    botLay->addLayout(barRow);

    // Footer meta row
    QHBoxLayout *metaRow = new QHBoxLayout();
    QLabel *verLabel = new QLabel("DTL v1.0.0", bottomStrip);
    verLabel->setStyleSheet(
        "color:#263238;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;letter-spacing:1px;background:transparent;");
    QLabel *sysOnline = new QLabel("● SYSTEMS ONLINE", bottomStrip);
    sysOnline->setStyleSheet(
        "color:#3fb950;font-family:'Cascadia Code','Consolas',monospace;"
        "font-size:9px;letter-spacing:1px;background:transparent;");
    metaRow->addWidget(verLabel);
    metaRow->addStretch();
    metaRow->addWidget(sysOnline);
    botLay->addLayout(metaRow);

    rootLay->addWidget(bottomStrip);

    // ── Wire connections ───────────────────────────────────────────
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(usernameField, &QLineEdit::returnPressed, this, &LoginWindow::handleLogin);
    connect(passwordField, &QLineEdit::returnPressed, this, &LoginWindow::handleLogin);
}

// ════════════════════════════════════════════════════════════
//  shakeAnimation
// ════════════════════════════════════════════════════════════
void LoginWindow::shakeAnimation()
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "pos");
    anim->setDuration(260);
    QPoint o = pos();
    anim->setKeyValueAt(0.00, o);
    anim->setKeyValueAt(0.15, o + QPoint(-9, 0));
    anim->setKeyValueAt(0.30, o + QPoint(9, 0));
    anim->setKeyValueAt(0.50, o + QPoint(-6, 0));
    anim->setKeyValueAt(0.70, o + QPoint(6, 0));
    anim->setKeyValueAt(0.85, o + QPoint(-3, 0));
    anim->setKeyValueAt(1.00, o);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// ════════════════════════════════════════════════════════════
//  validateCredentials
// ════════════════════════════════════════════════════════════
bool LoginWindow::validateCredentials(const QString &user, const QString &pass)
{
    return (user == "admin" && pass == "1234");
}

// ════════════════════════════════════════════════════════════
//  handleLogin
// ════════════════════════════════════════════════════════════
void LoginWindow::handleLogin()
{
    QString user = usernameField->text().trimmed();
    QString pass = passwordField->text();

    if (user.isEmpty() || pass.isEmpty())
    {
        statusLabel->setText("Username and password are required.");
        shakeAnimation();
        return;
    }

    if (validateCredentials(user, pass))
    {
        statusLabel->setStyleSheet(
            "color:#3fb950;font-family:'Segoe UI',sans-serif;"
            "font-size:11px;background:transparent;");
        statusLabel->setText("Access granted — loading dashboard...");
        loginButton->setEnabled(false);
        scanTimer->stop();
        loadBarTimer->stop();
        feedTimer->stop();

        // Fill the bar to 100 and launch
        if (loadBar)
            loadBar->setValue(100);
        if (loadPctLabel)
            loadPctLabel->setText("100%");
        if (loadTagLabel)
            loadTagLabel->setText(
                "LOADING . . .  //  DTLAB-ARCHIVE  //  QUANTUM KEY AUTH"
                "  //  GEOSPATIAL DATABASE  //  SECURE-KERNEL LOAD");

        QTimer::singleShot(700, this, [this]()
                           {
            emit loginSuccessful();
            MainWindow* dashboard = new MainWindow();
            dashboard->show();
            close(); });
    }
    else
    {
        statusLabel->setStyleSheet(
            "color:#f85149;font-family:'Segoe UI',sans-serif;"
            "font-size:11px;background:transparent;");
        statusLabel->setText("Invalid username or password.");
        passwordField->clear();
        shakeAnimation();
    }
}