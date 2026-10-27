#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

// ════════════════════════════════════════════════════════════
//  Static helper — keeps UI setup out of instance methods
// ════════════════════════════════════════════════════════════
class TerminalManager
{
public:
    static void styleConsole(QWidget *console);
    static void styleField(QLineEdit *field);
    static void styleButton(QPushButton *btn);
};

// ════════════════════════════════════════════════════════════
//  LoginWindow — Cyberpunk-terminal login with DT-Lab emblem
// ════════════════════════════════════════════════════════════
class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccessful();

private slots:
    void handleLogin();
    void tickScanLine();
    void tickLoadBar();   // animates the boot loading bar
    void tickScrollLog(); // scrolls the right-side data feed

private:
    QLineEdit *usernameField = nullptr;
    QLineEdit *passwordField = nullptr;
    QPushButton *loginButton = nullptr;
    QLabel *statusLabel = nullptr;
    QLabel *emblemLabel = nullptr;

    // Loading bar (bottom strip)
    QProgressBar *loadBar = nullptr;
    QLabel *loadTagLabel = nullptr;
    QLabel *loadPctLabel = nullptr;
    QLabel *loadBarStrip = nullptr; // the marquee text row

    // Right-side scrolling data feed
    QLabel *dataFeedLabel = nullptr;
    int feedOffset = 0;

    // Timers
    QTimer *scanTimer = nullptr;
    QTimer *loadBarTimer = nullptr;
    QTimer *feedTimer = nullptr;

    // State
    int scanLineY = 0;
    int loadBarValue = 0;

    // Loading sequence tags (marquee)
    static const QStringList BOOT_TAGS;
    int bootTagIdx = 0;

    void setupUi();
    void shakeAnimation();
    static bool validateCredentials(const QString &user, const QString &pass);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};

#endif // LOGINWINDOW_H