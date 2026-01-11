#include "main_window.h"
#include "serial_port.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QIntValidator>
#include <QSpacerItem>
#include <QDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , serialPort(std::make_unique<SerialPort>())
{
    setWindowTitle("SCOM-X - Serial Communication Tool");
    setWindowIcon(QIcon(":/icons/app.png"));
    setGeometry(100, 100, 1400, 800);

    // 创建中央窗口
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 应用样式
    applyStyles();

    // 创建菜单栏
    setupMenuBar();

    // 构建 UI
    setupUI();

    // 连接信号槽
    connectSignals();

    // 加载之前保存的设置
    loadSettings();

    // 创建状态栏
    connectionStatusLabel = new QLabel("已断开连接", this);
    bytesReceivedLabel = new QLabel("接收: 0 字节", this);
    bytesSentLabel = new QLabel("发送: 0 字节", this);
    
    statusBar()->addWidget(connectionStatusLabel);
    statusBar()->addPermanentWidget(bytesReceivedLabel);
    statusBar()->addPermanentWidget(bytesSentLabel);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenuBar() {
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    // 设置菜单
    QMenu *settingsMenu = menuBar()->addMenu("&Settings");
    QAction *preferencesAction = settingsMenu->addAction("&Preferences");
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::onPreferencesClicked);
    settingsMenu->addSeparator();
    QAction *exitAction = settingsMenu->addAction("&Exit");
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    
    // 工具菜单
    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    QAction *clearHistoryAction = toolsMenu->addAction("&Clear History");
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutAction);
}

