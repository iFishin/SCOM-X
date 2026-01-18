#include "config_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ConfigManager::ConfigManager() : configPath("") {
    initializeDefaults();
}

ConfigManager::~ConfigManager() {
    // 析构时自动保存配置
    saveConfig();
}

bool ConfigManager::initialize(const QString &path) {
    // 确定配置文件路径
    if (path.isEmpty()) {
        // 使用应用数据目录
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dataPath);
        configPath = dataPath + "/config.json";
    } else {
        configPath = path;
        QDir().mkpath(QFileInfo(path).absolutePath());
    }

    qDebug() << "[ConfigManager] Configuration path:" << configPath;

    // 尝试加载现有配置
    if (QFile::exists(configPath)) {
        return loadConfig();
    } else {
        // 首次运行，使用默认配置并保存
        initializeDefaults();
        return saveConfig();
    }
}

bool ConfigManager::loadConfig() {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ConfigManager] Failed to open config file:" << configPath;
        initializeDefaults();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        qWarning() << "[ConfigManager] Invalid JSON config file";
        initializeDefaults();
        return false;
    }

    configData = doc.object();
    qDebug() << "[ConfigManager] Config loaded successfully";
    return true;
}

bool ConfigManager::saveConfig() {
    if (configPath.isEmpty()) {
        qWarning() << "[ConfigManager] Config path not initialized";
        return false;
    }

    // 更新配置对象
    configData["serial"] = getSerialConfig();
    configData["ui"] = getUIConfig();

    QJsonDocument doc(configData);
    QFile file(configPath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "[ConfigManager] Failed to write config file:" << configPath;
        return false;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "[ConfigManager] Config saved successfully";
    return true;
}

void ConfigManager::initializeDefaults() {
    configData = QJsonObject();

    // 默认串口配置
    QJsonObject serialConfig;
    serialConfig["port"] = "COM1";
    serialConfig["baudRate"] = 115200;
    serialConfig["dataBits"] = 8;
    serialConfig["parity"] = "None";
    serialConfig["stopBits"] = "1";
    serialConfig["dtr"] = false;
    serialConfig["rts"] = false;
    serialConfig["flowControl"] = "None";  // None, RTS/CTS, XON/XOFF
    serialConfig["commandRows"] = 100;

    configData["serial"] = serialConfig;

    // 默认 UI 配置
    QJsonObject uiConfig;
    uiConfig["hexMode"] = false;
    uiConfig["windowWidth"] = 1200;
    uiConfig["windowHeight"] = 800;
    uiConfig["lineEndIndex"] = 0;  // 默认 0D0A (CRLF)
    uiConfig["terminalHistory"] = QJsonArray();  // 空的终端历史

    configData["ui"] = uiConfig;
}

// Getters - 串口配置
QString ConfigManager::getSerialPort() const {
    return configData["serial"].toObject()["port"].toString("COM1");
}

int ConfigManager::getBaudRate() const {
    return configData["serial"].toObject()["baudRate"].toInt(115200);
}

int ConfigManager::getDataBits() const {
    return configData["serial"].toObject()["dataBits"].toInt(8);
}

QString ConfigManager::getParity() const {
    return configData["serial"].toObject()["parity"].toString("None");
}

QString ConfigManager::getStopBits() const {
    return configData["serial"].toObject()["stopBits"].toString("1");
}

bool ConfigManager::getDTR() const {
    return configData["serial"].toObject()["dtr"].toBool(false);
}

bool ConfigManager::getRTS() const {
    return configData["serial"].toObject()["rts"].toBool(false);
}

QString ConfigManager::getFlowControl() const {
    return configData["serial"].toObject()["flowControl"].toString("None");
}

int ConfigManager::getCommandRows() const {
    return configData["serial"].toObject()["commandRows"].toInt(100);
}

// Getters - UI 配置
bool ConfigManager::getHexMode() const {
    return configData["ui"].toObject()["hexMode"].toBool(false);
}

int ConfigManager::getWindowWidth() const {
    return configData["ui"].toObject()["windowWidth"].toInt(1200);
}

int ConfigManager::getWindowHeight() const {
    return configData["ui"].toObject()["windowHeight"].toInt(800);
}

int ConfigManager::getLineEndIndex() const {
    return configData["ui"].toObject()["lineEndIndex"].toInt(0);  // 默认 0 (0D0A)
}

QStringList ConfigManager::getTerminalHistory() const {
    QJsonArray historyArray = configData["ui"].toObject()["terminalHistory"].toArray();
    QStringList history;
    
    for (const QJsonValue &value : historyArray) {
        history.append(value.toString());
    }
    
    return history;
}

void ConfigManager::setTerminalHistory(const QStringList &history) {
    QJsonArray historyArray;
    for (const QString &cmd : history) {
        historyArray.append(cmd);
    }
    
    QJsonObject ui = configData["ui"].toObject();
    ui["terminalHistory"] = historyArray;
    configData["ui"] = ui;
}

// Setters - 串口配置
void ConfigManager::setSerialPort(const QString &port) {
    QJsonObject serial = configData["serial"].toObject();
    serial["port"] = port;
    configData["serial"] = serial;
}

void ConfigManager::setBaudRate(int rate) {
    QJsonObject serial = configData["serial"].toObject();
    serial["baudRate"] = rate;
    configData["serial"] = serial;
}

void ConfigManager::setDataBits(int bits) {
    QJsonObject serial = configData["serial"].toObject();
    serial["dataBits"] = bits;
    configData["serial"] = serial;
}

void ConfigManager::setParity(const QString &parity) {
    QJsonObject serial = configData["serial"].toObject();
    serial["parity"] = parity;
    configData["serial"] = serial;
}

void ConfigManager::setStopBits(const QString &stopBits) {
    QJsonObject serial = configData["serial"].toObject();
    serial["stopBits"] = stopBits;
    configData["serial"] = serial;
}

void ConfigManager::setDTR(bool enabled) {
    QJsonObject serial = configData["serial"].toObject();
    serial["dtr"] = enabled;
    configData["serial"] = serial;
}

void ConfigManager::setRTS(bool enabled) {
    QJsonObject serial = configData["serial"].toObject();
    serial["rts"] = enabled;
    configData["serial"] = serial;
}

void ConfigManager::setFlowControl(const QString &flowControl) {
    QJsonObject serial = configData["serial"].toObject();
    serial["flowControl"] = flowControl;
    configData["serial"] = serial;
}

void ConfigManager::setCommandRows(int rows) {
    QJsonObject serial = configData["serial"].toObject();
    serial["commandRows"] = rows;
    configData["serial"] = serial;
}

// Setters - UI 配置
void ConfigManager::setHexMode(bool enabled) {
    QJsonObject ui = configData["ui"].toObject();
    ui["hexMode"] = enabled;
    configData["ui"] = ui;
}

void ConfigManager::setWindowSize(int width, int height) {
    QJsonObject ui = configData["ui"].toObject();
    ui["windowWidth"] = width;
    ui["windowHeight"] = height;
    configData["ui"] = ui;
}

void ConfigManager::setLineEndIndex(int index) {
    QJsonObject ui = configData["ui"].toObject();
    ui["lineEndIndex"] = index;
    configData["ui"] = ui;
}

// Private helpers
QJsonObject ConfigManager::getSerialConfig() const {
    return configData["serial"].toObject();
}

QJsonObject ConfigManager::getUIConfig() const {
    return configData["ui"].toObject();
}
