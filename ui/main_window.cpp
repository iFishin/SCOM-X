#include "main_window.h"
#include "ui_main_window.h"
#include "serial_port.h"
#include "config_manager.h"
#include "preferences_dialog.h"

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
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QIntValidator>
#include <QSpacerItem>
#include <QDialog>
#include <QKeyEvent>
#include <QTextCursor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()), 
      configManager(std::make_unique<ConfigManager>()), serialPort(std::make_unique<SerialPort>())
{
    // 初始化配置管理器
    configManager->initialize();

    // 从 .ui 文件生成的 UI 代码（由 Qt 自动生成）
    ui->setupUi(this);

    // 应用样式
    applyStyles();

    // 构建动态 UI（快捷指令表格等）
    setupDynamicUI();

    // 连接信号槽
    connectSignals();

    // 加载之前保存的设置
    loadSettings();

    // 初始化状态栏
    QLabel *bytesReceivedLabel = new QLabel("Rec: 0 Bytes", this);
    QLabel *bytesSentLabel = new QLabel("Sent: 0 Bytes", this);

    statusBar()->addPermanentWidget(bytesReceivedLabel);
    statusBar()->addPermanentWidget(bytesSentLabel);

    // 保存指针供后续使用（作为成员变量或直接使用）
    bytesReceived = 0;
    bytesSent = 0;
}

MainWindow::~MainWindow() {
    // 保存配置
    if (configManager) {
        configManager->saveConfig();
    }
}

void MainWindow::setupDynamicUI()
{
    // 配置端口下拉框
    ui->portComboBox->setMinimumWidth(100);

    // 配置波特率（QComboBox with editable mode）
    QStringList commonBaudRates = {
        "50", "75", "110", "134", "150", "200", "300", "600", "1200", "1800",
        "2400", "4800", "7200", "9600", "14400", "19200", "28800", "38400",
        "57600", "76800", "115200", "128000", "153600", "230400", "256000",
        "460800", "500000", "576000", "921600", "1000000", "1152000", "1500000",
        "2000000", "2500000", "3000000", "3500000", "4000000", "4500000",
        "5000000", "5500000", "6000000", "6500000", "7000000", "7500000", "8000000"};
    ui->baudRateSpinBox->addItems(commonBaudRates);
    ui->baudRateSpinBox->setCurrentText("115200"); // 设置默认值
    ui->baudRateSpinBox->setMaximumWidth(120);

    // 配置接收区：只读日志区域
    ui->receiveArea->setStyleSheet("QPlainTextEdit { font-family: 'Courier New', monospace; font-size: 10pt; }");
    ui->receiveArea->setReadOnly(true);
    
    // 配置终端输入框
    ui->terminalInput->setStyleSheet("QLineEdit { font-family: 'Courier New', monospace; font-size: 10pt; }");
    ui->terminalPrompt->setStyleSheet("QLabel { font-family: 'Courier New', monospace; font-size: 10pt; color: green; font-weight: bold; }");
    ui->terminalHexMode->setCheckState(Qt::Unchecked);
    
    // 配置快捷指令表
    ui->commandTableLayout->setSpacing(5);
    ui->commandTableLayout->setContentsMargins(5, 5, 5, 5);

    // 建立快捷指令行（从行1开始，行0是表头）
    rebuildCommandTable(currentCommandRows);
}

