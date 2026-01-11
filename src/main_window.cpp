#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QStatusBar>
#include <QDateTime>
#include <QSplitter>
#include <QApplication>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_serialPort(std::make_unique<SerialPort>(this))
{
    setWindowTitle("SCOM - Serial Communication Tool");
    setWindowIcon(QIcon(":/icons/app.ico"));
    resize(1000, 700);

    setupUI();
    applyStyles();
    connectSignals();
    loadSettings();

    // 初始扫描端口
    onRefreshPorts();
}

MainWindow::~MainWindow()
{
    saveSettings();
    if (m_serialPort && m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);

    // ============ 串口参数配置区 ============
    QGroupBox *configGroup = new QGroupBox("Serial Port Configuration", this);
    QHBoxLayout *configLayout = new QHBoxLayout(configGroup);

    // 端口选择
    configLayout->addWidget(new QLabel("Port:", this));
    m_portComboBox = new QComboBox(this);
    configLayout->addWidget(m_portComboBox);

    m_refreshButton = new QPushButton("Refresh", this);
    m_refreshButton->setMaximumWidth(80);
    configLayout->addWidget(m_refreshButton);

    configLayout->addSpacing(20);

    // 波特率
    configLayout->addWidget(new QLabel("Baud Rate:", this));
    m_baudRateSpinBox = new QSpinBox(this);
    m_baudRateSpinBox->setMinimum(1200);
    m_baudRateSpinBox->setMaximum(3000000);
    m_baudRateSpinBox->setValue(115200);
    m_baudRateSpinBox->setSingleStep(1);
    configLayout->addWidget(m_baudRateSpinBox);

    configLayout->addSpacing(20);

    // 数据位
    configLayout->addWidget(new QLabel("Data Bits:", this));
    m_dataBitsComboBox = new QComboBox(this);
    m_dataBitsComboBox->addItems({"5", "6", "7", "8"});
    m_dataBitsComboBox->setCurrentText("8");
    configLayout->addWidget(m_dataBitsComboBox);

    configLayout->addSpacing(10);

    // 停止位
    configLayout->addWidget(new QLabel("Stop Bits:", this));
    m_stopBitsComboBox = new QComboBox(this);
    m_stopBitsComboBox->addItems({"1", "1.5", "2"});
    m_stopBitsComboBox->setCurrentText("1");
    configLayout->addWidget(m_stopBitsComboBox);

    configLayout->addSpacing(10);

    // 校验位
    configLayout->addWidget(new QLabel("Parity:", this));
    m_parityComboBox = new QComboBox(this);
    m_parityComboBox->addItems({"None", "Even", "Odd", "Space", "Mark"});
    configLayout->addWidget(m_parityComboBox);

    configLayout->addSpacing(10);

    // 流控制
    configLayout->addWidget(new QLabel("Flow Control:", this));
    m_flowControlComboBox = new QComboBox(this);
    m_flowControlComboBox->addItems({"None", "RTS/CTS", "XON/XOFF"});
    configLayout->addWidget(m_flowControlComboBox);

    configLayout->addStretch();

    // 连接按钮
    m_connectButton = new QPushButton("Connect", this);
    m_connectButton->setMinimumWidth(100);
    m_connectButton->setCheckable(true);
    configLayout->addWidget(m_connectButton);

    mainLayout->addWidget(configGroup);

    // ============ 数据格式和发送区 ============
    QGroupBox *sendGroup = new QGroupBox("Send Data", this);
    QVBoxLayout *sendLayout = new QVBoxLayout(sendGroup);

    // 数据格式和选项
    QHBoxLayout *formatLayout = new QHBoxLayout();
    formatLayout->addWidget(new QLabel("Format:", this));
    m_sendFormatComboBox = new QComboBox(this);
    m_sendFormatComboBox->addItems({"ASCII", "HEX", "UTF-8"});
    formatLayout->addWidget(m_sendFormatComboBox);

    m_addNewlineCheckBox = new QCheckBox("Add Newline (\\n)", this);
    formatLayout->addWidget(m_addNewlineCheckBox);
    formatLayout->addStretch();

    m_clearSendButton = new QPushButton("Clear", this);
    m_clearSendButton->setMaximumWidth(80);
    formatLayout->addWidget(m_clearSendButton);

    sendLayout->addLayout(formatLayout);

    // 发送文本框
    m_sendTextEdit = new QPlainTextEdit(this);
    m_sendTextEdit->setPlaceholderText("Enter data to send...");
    m_sendTextEdit->setMaximumHeight(100);
    sendLayout->addWidget(m_sendTextEdit);

    // 发送按钮
    m_sendButton = new QPushButton("Send", this);
    m_sendButton->setMinimumHeight(40);
    m_sendButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    sendLayout->addWidget(m_sendButton);

    mainLayout->addWidget(sendGroup);

    // ============ 接收区 ============
    QGroupBox *receiveGroup = new QGroupBox("Receive Data", this);
    QVBoxLayout *receiveLayout = new QVBoxLayout(receiveGroup);

    // 接收文本框
    m_receiveTextEdit = new QTextEdit(this);
    m_receiveTextEdit->setReadOnly(true);
    m_receiveTextEdit->setPlaceholderText("Data received will be displayed here...");
    receiveLayout->addWidget(m_receiveTextEdit);

    // 清空按钮
    m_clearReceiveButton = new QPushButton("Clear", this);
    m_clearReceiveButton->setMaximumWidth(80);
    receiveLayout->addWidget(m_clearReceiveButton);

    mainLayout->addWidget(receiveGroup);

    // ============ 状态栏 ============
    m_statusLabel = new QLabel("Ready", this);
    m_bytesReceivedLabel = new QLabel("Received: 0 bytes", this);
    m_bytesSentLabel = new QLabel("Sent: 0 bytes", this);

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_bytesSentLabel);
    statusBar()->addPermanentWidget(m_bytesReceivedLabel);
}

