#include "log_manager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

LogManager& LogManager::instance()
{
    static LogManager manager;
    return manager;
}

LogManager::LogManager()
{
}

LogManager::~LogManager()
{
}

void LogManager::initialize()
{
    createLogDirectory();
    createLogFile();
}

void LogManager::createLogDirectory()
{
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (docsPath.isEmpty()) {
        docsPath = QDir::homePath();
    }
    
    logDirPath = docsPath + "/SCOM-X_Logs";
    configFilePath = logDirPath + "/config.json";
    logFilePath = logDirPath + "/application.log";
    
    QDir dir;
    if (!dir.exists(logDirPath)) {
        dir.mkpath(logDirPath);
    }
}

void LogManager::createLogFile()
{
    QFile file(logFilePath);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "[SCOM-X] Log file created at " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
            file.close();
        }
    }
}

LogManager::LogStats LogManager::getStats() const
{
    QMutexLocker locker(&mutex);
    LogStats stats;
    // 这里可以从日志文件中统计
    return stats;
}

QString LogManager::getLastLogs(int lineCount) const
{
    QMutexLocker locker(&mutex);
    
    QFile file(logFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);
    
    int startLine = qMax(0, lines.size() - lineCount);
    QString result = lines.mid(startLine).join("\n");
    
    return result;
}

bool LogManager::exportLogs(const QString &filePath) const
{
    QMutexLocker locker(&mutex);
    
    QFile sourceFile(logFilePath);
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QFile destFile(filePath);
    if (!destFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        sourceFile.close();
        return false;
    }
    
    QTextStream inStream(&sourceFile);
    QTextStream outStream(&destFile);
    outStream << inStream.readAll();
    
    sourceFile.close();
    destFile.close();
    
    return true;
}

QString LogManager::getLogFilePath() const
{
    return logFilePath;
}

QString LogManager::getConfigFilePath() const
{
    return configFilePath;
}
