#ifndef RECEIVE_DATA_PAGE_H
#define RECEIVE_DATA_PAGE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>

/**
 * @class ReceiveDataPage
 * @brief 接收数据显示页面
 * 
 * 专门用于显示串口接收数据的独立页面
 * 创建与主页面相同的组件副本，通过信号槽实现双向同步
 */
class ReceiveDataPage : public QWidget {
    Q_OBJECT

public:
    explicit ReceiveDataPage(
        QPlainTextEdit *mainReceiveArea,
        QComboBox *mainTerminalInput,
        QCheckBox *mainTerminalHexMode,
        QComboBox *mainLineEndComboBox,
        QWidget *parent = nullptr
    );
    ~ReceiveDataPage();

    // 获取本页面的组件引用，便于数据同步
    QPlainTextEdit* getReceiveArea() const { return receiveArea; }
    QComboBox* getTerminalInput() const { return terminalInput; }
    QCheckBox* getTerminalHexMode() const { return terminalHexMode; }
    QComboBox* getLineEndComboBox() const { return lineEndComboBox; }

signals:
    // 当用户在此页面的终端输入框按回车时发出
    void terminalCommandEntered(const QString &command);

public slots:
    // 同步主页面的数据到此页面
    void syncReceiveAreaFromMain(const QString &text);
    void syncTerminalInputFromMain(const QString &text);
    void syncTerminalHexModeFromMain(bool checked);
    void syncLineEndFromMain(int index);

private:
    void setupUI();
    void connectSignals();
    
    // 本页面独立的组件
    QPlainTextEdit *receiveArea = nullptr;
    QComboBox *terminalInput = nullptr;
    QCheckBox *terminalHexMode = nullptr;
    QComboBox *lineEndComboBox = nullptr;
    
    // 主页面的组件指针（用于反向同步）
    QPlainTextEdit *mainReceiveArea = nullptr;
    QComboBox *mainTerminalInput = nullptr;
    QCheckBox *mainTerminalHexMode = nullptr;
    QComboBox *mainLineEndComboBox = nullptr;
    
    // UI 组件
    QGroupBox *receivedDataGroupBox = nullptr;
    QLabel *terminalPrompt = nullptr;
    QLabel *lineEndLabel = nullptr;
};

#endif // RECEIVE_DATA_PAGE_H