void MainWindow::setupUI() {
    // 创建主容器和布局
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== 左侧面板 (2/3 宽度) =====
    leftPanel = new QWidget(this);
    leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 10, 5, 10);
    leftLayout->setSpacing(8);

    // --- 串口配置区 ---
    settingsGroupBox = new QGroupBox("串口配置", this);
    QGridLayout *settingsLayout = new QGridLayout(settingsGroupBox);

    settingsLayout->addWidget(new QLabel("端口:", this), 0, 0);
    portComboBox = new QComboBox(this);
    portComboBox->setMinimumWidth(100);
    settingsLayout->addWidget(portComboBox, 0, 1);

    refreshButton = new QPushButton("刷新", this);
    refreshButton->setMaximumWidth(60);
    settingsLayout->addWidget(refreshButton, 0, 2);

    settingsLayout->addWidget(new QLabel("波特率:", this), 1, 0);
    baudRateSpinBox = new QSpinBox(this);
    baudRateSpinBox->setMinimum(300);
    baudRateSpinBox->setMaximum(921600);
    baudRateSpinBox->setValue(115200);
    settingsLayout->addWidget(baudRateSpinBox, 1, 1, 1, 2);

    settingsLayout->addWidget(new QLabel("数据位:", this), 2, 0);
    dataBitsComboBox = new QComboBox(this);
    dataBitsComboBox->addItems({"5", "6", "7", "8"});
    dataBitsComboBox->setCurrentText("8");
    settingsLayout->addWidget(dataBitsComboBox, 2, 1, 1, 2);

    settingsLayout->addWidget(new QLabel("校验位:", this), 3, 0);
    parityComboBox = new QComboBox(this);
    parityComboBox->addItems({"None", "Odd", "Even"});
    settingsLayout->addWidget(parityComboBox, 3, 1, 1, 2);

    settingsLayout->addWidget(new QLabel("停止位:", this), 4, 0);
    stopBitsComboBox = new QComboBox(this);
    stopBitsComboBox->addItems({"1", "1.5", "2"});
    settingsLayout->addWidget(stopBitsComboBox, 4, 1, 1, 2);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    connectButton = new QPushButton("连接", this);
    disconnectButton = new QPushButton("断开", this);
    disconnectButton->setEnabled(false);
    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(disconnectButton);
    settingsLayout->addLayout(buttonLayout, 5, 0, 1, 3);

    leftLayout->addWidget(settingsGroupBox);

    // --- 发送命令区 ---
    commandGroupBox = new QGroupBox("发送命令", this);
    QVBoxLayout *commandLayout = new QVBoxLayout(commandGroupBox);

    sendArea = new QTextEdit(this);
    sendArea->setPlaceholderText("输入要发送的数据...");
    sendArea->setMaximumHeight(80);
    commandLayout->addWidget(sendArea);

    hexModeCheckBox = new QCheckBox("以十六进制形式发送", this);
    commandLayout->addWidget(hexModeCheckBox);

    sendButton = new QPushButton("发送", this);
    commandLayout->addWidget(sendButton);

    leftLayout->addWidget(commandGroupBox);

    // --- 热键区 ---
    hotkeysGroupBox = new QGroupBox("热键 (F1-F4)", this);
    hotkeysLayout = new QGridLayout(hotkeysGroupBox);
    hotkeysLayout->setSpacing(5);
    
    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton(QString("F%1").arg(i+1), this);
        btn->setMinimumHeight(35);
        hotkeyButtons.push_back(btn);
        hotkeysLayout->addWidget(btn, i / 4, i % 4);
        connect(btn, &QPushButton::clicked, this, [this, i]() { onHotkeyClicked(i); });
    }
    
    leftLayout->addWidget(hotkeysGroupBox);

    // --- 接收数据区 ---
    receivedDataGroupBox = new QGroupBox("接收数据", this);
    QVBoxLayout *receivedLayout = new QVBoxLayout(receivedDataGroupBox);

    receiveArea = new QTextEdit(this);
    receiveArea->setReadOnly(true);
    receiveArea->setPlaceholderText("接收到的数据将显示在这里...");
    receivedLayout->addWidget(receiveArea);

    QHBoxLayout *clearLayout = new QHBoxLayout();
    clearReceiveButton = new QPushButton("清空接收", this);
    clearSendButton = new QPushButton("清空发送", this);
    clearLayout->addWidget(clearReceiveButton);
    clearLayout->addWidget(clearSendButton);
    receivedLayout->addLayout(clearLayout);

    leftLayout->addWidget(receivedDataGroupBox, 1);

    mainLayout->addWidget(leftPanel, 1);

    // ===== 右侧面板 (1/2 宽度) =====
    rightPanel = new QWidget(this);
    rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(5, 10, 10, 10);
    rightLayout->setSpacing(8);

    // 快捷指令滚动区
    commandScrollArea = new QScrollArea(this);
    commandScrollArea->setWidgetResizable(true);
    commandScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    commandScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    commandScrollArea->setFrameShape(QFrame::NoFrame);
    commandScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    // 快捷指令容器
    commandTableGroupBox = new QGroupBox("快捷指令", this);
    commandTableLayout = new QGridLayout(commandTableGroupBox);
    commandTableLayout->setSpacing(5);
    commandTableLayout->setContentsMargins(5, 5, 5, 5);
    
    // 表头
    commandTableLayout->addWidget(new QLabel(""), 0, 0, Qt::AlignCenter);
    commandTableLayout->addWidget(new QLabel("按钮"), 0, 1, Qt::AlignCenter);
    commandTableLayout->addWidget(new QLabel("数据"), 0, 2, Qt::AlignCenter);
    commandTableLayout->addWidget(new QLabel("HEX"), 0, 3, Qt::AlignCenter);
    commandTableLayout->addWidget(new QLabel("加\n回\n车"), 0, 4, Qt::AlignCenter);
    commandTableLayout->addWidget(new QLabel("间隔\n(ms)"), 0, 5, Qt::AlignCenter);
    
    // 使用动态行数创建表格
    rebuildCommandTable(currentCommandRows);
    
    commandScrollArea->setWidget(commandTableGroupBox);
    rightLayout->addWidget(commandScrollArea);
    mainLayout->addWidget(rightPanel, 1);
}

