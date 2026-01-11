#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <memory>
#include "serial_port.h"

/**
 * @class MainWindow
 * @brief 应用主窗口
 *
 * 提供串口通信的图形界面，包括端口选择、数据发送接收等功能
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow() override;

protected:
    /**
     * @brief 关闭事件处理
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief 连接/断开串口
     */
    void onConnectClicked();

    /**
     * @brief 发送数据
     */
    void onSendClicked();

    /**
     * @brief 清空接收区域
     */
    void onClearReceiveClicked();

    /**
     * @brief 清空发送区域
     */
    void onClearSendClicked();

    /**
     * @brief 扫描串口
     */
    void onRefreshPorts();

    /**
     * @brief 处理数据接收
     */
    void onDataReceived(const QString &data, SerialPort::DataFormat format);

    /**
     * @brief 处理数据发送
     */
    void onDataSent(const QString &data, SerialPort::DataFormat format);

    /**
     * @brief 处理错误
     */
    void onError(const QString &error);

    /**
     * @brief 处理连接状态改变
     */
    void onConnectionStatusChanged(bool isOpen);

    /**
     * @brief 自动滚动到底部
     */
    void onReceiveTextChanged();

private:
    /**
     * @brief 初始化用户界面
     */
    void setupUI();

    /**
     * @brief 应用样式表
     */
    void applyStyles();

    /**
     * @brief 连接信号和槽
     */
    void connectSignals();

    /**
     * @brief 加载设置
     */
    void loadSettings();

    /**
     * @brief 保存设置
     */
    void saveSettings();

    /**
     * @brief 更新连接状态UI
     */
    void updateConnectionUI(bool isConnected);

    /**
     * @brief 添加接收日志
     */
    void addReceiveLog(const QString &data, const QString &format);

    /**
     * @brief 添加发送日志
     */
    void addSendLog(const QString &data, const QString &format);

    // UI 组件
    QWidget *m_centralWidget = nullptr;

    // 串口参数配置区
    QComboBox *m_portComboBox = nullptr;
    QSpinBox *m_baudRateSpinBox = nullptr;
    QComboBox *m_dataBitsComboBox = nullptr;
    QComboBox *m_stopBitsComboBox = nullptr;
    QComboBox *m_parityComboBox = nullptr;
    QComboBox *m_flowControlComboBox = nullptr;

    // 数据格式选择
    QComboBox *m_sendFormatComboBox = nullptr;
    QCheckBox *m_addNewlineCheckBox = nullptr;

    // 按钮
    QPushButton *m_connectButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QPushButton *m_sendButton = nullptr;
    QPushButton *m_clearSendButton = nullptr;
    QPushButton *m_clearReceiveButton = nullptr;

    // 文本编辑区
    QPlainTextEdit *m_sendTextEdit = nullptr;
    QTextEdit *m_receiveTextEdit = nullptr;

    // 状态标签
    QLabel *m_statusLabel = nullptr;
    QLabel *m_bytesReceivedLabel = nullptr;
    QLabel *m_bytesSentLabel = nullptr;

    // 串口对象
    std::unique_ptr<SerialPort> m_serialPort;

    // 统计变量
    qint64 m_bytesReceived = 0;
    qint64 m_bytesSent = 0;
    bool m_isConnected = false;
};

#endif // MAIN_WINDOW_H