void MainWindow::applyStyles()
{
    // 设置应用程序样式表
    QString styleSheet = R"(
        /* 主窗口样式 */
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        /* 分组框 */
        QGroupBox {
            color: #333333;
            border: 2px solid #cccccc;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
            font-size: 11pt;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
        
        /* 按钮样式 */
        QPushButton {
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: bold;
            font-size: 10pt;
        }
        
        QPushButton:hover {
            background-color: #0056b3;
        }
        
        QPushButton:pressed {
            background-color: #003d82;
        }
        
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        
        /* 连接按钮特殊样式 */
        QPushButton#connectButton {
            background-color: #28a745;
            min-width: 80px;
        }
        
        QPushButton#connectButton:hover {
            background-color: #218838;
        }
        
        QPushButton#connectButton:pressed {
            background-color: #1a6e2e;
        }
        
        /* 文本编辑框 */
        QTextEdit {
            background-color: white;
            color: #333333;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px;
            font-family: "Courier New", monospace;
            font-size: 10pt;
            selection-background-color: #007bff;
        }
        
        /* 组合框 */
        QComboBox {
            background-color: white;
            color: #333333;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px;
            font-size: 10pt;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
        }
        
        QComboBox:hover {
            border: 1px solid #0078d4;
            background-color: #f9f9f9;
        }
        
        /* 数字输入框 */
        QSpinBox, QDoubleSpinBox {
            background-color: white;
            color: #333333;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 4px;
            font-size: 10pt;
        }
        
        QSpinBox:hover, QDoubleSpinBox:hover {
            border: 1px solid #0078d4;
        }
        
        /* 标签 */
        QLabel {
            color: #333333;
            font-size: 10pt;
        }
        
        /* 状态栏 */
        QStatusBar {
            background-color: #e8e8e8;
            color: #333333;
            border-top: 1px solid #cccccc;
        }
        
        QStatusBar::item {
            border: none;
        }
        
        /* 滚动条 */
        QScrollBar:vertical {
            background-color: #f5f5f5;
            width: 12px;
            border: none;
        }
        
        QScrollBar::handle:vertical {
            background-color: #cccccc;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #999999;
        }
        
        QScrollBar::handle:vertical:pressed {
            background-color: #666666;
        }
        
        QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {
            border: none;
            width: 0px;
            height: 0px;
        }
        
        /* 分割线 */
        QSplitter::handle {
            background-color: #e8e8e8;
        }
        
        QSplitter::handle:hover {
            background-color: #d0d0d0;
        }
    )";
    
    qApp->setStyle("Fusion");
    qApp->setStyleSheet(styleSheet);
    
    // 应用调色板
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(245, 245, 245));
    darkPalette.setColor(QPalette::WindowText, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::ToolTipText, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::Text, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::Button, QColor(245, 245, 245));
    darkPalette.setColor(QPalette::ButtonText, QColor(51, 51, 51));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(0, 123, 255));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 123, 255));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    
    qApp->setPalette(darkPalette);
}

