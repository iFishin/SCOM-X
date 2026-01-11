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
#include <QGridLayout>
#include <QLineEdit>
#include <memory>
#include <vector>

class SerialPort;

/**
 * @class MainWindow
 * @brief SCOM-X 主窗口类
 * 
 * 提供串口通信的图形用户界面，采用左右分屏布局：
 * - 左侧：配置、命令、接收数据区、热键
 * - 右侧：可滚动的快捷指令表格
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
    
    // 快捷指令槽
    void onQuickCommandButtonClicked(int index);
    void onQuickCommandReturnPressed(int index);
    
    // 热键槽
    void onHotkeyClicked(int index);
    
    // 菜单槽
    void onSettingsAction();
    void onAboutAction();
    void onPreferencesClicked();
    
    // 串口信号处理
    void onConnectionStatusChanged(bool connected);
    void onSerialError(const QString &errorMsg);

    // 设置相关槽
    void onSettingChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupQuickCommands();
    void setupHotkeys();
    void connectSignals();
    void rebuildCommandTable(int rowCount);
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

    // 热键区 (Hotkeys GroupBox)
    QGroupBox *hotkeysGroupBox;
    QGridLayout *hotkeysLayout;
    std::vector<QPushButton*> hotkeyButtons;

    // ===== 右侧面板组件 =====
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    
    // 快捷指令组（可滚动表格）
    QScrollArea *commandScrollArea;
    QGroupBox *commandTableGroupBox;
    QGridLayout *commandTableLayout;
    
    // 快捷指令行数据
    std::vector<QCheckBox*> commandCheckboxes;
    std::vector<QPushButton*> commandButtons;
    std::vector<QLineEdit*> commandInputs;
    std::vector<QCheckBox*> commandHexCheckboxes;
    std::vector<QCheckBox*> commandEndCheckboxes;
    std::vector<QLineEdit*> commandIntervals;
    
    // 快捷指令行数设置
    int currentCommandRows = 100;  // 当前行数
    int maxCommandRows = 300;       // 最大行数

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
