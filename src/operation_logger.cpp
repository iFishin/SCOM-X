#include "operation_logger.h"

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include <QMutex>

static QMutex logMutex;

OperationLogger &OperationLogger::instance()
{
    static OperationLogger logger;
    return logger;
}

OperationLogger::OperationLogger()
{
}

OperationLogger::~OperationLogger()
{
}

void OperationLogger::initialize()
{
    // 创建日志目录
    QString logDir = QString("%1/.config/SCOM-X").arg(QDir::homePath());
    QDir().mkpath(logDir);
    
    // 设置日志文件路径
    logFilePath = logDir + "/operation.log";
    
    // 初始化日志文件
    QFile file(logFilePath);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            out << "========== SCOM-X 操作日志 ==========\n";
            out << QString("启动时间: %1\n").arg(formatTimestamp());
            out << "=====================================\n\n";
            file.close();
        }
    }
    
    logInfo("日志系统已初始化");
}

QString OperationLogger::formatTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

void OperationLogger::writeLog(const QString &level, const QString &message)
{
    QMutexLocker locker(&logMutex);
    
    QString logMessage = QString("[%1] [%2] %3")
        .arg(formatTimestamp())
        .arg(level)
        .arg(message);
    
    // 写入文件
    QFile file(logFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << logMessage << "\n";
        file.close();
    }
    
    // 发射信号
    emit logMessageAdded(logMessage);
}

void OperationLogger::logSerialConnect(const QString &port, int baudRate)
{
    QString msg = QString("串口连接: %1 @ %2 bps").arg(port).arg(baudRate);
    writeLog("CONNECT", msg);
}

void OperationLogger::logSerialDisconnect()
{
    writeLog("DISCONNECT", "串口已断开");
}

void OperationLogger::logDataSent(const QString &data, int bytes)
{
    QString msg = QString("发送数据: %1 字节 | 内容: %2").arg(bytes).arg(data.left(100));
    writeLog("SEND", msg);
}

void OperationLogger::logDataReceived(const QString &data, int bytes)
{
    QString msg = QString("接收数据: %1 字节 | 内容: %2").arg(bytes).arg(data.left(100));
    writeLog("RECV", msg);
}

void OperationLogger::logCommandExecuted(const QString &command)
{
    QString msg = QString("执行命令: %1").arg(command);
    writeLog("CMD", msg);
}

void OperationLogger::logPageSwitch(const QString &pageName)
{
    QString msg = QString("切换页面: %1").arg(pageName);
    writeLog("PAGE", msg);
}

void OperationLogger::logSettingChanged(const QString &settingName, const QString &oldValue, const QString &newValue)
{
    QString msg = QString("设置变更: %1 | 旧值: %2 | 新值: %3")
        .arg(settingName)
        .arg(oldValue.isEmpty() ? "N/A" : oldValue)
        .arg(newValue.isEmpty() ? "N/A" : newValue);
    writeLog("SETTING", msg);
}

void OperationLogger::logError(const QString &errorMsg)
{
    writeLog("ERROR", errorMsg);
}

void OperationLogger::logWarning(const QString &warningMsg)
{
    writeLog("WARN", warningMsg);
}

void OperationLogger::logInfo(const QString &infoMsg)
{
    writeLog("INFO", infoMsg);
}
