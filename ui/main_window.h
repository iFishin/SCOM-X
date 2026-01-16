#ifndef SCOM_UI_MAIN_WINDOW_H
#define SCOM_UI_MAIN_WINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <memory>
#include <vector>

class SerialPort;

/**
 * @class TerminalTextEdit
 * @brief 支持终端风格快捷键的文本编辑器
 * 
 * 支持快捷键：
 * - Ctrl+K: 清空所有内容
 * - Ctrl+Z: 撤销
 * - Ctrl+Y: 重做
 * - Return/Enter: 发送指令信号
 */
class TerminalTextEdit : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit TerminalTextEdit(QWidget *parent = nullptr);

signals:
    void commandEntered(const QString &command);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString getLastLine() const;
    void removeLastLine();
};

// 前向声明 UI 类（由 Qt 自动生成）
namespace Ui {
    class MainWindow;
}

/**
 * @class MainWindow
 * @brief SCOM-X 主窗口类
 * 
 * 主窗口类使用 Qt Designer 生成的 UI 文件（main_window.ui）
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
    void setupDynamicUI();
    void connectSignals();
    void rebuildCommandTable(int rowCount);
    void applyStyles();
    void loadSettings();
    void saveSettings();
    void updateConnectionStatus(bool connected);

    // UI 类指针（由 Qt 自动生成的 ui_main_window.h）
    std::unique_ptr<Ui::MainWindow> ui;

    // 动态创建的快捷指令组件（不在 UI 文件中定义）
    std::vector<QCheckBox*> commandCheckboxes;
    std::vector<QPushButton*> commandButtons;
    std::vector<QLineEdit*> commandInputs;
    std::vector<QCheckBox*> commandHexCheckboxes;
    std::vector<QCheckBox*> commandEndCheckboxes;
    std::vector<QLineEdit*> commandIntervals;
    
    // 快捷指令行数设置
    int currentCommandRows = 100;   // 当前行数
    int maxCommandRows = 300;       // 最大行数

    // 统计数据
    int bytesReceived = 0;
    int bytesSent = 0;

    // 串口对象
    std::unique_ptr<SerialPort> serialPort;
};

#endif // SCOM_UI_MAIN_WINDOW_H

