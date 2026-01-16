#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用信息
    app.setApplicationName("SCOM-X");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("SCOM-X");

    // 创建并显示主窗口
    MainWindow window;
    window.show();

    return app.exec();
}
