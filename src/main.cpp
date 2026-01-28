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

// 日志文件
QFile *logFile = nullptr;

void setupLogging()
{
    QString logPath = QDir::homePath() + "/.config/SCOM-X/debug.log";
    QDir().mkpath(QDir::homePath() + "/.config/SCOM-X");
    logFile = new QFile(logPath);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        logFile->write(QString("\n========== 应用启动 %1 ==========\n").arg(QDateTime::currentDateTime().toString()).toLatin1());
        logFile->flush();
    }
}

void debugLog(const QString &msg)
{
    if (logFile && logFile->isOpen()) {
        logFile->write(msg.toLatin1());
        logFile->write("\n");
        logFile->flush();
    }
    qDebug() << msg;
}

int main(int argc, char *argv[])
{
    setupLogging();
    debugLog("[MAIN] 应用启动中...");
    
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
    debugLog("[MAIN] 开始创建 MainWindow...");
    try {
        MainWindow window;
        debugLog("[MAIN] MainWindow 创建成功");
        splash.finish(&window);  // 关闭启动画面并指定主窗口
        debugLog("[MAIN] splash.finish() 完成");
        window.show();
        debugLog("[MAIN] window.show() 完成");
        debugLog("[MAIN] 进入事件循环");
        if (logFile) logFile->close();
        return app.exec();
    } catch (const std::exception &e) {
        QString errMsg = QString("[CRITICAL] 创建 MainWindow 时异常: %1").arg(e.what());
        debugLog(errMsg);
        if (logFile) logFile->close();
        return -1;
    } catch (...) {
        debugLog("[CRITICAL] 创建 MainWindow 时发生未知异常");
        if (logFile) logFile->close();
        return -1;
    }
}
