#include "main_window.h"
#include "serial_port.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QCheckBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , serialPort(std::make_unique<SerialPort>())
{
    setWindowTitle("SCOM-X - Serial Communication Tool");
    setWindowIcon(QIcon(":/icons/app.png"));
    setGeometry(100, 100, 1200, 700);

    // 创建中央窗口
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 应用样式
    applyStyles();

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

    mainLayout->addWidget(leftPanel, 2);

    // ===== 右侧面板 (1/3 宽度) =====
    rightPanel = new QWidget(this);
    rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(5, 10, 10, 10);
    rightLayout->setSpacing(8);

    // 按钮滚动区
    buttonScrollArea = new QScrollArea(this);
    buttonScrollArea->setWidgetResizable(true);
    buttonScrollArea->setStyleSheet("QScrollArea { background-color: #ffffff; }");
    buttonScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    buttonScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 按钮容器
    buttonGroupBox = new QGroupBox("快捷命令", this);
    QVBoxLayout *buttonGroupLayout = new QVBoxLayout(buttonGroupBox);
    buttonGroupLayout->setSpacing(5);

    // 示例快捷按钮
    QStringList buttonLabels = {
        "AT",           "ATI",           "ATE0",          "ATE1",
        "AT+CFUN?",     "AT+COPS?",      "AT+CSQ",        "AT+CREG?",
        "AT+CIMI",      "AT+CGSN",       "AT+CCID",       "AT+CCCID",
        "AT#FWSWITCH=1","AT#FWSWITCH=2", "AT#MONI",       "AT#MONP",
        "AT#MONU",      "AT#MONI RESET", "AT+CGDCONT?",   "AT+CGACT?"
    };

    for (const QString &label : buttonLabels) {
        QPushButton *btn = new QPushButton(label, this);
        btn->setMinimumHeight(35);
        buttonGroupLayout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, [this, label]() {
            sendArea->setText(label);
            onSendDataClicked();
        });
    }

    buttonGroupLayout->addStretch();
    buttonGroupBox->setLayout(buttonGroupLayout);
    buttonScrollArea->setWidget(buttonGroupBox);

    rightLayout->addWidget(buttonScrollArea);
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

    onRefreshPorts();
}

void MainWindow::saveSettings() {
    QSettings settings("SCOM-X", "SCOM-X");
    settings.setValue("baudRate", baudRateSpinBox->value());
    settings.setValue("dataBits", dataBitsComboBox->currentText());
    settings.setValue("parity", parityComboBox->currentText());
    settings.setValue("stopBits", stopBitsComboBox->currentText());
    settings.sync();
}
