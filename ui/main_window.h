#ifndef SCOM_UI_MAIN_WINDOW_H
#define SCOM_UI_MAIN_WINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QStatusBar>
#include <QScrollArea>
#include <QGroupBox>
#include <QVBoxLayout>
#include <memory>

class SerialPort;

/**
 * @class MainWindow
 * @brief SCOM-X 主窗口类
 * 
 * 提供串口通信的图形用户界面，采用左右分屏布局：
 * - 左侧：配置、命令、接收数据区
 * - 右侧：可滚动的快捷按钮组
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 串口相关槽
    void onConnectClicked();
    void onDisconnectClicked();
    void onSendDataClicked();
    void onClearReceiveArea();
    void onClearSendArea();
    void onRefreshPorts();
    
    // 串口信号处理
    void onConnectionStatusChanged(bool connected);
    void onSerialError(const QString &errorMsg);

    // 设置相关槽
    void onSettingChanged();

private:
    void setupUI();
    void setupMenuBar();
    void connectSignals();
    void applyStyles();
    void loadSettings();
    void saveSettings();
    void updateConnectionStatus(bool connected);

    // 菜单和工具栏
    QWidget *centralWidget;
    
    // ===== 左侧面板组件 =====
    QWidget *leftPanel;
    QVBoxLayout *leftLayout;

    // 串口配置区 (Settings GroupBox)
    QGroupBox *settingsGroupBox;
    QComboBox *portComboBox;
    QSpinBox *baudRateSpinBox;
    QComboBox *dataBitsComboBox;
    QComboBox *parityComboBox;
    QComboBox *stopBitsComboBox;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;
    QLabel *statusLabel;

    // 命令发送区 (Command GroupBox)
    QGroupBox *commandGroupBox;
    QTextEdit *sendArea;
    QCheckBox *hexModeCheckBox;
    QPushButton *sendButton;
    QPushButton *clearSendButton;

    // 数据接收区 (Received Data GroupBox)
    QGroupBox *receivedDataGroupBox;
    QTextEdit *receiveArea;
    QPushButton *clearReceiveButton;

    // ===== 右侧面板组件 =====
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    
    // 快捷按钮组（可滚动）
    QScrollArea *buttonScrollArea;
    QGroupBox *buttonGroupBox;
    QVBoxLayout *buttonGroupLayout;

    // 底部状态栏
    QLabel *connectionStatusLabel;
    QLabel *bytesReceivedLabel;
    QLabel *bytesSentLabel;
    
    // 串口对象
    std::unique_ptr<SerialPort> serialPort;
    
    // 统计数据
    int bytesReceived = 0;
    int bytesSent = 0;
};

#endif // SCOM_UI_MAIN_WINDOW_H
