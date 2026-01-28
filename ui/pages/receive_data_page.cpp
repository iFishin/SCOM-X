#include "receive_data_page.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDebug>

ReceiveDataPage::ReceiveDataPage(
    QPlainTextEdit *mainReceiveArea,
    QComboBox *mainTerminalInput,
    QCheckBox *mainTerminalHexMode,
    QComboBox *mainLineEndComboBox,
    QWidget *parent)
    : QWidget(parent),
      mainReceiveArea(mainReceiveArea),
      mainTerminalInput(mainTerminalInput),
      mainTerminalHexMode(mainTerminalHexMode),
      mainLineEndComboBox(mainLineEndComboBox)
{
    setupUI();
    connectSignals();
}

ReceiveDataPage::~ReceiveDataPage()
{
}

void ReceiveDataPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // 创建 Received Data 分组框
    receivedDataGroupBox = new QGroupBox("Received Data", this);
    QVBoxLayout *groupLayout = new QVBoxLayout(receivedDataGroupBox);
    
    // 创建接收区域的副本
    receiveArea = new QPlainTextEdit(this);
    receiveArea->setReadOnly(true);
    groupLayout->addWidget(receiveArea);
    
    // 创建终端输入行
    QHBoxLayout *terminalInputLayout = new QHBoxLayout();
    terminalPrompt = new QLabel(">", this);
    terminalPrompt->setMaximumWidth(30);
    terminalInputLayout->addWidget(terminalPrompt);
    
    // 创建终端输入框的副本
    terminalInput = new QComboBox(this);
    terminalInput->setEditable(true);
    terminalInput->setInsertPolicy(QComboBox::InsertPolicy::InsertAtTop);
    terminalInputLayout->addWidget(terminalInput);
    groupLayout->addLayout(terminalInputLayout);
    
    // 创建终端控制行
    QHBoxLayout *terminalControlLayout = new QHBoxLayout();
    
    // 创建 HEX 模式复选框的副本
    terminalHexMode = new QCheckBox("HEX", this);
    terminalControlLayout->addWidget(terminalHexMode);
    
    // 创建行尾符标签和下拉框
    lineEndLabel = new QLabel("Line End:", this);
    terminalControlLayout->addWidget(lineEndLabel);
    
    lineEndComboBox = new QComboBox(this);
    lineEndComboBox->addItem("0D0A (CRLF)");
    lineEndComboBox->addItem("0A (LF)");
    lineEndComboBox->addItem("0D (CR)");
    lineEndComboBox->addItem("None");
    terminalControlLayout->addWidget(lineEndComboBox);
    
    // 添加伸缩器
    terminalControlLayout->addStretch();
    
    groupLayout->addLayout(terminalControlLayout);
    
    // 将分组框添加到主布局
    mainLayout->addWidget(receivedDataGroupBox);
    
    setLayout(mainLayout);
}

void ReceiveDataPage::connectSignals()
{
    if (!receiveArea || !terminalInput) {
        qWarning() << "[ReceiveDataPage] Failed to connect signals: null pointer";
        return;
    }
    
    // 连接本页面终端输入框的回车事件
    if (terminalInput->lineEdit()) {
        connect(terminalInput->lineEdit(), &QLineEdit::returnPressed, this, [this]() {
            QString command = terminalInput->lineEdit()->text().trimmed();
            if (!command.isEmpty()) {
                emit terminalCommandEntered(command);
            }
        });
    }
    
    qDebug() << "[ReceiveDataPage] Page initialized";
}

void ReceiveDataPage::syncReceiveAreaFromMain(const QString &text)
{
    if (receiveArea) {
        receiveArea->setPlainText(text);
        receiveArea->moveCursor(receiveArea->textCursor().MoveOperation::End);
    }
}

void ReceiveDataPage::syncTerminalInputFromMain(const QString &text)
{
    if (terminalInput && terminalInput->lineEdit()) {
        terminalInput->lineEdit()->setText(text);
    }
}

void ReceiveDataPage::syncTerminalHexModeFromMain(bool checked)
{
    if (terminalHexMode) {
        terminalHexMode->setChecked(checked);
    }
}

void ReceiveDataPage::syncLineEndFromMain(int index)
{
    if (lineEndComboBox) {
        lineEndComboBox->setCurrentIndex(index);
    }
}
