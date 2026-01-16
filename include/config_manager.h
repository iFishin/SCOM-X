#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @class ConfigManager
 * @brief 管理应用配置的 JSON 文件读写
 * 
 * 负责读取和保存所有应用配置信息到 JSON 文件
 * 包括串口配置、UI 状态等
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    // 初始化配置文件
    bool initialize(const QString &configPath = "");

    // 获取配置值
    QString getSerialPort() const;
    int getBaudRate() const;
    int getDataBits() const;
    QString getParity() const;
    QString getStopBits() const;
    bool getDTR() const;
    bool getRTS() const;
    QString getFlowControl() const;  // RTS/CTS, XON/XOFF, None
    int getCommandRows() const;

    // 获取 UI 状态
    bool getHexMode() const;
    int getWindowWidth() const;
    int getWindowHeight() const;
    int getLineEndIndex() const;

    // 设置配置值
    void setSerialPort(const QString &port);
    void setBaudRate(int rate);
    void setDataBits(int bits);
    void setParity(const QString &parity);
    void setStopBits(const QString &stopBits);
    void setDTR(bool enabled);
    void setRTS(bool enabled);
    void setFlowControl(const QString &flowControl);
    void setCommandRows(int rows);

    // 设置 UI 状态
    void setHexMode(bool enabled);
    void setWindowSize(int width, int height);
    void setLineEndIndex(int index);

    // 保存配置到文件
    bool saveConfig();

    // 重新加载配置文件
    bool loadConfig();

private:
    QString configPath;
    QJsonObject configData;

    // 初始化默认配置
    void initializeDefaults();

    // 获取序列化的配置 JSON
    QJsonObject getSerialConfig() const;
    QJsonObject getUIConfig() const;
};

#endif // CONFIG_MANAGER_H