void MainWindow::connectSignals()
{
    // 连接按钮
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(ui->clearReceiveButton, &QPushButton::clicked, this, &MainWindow::onClearReceiveArea);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshPorts);
    connect(ui->moreSettingsButton, &QPushButton::clicked, this, &MainWindow::onPreferencesClicked);

    // 连接菜单信号
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::onPreferencesClicked);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutAction);

    // 连接设置变化
    connect(ui->portComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSettingChanged);
    connect(ui->baudRateSpinBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSettingChanged);

    // 连接终端输入框的 Return 键
    connect(ui->terminalInput, &QLineEdit::returnPressed, this, [this]() {
        QString command = ui->terminalInput->text().trimmed();
        if (command.isEmpty()) {
            return;
        }
        
        // 显示输入命令到日志区域
        ui->receiveArea->insertPlainText("> " + command + "\n");
        
        // 滚动到底部
        QTextCursor cursor = ui->receiveArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->receiveArea->setTextCursor(cursor);
        
        // 发送到串口
        if (serialPort && serialPort->isOpen()) {
            if (ui->terminalHexMode->isChecked()) {
                serialPort->write(command, SerialPort::DataFormat::HEX);
            } else {
                serialPort->write(command, SerialPort::DataFormat::ASCII);
            }
            bytesSent += command.length();
            statusBar()->showMessage(QString("发送: %1 字节").arg(bytesSent));
        } else {
            ui->receiveArea->insertPlainText("[错误] 串口未连接\n");
        }
        
        // 清空输入框，焦点保留在输入框
        ui->terminalInput->clear();
    });

    // 连接串口信号
    if (serialPort)
    {
        connect(serialPort.get(), QOverload<const QString &, SerialPort::DataFormat>::of(&SerialPort::dataReceived),
                this, [this](const QString &data, SerialPort::DataFormat format)
                {
                    ui->receiveArea->insertPlainText(data + "\n");
                    // 滚动到底部
                    QTextCursor cursor = ui->receiveArea->textCursor();
                    cursor.movePosition(QTextCursor::End);
                    ui->receiveArea->setTextCursor(cursor);
                    
                    bytesReceived += data.length();
                    // 更新状态栏
                    statusBar()->showMessage(QString("接收: %1 字节").arg(bytesReceived)); });

        connect(serialPort.get(), QOverload<const QString &, SerialPort::DataFormat>::of(&SerialPort::dataSent),
                this, [this](const QString &data, SerialPort::DataFormat format)
                {
                    bytesSent += data.length();
                    // 更新状态栏
                    statusBar()->showMessage(QString("发送: %1 字节").arg(bytesSent)); });

        connect(serialPort.get(), &SerialPort::connectionStatusChanged,
                this, &MainWindow::onConnectionStatusChanged);

        connect(serialPort.get(), &SerialPort::errorOccurred,
                this, &MainWindow::onSerialError);
    }
}

void MainWindow::applyStyles()
{
    QFile styleFile(":/ui/style.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(style);
        styleFile.close();
        qDebug() << "✓ 样式文件加载成功，文件大小:" << style.size() << "字符";
    }
    else
    {
        qDebug() << "✗ 无法加载样式文件: :/ui/style.qss";
        qDebug() << "可用资源:";
        // 列出可用资源进行调试
        QDir dir(":/ui");
        qDebug() << dir.entryList();
    }
}

void MainWindow::onConnectClicked()
{
    if (!serialPort)
        return;

    QString portName = ui->portComboBox->currentText();
    int baudRate = ui->baudRateSpinBox->currentText().toInt();

    if (portName.isEmpty())
    {
        QMessageBox::warning(this, "错误", "请选择一个串口");
        return;
    }

    if (serialPort->open(portName, baudRate))
    {
        updateConnectionStatus(true);
    }
    else
    {
        QMessageBox::critical(this, "错误", "无法打开串口: " + portName);
    }
}

void MainWindow::onDisconnectClicked()
{
    if (serialPort && serialPort->isOpen())
    {
        serialPort->close();
        updateConnectionStatus(false);
    }
}

void MainWindow::onClearReceiveArea()
{
    ui->receiveArea->clear();
    bytesReceived = 0;
    statusBar()->showMessage("接收: 0 字节");
}

void MainWindow::onRefreshPorts()
{
    ui->portComboBox->clear();
    QStringList ports = SerialPort::scanAvailablePorts();
    ui->portComboBox->addItems(ports);
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
    updateConnectionStatus(connected);
}

void MainWindow::onSerialError(const QString &errorMsg)
{
    QMessageBox::critical(this, "串口错误", errorMsg);
}