void MainWindow::connectSignals() {
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendDataClicked);
    connect(clearReceiveButton, &QPushButton::clicked, this, &MainWindow::onClearReceiveArea);
    connect(clearSendButton, &QPushButton::clicked, this, &MainWindow::onClearSendArea);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshPorts);

    connect(portComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onSettingChanged);
    connect(baudRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onSettingChanged);

    if (serialPort) {
        // 连接串口信号
        connect(serialPort.get(), QOverload<const QString&, SerialPort::DataFormat>::of(&SerialPort::dataReceived),
                this, [this](const QString &data, SerialPort::DataFormat format) {
                    receiveArea->append(data);
                    bytesReceivedLabel->setText(QString("接收: %1 字节").arg(
                        receiveArea->toPlainText().length()));
                });
        
        connect(serialPort.get(), QOverload<const QString&, SerialPort::DataFormat>::of(&SerialPort::dataSent),
                this, [this](const QString &data, SerialPort::DataFormat format) {
                    bytesSentLabel->setText(QString("发送: %1 字节").arg(
                        bytesSentLabel->text().remove(0, 3).toInt() + data.length()));
                });
        
        connect(serialPort.get(), &SerialPort::connectionStatusChanged,
                this, &MainWindow::onConnectionStatusChanged);
        
        connect(serialPort.get(), &SerialPort::errorOccurred,
                this, &MainWindow::onSerialError);
    }
}

void MainWindow::applyStyles() {
    QFile styleFile(":/ui/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(style);
        styleFile.close();
        qDebug() << "✓ 样式文件加载成功，文件大小:" << style.size() << "字符";
    } else {
        qDebug() << "✗ 无法加载样式文件: :/ui/style.qss";
        qDebug() << "可用资源:";
        // 列出可用资源进行调试
        QDir dir(":/ui");
        qDebug() << dir.entryList();
    }
}

void MainWindow::onConnectClicked() {
    if (!serialPort) return;
    
    QString portName = portComboBox->currentText();
    int baudRate = baudRateSpinBox->value();
    
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "错误", "请选择一个串口");
        return;
    }
    
    if (serialPort->open(portName, baudRate)) {
        updateConnectionStatus(true);
    } else {
        QMessageBox::critical(this, "错误", "无法打开串口: " + portName);
    }
}

void MainWindow::onDisconnectClicked() {
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        updateConnectionStatus(false);
    }
}

void MainWindow::onSendDataClicked() {
    if (!serialPort || !serialPort->isOpen()) {
        QMessageBox::warning(this, "错误", "串口未连接");
        return;
    }
    
    QString data = sendArea->toPlainText();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入要发送的数据");
        return;
    }
    
    if (hexModeCheckBox->isChecked()) {
        serialPort->write(data, SerialPort::DataFormat::HEX);
    } else {
        serialPort->write(data, SerialPort::DataFormat::ASCII);
    }
}

void MainWindow::onClearReceiveArea() {
    receiveArea->clear();
    bytesReceivedLabel->setText("接收: 0 字节");
}

void MainWindow::onClearSendArea() {
    sendArea->clear();
}

void MainWindow::onRefreshPorts() {
    portComboBox->clear();
    QStringList ports = SerialPort::scanAvailablePorts();
    portComboBox->addItems(ports);
}

void MainWindow::onConnectionStatusChanged(bool connected) {
    updateConnectionStatus(connected);
}

void MainWindow::onSerialError(const QString &errorMsg) {
    QMessageBox::critical(this, "串口错误", errorMsg);
}

void MainWindow::onSettingChanged() {
    // 设置变化时可以添加其他逻辑
}