void MainWindow::connectSignals()
{
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_clearSendButton, &QPushButton::clicked, this, &MainWindow::onClearSendClicked);
    connect(m_clearReceiveButton, &QPushButton::clicked, this, &MainWindow::onClearReceiveClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshPorts);

    connect(m_serialPort.get(), &SerialPort::dataReceived,
            this, &MainWindow::onDataReceived);
    connect(m_serialPort.get(), &SerialPort::dataSent,
            this, &MainWindow::onDataSent);
    connect(m_serialPort.get(), &SerialPort::errorOccurred,
            this, &MainWindow::onError);
    connect(m_serialPort.get(), &SerialPort::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);

    connect(m_receiveTextEdit, &QTextEdit::textChanged,
            this, &MainWindow::onReceiveTextChanged);
}

void MainWindow::loadSettings()
{
    QSettings settings("SCOM", "SerialCommunicationTool");

    QString lastPort = settings.value("lastPort", "").toString();
    int lastBaudRate = settings.value("lastBaudRate", 115200).toInt();
    QString lastFormat = settings.value("lastFormat", "ASCII").toString();

    m_baudRateSpinBox->setValue(lastBaudRate);

    int portIndex = m_portComboBox->findText(lastPort);
    if (portIndex >= 0)
    {
        m_portComboBox->setCurrentIndex(portIndex);
    }

    int formatIndex = m_sendFormatComboBox->findText(lastFormat);
    if (formatIndex >= 0)
    {
        m_sendFormatComboBox->setCurrentIndex(formatIndex);
    }

    restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
    restoreState(settings.value("windowState", QByteArray()).toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("SCOM", "SerialCommunicationTool");

    settings.setValue("lastPort", m_portComboBox->currentText());
    settings.setValue("lastBaudRate", m_baudRateSpinBox->value());
    settings.setValue("lastFormat", m_sendFormatComboBox->currentText());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    if (m_serialPort && m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::onConnectClicked()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
        m_connectButton->setChecked(false);
    }
    else
    {
        QString portName = m_portComboBox->currentText();
        if (portName.isEmpty())
        {
            m_statusLabel->setText("Error: No port selected");
            return;
        }

        qint32 baudRate = m_baudRateSpinBox->value();

        // 解析数据位
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        int dataBitsValue = m_dataBitsComboBox->currentText().toInt();
        if (dataBitsValue == 5)
            dataBits = QSerialPort::Data5;
        else if (dataBitsValue == 6)
            dataBits = QSerialPort::Data6;
        else if (dataBitsValue == 7)
            dataBits = QSerialPort::Data7;

        // 解析停止位
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QString stopBitsText = m_stopBitsComboBox->currentText();
        if (stopBitsText == "1.5")
            stopBits = QSerialPort::OneAndHalfStop;
        else if (stopBitsText == "2")
            stopBits = QSerialPort::TwoStop;

        // 解析校验位
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QString parityText = m_parityComboBox->currentText();
        if (parityText == "Even")
            parity = QSerialPort::EvenParity;
        else if (parityText == "Odd")
            parity = QSerialPort::OddParity;
        else if (parityText == "Space")
            parity = QSerialPort::SpaceParity;
        else if (parityText == "Mark")
            parity = QSerialPort::MarkParity;

        // 解析流控制
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
        QString flowControlText = m_flowControlComboBox->currentText();
        if (flowControlText == "RTS/CTS")
            flowControl = QSerialPort::HardwareControl;
        else if (flowControlText == "XON/XOFF")
            flowControl = QSerialPort::SoftwareControl;

        if (m_serialPort->open(portName, baudRate, dataBits, stopBits, parity, flowControl))
        {
            m_connectButton->setChecked(true);
            m_statusLabel->setText("Connected: " + portName + " @ " + QString::number(baudRate));
        }
        else
        {
            m_connectButton->setChecked(false);
            m_statusLabel->setText("Failed to connect");
        }
    }
}

void MainWindow::onSendClicked()
{
    if (!m_serialPort->isOpen())
    {
        m_statusLabel->setText("Error: Serial port is not open");
        return;
    }

    QString data = m_sendTextEdit->toPlainText();
    if (data.isEmpty())
    {
        return;
    }

    if (m_addNewlineCheckBox->isChecked() && !data.endsWith('\n'))
    {
        data.append('\n');
    }

    SerialPort::DataFormat format = SerialPort::DataFormat::ASCII;
    QString formatText = m_sendFormatComboBox->currentText();
    if (formatText == "HEX")
        format = SerialPort::DataFormat::HEX;
    else if (formatText == "UTF-8")
        format = SerialPort::DataFormat::UTF8;

    m_serialPort->write(data, format);
}

void MainWindow::onClearReceiveClicked()
{
    m_receiveTextEdit->clear();
    m_bytesReceived = 0;
    m_bytesReceivedLabel->setText("Received: 0 bytes");
}

void MainWindow::onClearSendClicked()
{
    m_sendTextEdit->clear();
}

void MainWindow::onRefreshPorts()
{
    QString currentPort = m_portComboBox->currentText();
    m_portComboBox->clear();

    QStringList ports = SerialPort::scanAvailablePorts();
    m_portComboBox->addItems(ports);

    int index = m_portComboBox->findText(currentPort);
    if (index >= 0)
    {
        m_portComboBox->setCurrentIndex(index);
    }

    if (ports.isEmpty())
    {
        m_statusLabel->setText("No serial ports available");
    }
}

void MainWindow::onDataReceived(const QString &data, SerialPort::DataFormat format)
{
    QString formatStr;
    switch (format)
    {
    case SerialPort::DataFormat::ASCII:
        formatStr = "ASCII";
        break;
    case SerialPort::DataFormat::HEX:
        formatStr = "HEX";
        break;
    case SerialPort::DataFormat::UTF8:
        formatStr = "UTF-8";
        break;
    }

    addReceiveLog(data, formatStr);
    m_bytesReceived += data.length();
    m_bytesReceivedLabel->setText("Received: " + QString::number(m_bytesReceived) + " bytes");
}

void MainWindow::onDataSent(const QString &data, SerialPort::DataFormat format)
{
    QString formatStr;
    switch (format)
    {
    case SerialPort::DataFormat::ASCII:
        formatStr = "ASCII";
        break;
    case SerialPort::DataFormat::HEX:
        formatStr = "HEX";
        break;
    case SerialPort::DataFormat::UTF8:
        formatStr = "UTF-8";
        break;
    }

    addSendLog(data, formatStr);
    m_bytesSent += data.length();
    m_bytesSentLabel->setText("Sent: " + QString::number(m_bytesSent) + " bytes");
}

void MainWindow::onError(const QString &error)
{
    m_statusLabel->setText("Error: " + error);
    addReceiveLog("[ERROR] " + error, "ERROR");
}

void MainWindow::onConnectionStatusChanged(bool isOpen)
{
    updateConnectionUI(isOpen);
}

void MainWindow::updateConnectionUI(bool isConnected)
{
    m_isConnected = isConnected;
    m_connectButton->setChecked(isConnected);

    // 设置发送按钮状态
    m_sendButton->setEnabled(isConnected);
    m_sendTextEdit->setEnabled(isConnected);
    m_sendFormatComboBox->setEnabled(isConnected);

    // 设置配置选项状态
    m_portComboBox->setEnabled(!isConnected);
    m_baudRateSpinBox->setEnabled(!isConnected);
    m_dataBitsComboBox->setEnabled(!isConnected);
    m_stopBitsComboBox->setEnabled(!isConnected);
    m_parityComboBox->setEnabled(!isConnected);
    m_flowControlComboBox->setEnabled(!isConnected);
    m_refreshButton->setEnabled(!isConnected);

    if (isConnected)
    {
        m_connectButton->setText("Disconnect");
        m_connectButton->setStyleSheet("background-color: #f44336; color: white;");
    }
    else
    {
        m_connectButton->setText("Connect");
        m_connectButton->setStyleSheet("");
    }
}

void MainWindow::addReceiveLog(const QString &data, const QString &format)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logEntry = QString("[%1] <%2> %3\n")
                           .arg(timestamp)
                           .arg(format)
                           .arg(data);

    m_receiveTextEdit->insertPlainText(logEntry);
}

void MainWindow::addSendLog(const QString &data, const QString &format)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logEntry = QString("[%1] >%2> %3\n")
                           .arg(timestamp)
                           .arg(format)
                           .arg(data);

    m_receiveTextEdit->insertPlainText(logEntry);
}

void MainWindow::onReceiveTextChanged()
{
    // 自动滚动到底部
    QScrollBar *scrollBar = m_receiveTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