void MainWindow::onSettingChanged()
{
    // 设置变化时的处理逻辑
    saveSettings();
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected)
    {
        ui->statusLabel->setText("已连接");
        ui->statusLabel->setStyleSheet("color: #00a86b; font-weight: bold;");
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->terminalInput->setEnabled(true);
        ui->portComboBox->setEnabled(false);
        ui->baudRateSpinBox->setEnabled(false);
    }
    else
    {
        ui->statusLabel->setText("已断开连接");
        ui->statusLabel->setStyleSheet("color: #6c757d; font-weight: bold;");
        ui->connectButton->setEnabled(true);
        ui->disconnectButton->setEnabled(false);
        ui->terminalInput->setEnabled(false);
        ui->portComboBox->setEnabled(true);
        ui->baudRateSpinBox->setEnabled(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    if (serialPort && serialPort->isOpen())
    {
        serialPort->close();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    // 从配置管理器加载设置
    if (!configManager) {
        qWarning() << "[MainWindow] ConfigManager not initialized";
        return;
    }

    // 加载串口设置（主界面显示）
    ui->portComboBox->setCurrentText(configManager->getSerialPort());
    ui->baudRateSpinBox->setCurrentText(QString::number(configManager->getBaudRate()));
    ui->terminalHexMode->setChecked(configManager->getHexMode());

    // 加载快捷指令行数设置
    currentCommandRows = configManager->getCommandRows();
    if (currentCommandRows < 1)
        currentCommandRows = 1;
    if (currentCommandRows > maxCommandRows)
        currentCommandRows = maxCommandRows;

    // 加载快捷指令数据（从 QSettings 作为备用）
    QSettings qsettings("SCOM-X", "SCOM-X");
    for (int i = 0; i < (int)commandInputs.size(); ++i)
    {
        QString data = qsettings.value(QString("command_%1_data").arg(i), "").toString();
        if (!data.isEmpty())
        {
            commandInputs[i]->setText(data);
            commandHexCheckboxes[i]->setChecked(qsettings.value(QString("command_%1_hex").arg(i), false).toBool());
            commandEndCheckboxes[i]->setChecked(qsettings.value(QString("command_%1_end").arg(i), true).toBool());
            commandIntervals[i]->setText(qsettings.value(QString("command_%1_interval").arg(i), "0").toString());
        }
    }

    qDebug() << "[MainWindow] Settings loaded from ConfigManager";
    onRefreshPorts();
}

void MainWindow::saveSettings()
{
    // 保存到 ConfigManager
    if (!configManager) {
        qWarning() << "[MainWindow] ConfigManager not available for saving";
        return;
    }

    configManager->setSerialPort(ui->portComboBox->currentText());
    configManager->setBaudRate(ui->baudRateSpinBox->currentText().toInt());
    configManager->setHexMode(ui->terminalHexMode->isChecked());
    configManager->setCommandRows(currentCommandRows);

    // 同时保存快捷指令到 QSettings（作为备用）
    QSettings settings("SCOM-X", "SCOM-X");
    settings.setValue("commandRows", currentCommandRows);

    for (int i = 0; i < (int)commandInputs.size(); ++i)
    {
        settings.setValue(QString("command_%1_data").arg(i), commandInputs[i]->text());
        settings.setValue(QString("command_%1_hex").arg(i), commandHexCheckboxes[i]->isChecked());
        settings.setValue(QString("command_%1_end").arg(i), commandEndCheckboxes[i]->isChecked());
        settings.setValue(QString("command_%1_interval").arg(i), commandIntervals[i]->text());
    }

    settings.sync();
    configManager->saveConfig();

    qDebug() << "[MainWindow] Settings saved to ConfigManager and QSettings";
}

void MainWindow::onQuickCommandButtonClicked(int index)
{
    if (index < (int)commandInputs.size())
    {
        QString data = commandInputs[index]->text();
        if (!data.isEmpty())
        {
            if (serialPort && serialPort->isOpen())
            {
                serialPort->write(data, commandHexCheckboxes[index]->isChecked() ? SerialPort::DataFormat::HEX : SerialPort::DataFormat::ASCII);
                bytesSent += data.length();
                statusBar()->showMessage(QString("发送: %1 字节").arg(bytesSent));
            }
            else
            {
                QMessageBox::warning(this, "错误", "串口未连接");
            }
        }
    }
}

void MainWindow::onQuickCommandReturnPressed(int index)
{
    onQuickCommandButtonClicked(index);
}

void MainWindow::onHotkeyClicked(int index)
{
    if (index >= 0 && index < (int)commandInputs.size())
    {
        onQuickCommandButtonClicked(index);
    }
}

void MainWindow::onSettingsAction()
{
    QMessageBox::information(this, "设置", "设置功能开发中...");
}

void MainWindow::onAboutAction()
{
    QMessageBox::about(this, "关于 SCOM-X",
                       "SCOM-X - 串口通信工具\n"
                       "版本: 1.0.0\n\n"
                       "一个功能强大的串口通信工具，支持多种数据格式和快捷指令。");
}

void MainWindow::onPreferencesClicked()
{
    // 创建首选项对话框（带 DTR, RTS, 流控制等串口设置）
    PreferencesDialog preferencesDialog(this, configManager.get());
    preferencesDialog.exec();
}

void MainWindow::rebuildCommandTable(int rowCount)
{
    // 清除现有的数据行（保留表头第0行）
    while (ui->commandTableLayout->rowCount() > 1)
    {
        for (int col = 0; col < 6; ++col)
        {
            QLayoutItem *item = ui->commandTableLayout->itemAtPosition(ui->commandTableLayout->rowCount() - 1, col);
            if (item)
            {
                delete item->widget();
                ui->commandTableLayout->removeItem(item);
            }
        }
    }

    // 清除向量中的指针
    for (auto *checkbox : commandCheckboxes)
        delete checkbox;
    for (auto *button : commandButtons)
        delete button;
    for (auto *input : commandInputs)
        delete input;
    for (auto *hexCheckbox : commandHexCheckboxes)
        delete hexCheckbox;
    for (auto *endCheckbox : commandEndCheckboxes)
        delete endCheckbox;
    for (auto *interval : commandIntervals)
        delete interval;

    commandCheckboxes.clear();
    commandButtons.clear();
    commandInputs.clear();
    commandHexCheckboxes.clear();
    commandEndCheckboxes.clear();
    commandIntervals.clear();

    // 创建新的快捷指令行
    for (int i = 0; i < rowCount; ++i)
    {
        // 复选框
        QCheckBox *checkbox = new QCheckBox();
        commandCheckboxes.push_back(checkbox);
        ui->commandTableLayout->addWidget(checkbox, i + 1, 0, Qt::AlignCenter);

        // 发送按钮
        QPushButton *button = new QPushButton(QString("Send %1").arg(i + 1));
        button->setMaximumWidth(80);
        commandButtons.push_back(button);
        ui->commandTableLayout->addWidget(button, i + 1, 1);
        connect(button, &QPushButton::clicked, this, [this, i]()
                { onQuickCommandButtonClicked(i); });

        // 数据输入框（占据尽可能多的空间）
        QLineEdit *inputField = new QLineEdit();
        inputField->setPlaceholderText("Input command...");
        commandInputs.push_back(inputField);
        ui->commandTableLayout->addWidget(inputField, i + 1, 2);
        connect(inputField, &QLineEdit::returnPressed, this, [this, i]()
                { onQuickCommandReturnPressed(i); });

        // HEX 复选框
        QCheckBox *hexCheckbox = new QCheckBox();
        hexCheckbox->setToolTip("以十六进制发送");
        commandHexCheckboxes.push_back(hexCheckbox);
        ui->commandTableLayout->addWidget(hexCheckbox, i + 1, 3, Qt::AlignCenter);

        // 加回车复选框
        QCheckBox *endCheckbox = new QCheckBox();
        endCheckbox->setChecked(true);
        endCheckbox->setToolTip("自动添加换行符");
        commandEndCheckboxes.push_back(endCheckbox);
        ui->commandTableLayout->addWidget(endCheckbox, i + 1, 4, Qt::AlignCenter);

        // 间隔输入框
        QLineEdit *intervalField = new QLineEdit();
        intervalField->setMaximumWidth(50);
        intervalField->setPlaceholderText("0");
        intervalField->setAlignment(Qt::AlignCenter);
        // 只允许数字输入
        QIntValidator *validator = new QIntValidator(0, 99999, this);
        intervalField->setValidator(validator);
        commandIntervals.push_back(intervalField);
        ui->commandTableLayout->addWidget(intervalField, i + 1, 5);
    }

    // 添加伸缩项
    ui->commandTableLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding),
                                    rowCount + 1, 0, 1, 6);
}
