#ifndef OPERATION_LOGGER_H
#define OPERATION_LOGGER_H

#include <QString>
#include <QObject>
#include <memory>

/**
 * @brief 操作日志记录器 - 记录用户所有操作
 */
class OperationLogger : public QObject {
    Q_OBJECT

public:
    static OperationLogger &instance();
    
    // 初始化日志系统
    void initialize();
    
    // 记录各种操作日志
    void logSerialConnect(const QString &port, int baudRate);
    void logSerialDisconnect();
    void logDataSent(const QString &data, int bytes);
    void logDataReceived(const QString &data, int bytes);
    void logCommandExecuted(const QString &command);
    void logPageSwitch(const QString &pageName);
    void logSettingChanged(const QString &settingName, const QString &oldValue, const QString &newValue);
    void logError(const QString &errorMsg);
    void logWarning(const QString &warningMsg);
    void logInfo(const QString &infoMsg);
    
    // 获取日志文件路径
    QString getLogFilePath() const { return logFilePath; }

signals:
    void logMessageAdded(const QString &message);

private:
    OperationLogger();
    ~OperationLogger();
    
    void writeLog(const QString &level, const QString &message);
    QString formatTimestamp();
    
    QString logFilePath;
};

#endif // OPERATION_LOGGER_H
