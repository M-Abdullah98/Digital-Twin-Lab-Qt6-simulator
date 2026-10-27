#include <QApplication>
#include "LoginWindow.h"

// Entry point — create the Qt application and show the login window.
// Everything else is launched from there via signals and slots.
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Digital Twin Laboratory");
    app.setApplicationVersion("1.0.0");

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();  // starts the Qt event loop
}
