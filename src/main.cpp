#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "main_window.h"
#include "splash_screen.h"
#include "palette.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用信息
    app.setApplicationName("SCOM-X");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("SCOM-X");

    // 创建并显示启动画面
    SplashScreen splash;
    splash.show();
    app.processEvents();

    // ========== 加载步骤 1: 初始化应用程序 ==========
    splash.showLoadingMessage("初始化应用程序...", 10);
    QThread::msleep(100);

    // ========== 加载步骤 2: 加载配置文件 ==========
    splash.showLoadingMessage("加载配置文件...", 25);
    QFile configDir(QString("%1/.config/SCOM-X").arg(QDir::homePath()));
    QThread::msleep(200);

    // ========== 加载步骤 3: 加载样式表 ==========
    splash.showLoadingMessage("加载样式表...", 40);
    loadFlatStyle();  // 使用颜色调色板加载样式
    QThread::msleep(100);

    // ========== 加载步骤 4: 初始化串口 ==========
    splash.showLoadingMessage("初始化串口模块...", 55);
    QThread::msleep(150);

    // ========== 加载步骤 5: 建立信号连接 ==========
    splash.showLoadingMessage("建立信号连接...", 70);
    QThread::msleep(100);

    // ========== 加载步骤 6: 加载用户设置 ==========
    splash.showLoadingMessage("加载用户设置...", 85);
    QThread::msleep(150);

    // ========== 加载步骤 7: 准备完毕 ==========
    splash.showLoadingMessage("准备完毕!", 100);
    QThread::msleep(200);

    // 创建并显示主窗口
    MainWindow window;
    splash.finish(&window);  // 关闭启动画面并指定主窗口
    window.show();

    return app.exec();
}