void MainWindow::updateConnectionStatus(bool connected) {
    if (connected) {
        connectionStatusLabel->setText("已连接");
        connectionStatusLabel->setStyleSheet("color: #00a86b; font-weight: bold;");
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(true);
        sendButton->setEnabled(true);
        portComboBox->setEnabled(false);
        baudRateSpinBox->setEnabled(false);
    } else {
        connectionStatusLabel->setText("已断开连接");
        connectionStatusLabel->setStyleSheet("color: #6c757d; font-weight: bold;");
        connectButton->setEnabled(true);
        disconnectButton->setEnabled(false);
        sendButton->setEnabled(false);
        portComboBox->setEnabled(true);
        baudRateSpinBox->setEnabled(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings() {
    QSettings settings("SCOM-X", "SCOM-X");
    
    int baudRate = settings.value("baudRate", 115200).toInt();
    baudRateSpinBox->setValue(baudRate);
    
    QString dataBits = settings.value("dataBits", "8").toString();
    dataBitsComboBox->setCurrentText(dataBits);
    
    QString parity = settings.value("parity", "None").toString();
    parityComboBox->setCurrentText(parity);
    
    QString stopBits = settings.value("stopBits", "1").toString();
    stopBitsComboBox->setCurrentText(stopBits);
    
    // 加载快捷指令行数设置
    currentCommandRows = settings.value("commandRows", 10).toInt();
    if (currentCommandRows < 1) currentCommandRows = 1;
    if (currentCommandRows > maxCommandRows) currentCommandRows = maxCommandRows;

    onRefreshPorts();
}

void MainWindow::saveSettings() {
    QSettings settings("SCOM-X", "SCOM-X");
    settings.setValue("baudRate", baudRateSpinBox->value());
    settings.setValue("dataBits", dataBitsComboBox->currentText());
    settings.setValue("parity", parityComboBox->currentText());
    settings.setValue("stopBits", stopBitsComboBox->currentText());
    
    // 保存快捷指令行数
    settings.setValue("commandRows", currentCommandRows);
    
    // 保存快捷指令
    for (int i = 0; i < (int)commandInputs.size(); ++i) {
        settings.setValue(QString("command_%1_data").arg(i), commandInputs[i]->text());
        settings.setValue(QString("command_%1_hex").arg(i), commandHexCheckboxes[i]->isChecked());
        settings.setValue(QString("command_%1_end").arg(i), commandEndCheckboxes[i]->isChecked());
        settings.setValue(QString("command_%1_interval").arg(i), commandIntervals[i]->text());
    }
    
    settings.sync();
}

void MainWindow::onQuickCommandButtonClicked(int index) {
    if (index < (int)commandInputs.size()) {
        QString data = commandInputs[index]->text();
        if (!data.isEmpty()) {
            if (serialPort && serialPort->isOpen()) {
                serialPort->write(data, commandHexCheckboxes[index]->isChecked() ?
                                 SerialPort::DataFormat::HEX :
                                 SerialPort::DataFormat::ASCII);
                bytesSentLabel->setText(QString("发送: %1 字节").arg(data.length()));
            } else {
                QMessageBox::warning(this, "错误", "串口未连接");
            }
        }
    }
}

void MainWindow::onQuickCommandReturnPressed(int index) {
    onQuickCommandButtonClicked(index);
}

void MainWindow::onHotkeyClicked(int index) {
    if (index >= 0 && index < (int)commandInputs.size()) {
        onQuickCommandButtonClicked(index);
    }
}

void MainWindow::onSettingsAction() {
    QMessageBox::information(this, "设置", "设置功能开发中...");
}

void MainWindow::onAboutAction() {
    QMessageBox::about(this, "关于 SCOM-X",
        "SCOM-X - 串口通信工具\n"
        "版本: 1.0.0\n\n"
        "一个功能强大的串口通信工具，支持多种数据格式和快捷指令。");
}

void MainWindow::onPreferencesClicked() {
    // 创建偏好设置对话框
    QDialog prefsDialog(this);
    prefsDialog.setWindowTitle("偏好设置");
    prefsDialog.setFixedSize(400, 200);
    
    QVBoxLayout *layout = new QVBoxLayout(&prefsDialog);
    
    // 快捷指令行数设置
    QHBoxLayout *rowsLayout = new QHBoxLayout();
    rowsLayout->addWidget(new QLabel("快捷指令行数:"));
    QSpinBox *rowsSpinBox = new QSpinBox();
    rowsSpinBox->setMinimum(1);
    rowsSpinBox->setMaximum(maxCommandRows);
    rowsSpinBox->setValue(currentCommandRows);
    rowsLayout->addWidget(rowsSpinBox);
    rowsLayout->addStretch();
    layout->addLayout(rowsLayout);
    
    QLabel *tipsLabel = new QLabel("(1-30 行，修改后立即生效)");
    tipsLabel->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(tipsLabel);
    
    layout->addStretch();
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("确定");
    QPushButton *cancelButton = new QPushButton("取消");
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    connect(okButton, &QPushButton::clicked, &prefsDialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &prefsDialog, &QDialog::reject);
    
    if (prefsDialog.exec() == QDialog::Accepted) {
        int newRows = rowsSpinBox->value();
        if (newRows != currentCommandRows) {
            currentCommandRows = newRows;
            rebuildCommandTable(currentCommandRows);
            saveSettings();
        }
    }
}

void MainWindow::rebuildCommandTable(int rowCount) {
    // 清除现有的数据行（保留表头第0行）
    while (commandTableLayout->rowCount() > 1) {
        for (int col = 0; col < 6; ++col) {
            QLayoutItem *item = commandTableLayout->itemAtPosition(commandTableLayout->rowCount() - 1, col);
            if (item) {
                delete item->widget();
                commandTableLayout->removeItem(item);
            }
        }
    }
    
    // 清除向量中的指针
    for (auto *checkbox : commandCheckboxes) delete checkbox;
    for (auto *button : commandButtons) delete button;
    for (auto *input : commandInputs) delete input;
    for (auto *hexCheckbox : commandHexCheckboxes) delete hexCheckbox;
    for (auto *endCheckbox : commandEndCheckboxes) delete endCheckbox;
    for (auto *interval : commandIntervals) delete interval;
    
    commandCheckboxes.clear();
    commandButtons.clear();
    commandInputs.clear();
    commandHexCheckboxes.clear();
    commandEndCheckboxes.clear();
    commandIntervals.clear();
    
    // 创建新的快捷指令行
    for (int i = 0; i < rowCount; ++i) {
        // 复选框
        QCheckBox *checkbox = new QCheckBox();
        commandCheckboxes.push_back(checkbox);
        commandTableLayout->addWidget(checkbox, i+1, 0, Qt::AlignCenter);
        
        // 发送按钮
        QPushButton *button = new QPushButton(QString("Send %1").arg(i+1));
        button->setMaximumWidth(80);
        commandButtons.push_back(button);
        commandTableLayout->addWidget(button, i+1, 1);
        connect(button, &QPushButton::clicked, this, [this, i]() { onQuickCommandButtonClicked(i); });
        
        // 数据输入框（占据尽可能多的空间）
        QLineEdit *inputField = new QLineEdit();
        inputField->setPlaceholderText("输入数据...");
        commandInputs.push_back(inputField);
        commandTableLayout->addWidget(inputField, i+1, 2);
        connect(inputField, &QLineEdit::returnPressed, this, [this, i]() { onQuickCommandReturnPressed(i); });
        
        // HEX 复选框
        QCheckBox *hexCheckbox = new QCheckBox();
        hexCheckbox->setToolTip("以十六进制发送");
        commandHexCheckboxes.push_back(hexCheckbox);
        commandTableLayout->addWidget(hexCheckbox, i+1, 3, Qt::AlignCenter);
        
        // 加回车复选框
        QCheckBox *endCheckbox = new QCheckBox();
        endCheckbox->setChecked(true);
        endCheckbox->setToolTip("自动添加换行符");
        commandEndCheckboxes.push_back(endCheckbox);
        commandTableLayout->addWidget(endCheckbox, i+1, 4, Qt::AlignCenter);
        
        // 间隔输入框
        QLineEdit *intervalField = new QLineEdit();
        intervalField->setMaximumWidth(50);
        intervalField->setPlaceholderText("0");
        intervalField->setAlignment(Qt::AlignCenter);
        // 只允许数字输入
        QIntValidator *validator = new QIntValidator(0, 99999, this);
        intervalField->setValidator(validator);
        commandIntervals.push_back(intervalField);
        commandTableLayout->addWidget(intervalField, i+1, 5);
    }
    
    // 添加伸缩项
    commandTableLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 
                                rowCount+1, 0, 1, 6);
}
