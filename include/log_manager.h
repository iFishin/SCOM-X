#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <QString>
#include <QVector>
#include <QMutex>
#include <QDateTime>

/**
 * @class LogManager
 * @brief 全局日志管理器
 * 
 * 单例模式，用于管理应用的日志记录
 */
class LogManager {
public:
    static LogManager& instance();
    
    void initialize();
    
    // 日志统计结构
    struct LogStats {
        int debugCount = 0;
        int infoCount = 0;
        int warningCount = 0;
        int criticalCount = 0;
        int fileSize = 0;
    };
    
    LogStats getStats() const;
    
    QString getLastLogs(int lineCount) const;
    
    bool exportLogs(const QString &filePath) const;
    
    QString getLogFilePath() const;
    QString getConfigFilePath() const;

private:
    LogManager();
    ~LogManager();
    
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    void createLogDirectory();
    void createLogFile();
    
    QString logDirPath;
    QString logFilePath;
    QString configFilePath;
    
    mutable QMutex mutex;
    mutable QVector<QString> memoryBuffer;
    static const int MAX_MEMORY_LINES = 10000;
};

#endif // LOG_MANAGER_H
