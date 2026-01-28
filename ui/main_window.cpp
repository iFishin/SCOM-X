#include "main_window.h"
#include "ui_main_window.h"
#include "serial_port.h"
#include "config_manager.h"
#include "preferences_dialog.h"
#include "at_command_page.h"
#include "log_page.h"
#include "receive_data_page.h"
#include "log_viewer_dialog.h"
#include "operation_logger.h"

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

// 外部声明日志函数
extern void debugLog(const QString &msg);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()), 
      configManager(std::make_unique<ConfigManager>()), serialPort(std::make_unique<SerialPort>())
{
    debugLog("[MainWindow] 1. 初始化配置管理器...");
    // 初始化配置管理器
    configManager->initialize();
    debugLog("[MainWindow] 1. OK - 配置管理器初始化完成");

    debugLog("[MainWindow] 2. setupUi...");
    // 从 .ui 文件生成的 UI 代码（由 Qt 自动生成）
    ui->setupUi(this);
    debugLog("[MainWindow] 2. OK - setupUi 完成");

    debugLog("[MainWindow] 3. applyStyles...");
    // 应用样式
    applyStyles();
    debugLog("[MainWindow] 3. OK - 样式应用完成");

    debugLog("[MainWindow] 4. setupDynamicUI...");
    // 构建动态 UI（快捷指令表格等）
    setupDynamicUI();
    debugLog("[MainWindow] 4. OK - 动态UI完成");

    debugLog("[MainWindow] 5. connectSignals...");
    // 连接信号槽
    connectSignals();
    debugLog("[MainWindow] 5. OK - 信号槽连接完成");
    
    debugLog("[MainWindow] 6. 创建 AT Command 页面...");
    // 创建 AT Command 页面（但暂时隐藏）
    try {
        atCommandPage = std::make_unique<ATCommandPage>(configManager.get(), this);
        atCommandPage->hide();
        debugLog("[MainWindow] 6. OK - AT Command 页面创建完成");
    } catch (const std::exception &e) {
        debugLog(QString("[MainWindow] 6. FAILED - AT Command 页面创建失败: %1").arg(e.what()));
    }
    
    debugLog("[MainWindow] 7. 创建 Log 页面...");
    // 创建 Log 页面
    try {
        logPage = std::make_unique<LogPage>(this);
        logPage->hide();
        debugLog("[MainWindow] 7. OK - Log 页面创建完成");
    } catch (const std::exception &e) {
        debugLog(QString("[MainWindow] 7. FAILED - Log 页面创建失败: %1").arg(e.what()));
    }
    
    debugLog("[MainWindow] 8. 创建 Receive Data 页面...");
    // 创建 Receive Data 页面
    try {
        receiveDataPage = std::make_unique<ReceiveDataPage>(
            ui->receiveArea,
            ui->terminalInput,
            ui->terminalHexMode,
            ui->lineEndComboBox,
            this
        );
        receiveDataPage->hide();
        debugLog("[MainWindow] 8. OK - Receive Data 页面创建完成");
    } catch (const std::exception &e) {
        debugLog(QString("[MainWindow] 8. FAILED - Receive Data 页面创建失败: %1").arg(e.what()));
    }

    debugLog("[MainWindow] 9. 加载设置...");
    // 加载之前保存的设置
    loadSettings();
    debugLog("[MainWindow] 9. OK - 设置加载完成");

    debugLog("[MainWindow] 10. 初始化状态栏...");
    // 初始化状态栏
    QLabel *bytesReceivedLabel = new QLabel("Rec: 0 Bytes", this);
    QLabel *bytesSentLabel = new QLabel("Sent: 0 Bytes", this);

    statusBar()->addPermanentWidget(bytesReceivedLabel);
    statusBar()->addPermanentWidget(bytesSentLabel);

    // 保存指针供后续使用（作为成员变量或直接使用）
    bytesReceived = 0;
    bytesSent = 0;
    debugLog("[MainWindow] 10. OK - 状态栏初始化完成");
    
    debugLog("[MainWindow] 11. 初始化日志查看器...");
    // 初始化日志查看器（但不显示）
    logViewerDialog = nullptr;
    debugLog("[MainWindow] 11. OK - 日志查看器初始化完成");
    
    debugLog("[MainWindow] 12. 初始化操作日志器...");
    // 初始化操作日志器
    OperationLogger::instance().initialize();
    OperationLogger::instance().logInfo("应用启动成功");
    debugLog("[MainWindow] 12. OK - 操作日志器初始化完成");
    
    debugLog("[MainWindow] ========== 构造函数完成 ==========");
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
    ui->receiveArea->setObjectName("receiveArea");
    ui->receiveArea->setReadOnly(true);
    
    // 配置终端输入框（QComboBox with history）
    ui->terminalInput->setObjectName("terminalInput");
    ui->terminalInput->setMaxVisibleItems(10);
    ui->terminalInput->setMaximumHeight(30);
    ui->terminalPrompt->setObjectName("terminalPrompt");
    ui->terminalHexMode->setCheckState(Qt::Unchecked);
    
    // 配置行尾符下拉框（默认选择 0D0A - CRLF）
    ui->lineEndComboBox->setCurrentIndex(0);
    ui->lineEndComboBox->setMaximumWidth(120);
    
    // 配置快捷指令表
    ui->commandTableLayout->setSpacing(5);
    ui->commandTableLayout->setContentsMargins(5, 5, 5, 5);

    // 建立快捷指令行（从行1开始，行0是表头）
    rebuildCommandTable(currentCommandRows);
    
    // 加载终端命令历史
    loadTerminalHistory();
    
    // 添加日志查看器菜单项
    QMenu *viewMenu = menuBar()->addMenu(tr("查看(&V)"));
    QAction *logViewerAction = viewMenu->addAction(tr("程序日志(&L)"));
    logViewerAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_L);
    connect(logViewerAction, &QAction::triggered, this, &MainWindow::onShowLogViewer);
}

void MainWindow::connectSignals()
{
    // 连接按钮
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->clearReceiveButton, &QPushButton::clicked, this, &MainWindow::onClearReceiveArea);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshPorts);
    connect(ui->moreSettingsButton, &QPushButton::clicked, this, &MainWindow::onPreferencesClicked);

    // 连接菜单信号
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::onPreferencesClicked);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutAction);

    // 连接窗口切换菜单
    connect(ui->actionMain, &QAction::triggered, this, &MainWindow::onSwitchToMain);
    connect(ui->actionATCommand, &QAction::triggered, this, &MainWindow::onSwitchToATCommand);
    connect(ui->actionLog, &QAction::triggered, this, &MainWindow::onSwitchToLog);
    
    // 连接日志查看器菜单（如果菜单中存在日志查看器选项）
    // 我们稍后会添加这个菜单项

    // 连接设置变化
    connect(ui->portComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSettingChanged);
    connect(ui->baudRateSpinBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSettingChanged);

    // 连接终端输入框的 Return 键 - QComboBox 需要连接内部 lineEdit 的信号
    connect(ui->terminalInput->lineEdit(), &QLineEdit::returnPressed, this, [this]() {
        QString command = ui->terminalInput->lineEdit()->text().trimmed();
        if (command.isEmpty()) {
            return;
        }
        
        // 获取行尾符
        QString lineEnd = getLineEndSuffix();
        
        // 显示输入命令到日志区域
        ui->receiveArea->insertPlainText("> " + command + "\n");
        
        // 滚动到底部
        QTextCursor cursor = ui->receiveArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->receiveArea->setTextCursor(cursor);
        
        // 发送到串口
        if (serialPort && serialPort->isOpen()) {
            // 拼接完整的发送数据（命令 + 行尾符）
            QString fullCommand = command + lineEnd;
            
            if (ui->terminalHexMode->isChecked()) {
                serialPort->write(fullCommand, SerialPort::DataFormat::HEX);
            } else {
                serialPort->write(fullCommand, SerialPort::DataFormat::ASCII);
            }
            bytesSent += fullCommand.length();
            statusBar()->showMessage(QString("发送: %1 字节").arg(bytesSent));
        } else {
            ui->receiveArea->insertPlainText("[错误] 串口未连接\n");
        }
        
        // 将命令添加到历史记录
        addTerminalHistory(command);
        
        // 清空输入框，焦点保留在输入框
        // ui->terminalInput->lineEdit()->clear();
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

    // 连接表头复选框的全选/取消全选
    connect(ui->headerCheck, &QCheckBox::toggled, this, &MainWindow::onHeaderCheckBoxToggled);
    
    // 连接 ReceiveDataPage 的命令发送信号
    if (receiveDataPage) {
        connect(receiveDataPage.get(), &ReceiveDataPage::terminalCommandEntered, this, [this](const QString &command) {
            if (command.isEmpty()) {
                return;
            }
            
            // 获取行尾符
            QString lineEnd = getLineEndSuffix();
            
            // 显示输入命令到主窗口的接收区域
            ui->receiveArea->insertPlainText("> " + command + "\n");
            
            // 滚动到底部
            QTextCursor cursor = ui->receiveArea->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->receiveArea->setTextCursor(cursor);
            
            // 发送到串口
            if (serialPort && serialPort->isOpen()) {
                // 拼接完整的发送数据（命令 + 行尾符）
                QString fullCommand = command + lineEnd;
                
                if (ui->terminalHexMode->isChecked()) {
                    serialPort->write(fullCommand, SerialPort::DataFormat::HEX);
                } else {
                    serialPort->write(fullCommand, SerialPort::DataFormat::ASCII);
                }
                bytesSent += fullCommand.length();
                statusBar()->showMessage(QString("发送: %1 字节").arg(bytesSent));
            } else {
                ui->receiveArea->insertPlainText("[错误] 串口未连接\n");
            }
            
            // 将命令添加到历史记录
            addTerminalHistory(command);
            
            // 同步清空两个页面的输入框
            ui->terminalInput->lineEdit()->clear();
            if (receiveDataPage && receiveDataPage->getTerminalInput()) {
                receiveDataPage->getTerminalInput()->lineEdit()->clear();
            }
        });
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

    // 如果已连接，则执行断开操作
    if (serialPort->isOpen())
    {
        serialPort->close();
        updateConnectionStatus(false);
        OperationLogger::instance().logSerialDisconnect();
        return;
    }

    // 否则执行连接操作
    QString portName = ui->portComboBox->currentText();
    int baudRate = ui->baudRateSpinBox->currentText().toInt();

    if (portName.isEmpty())
    {
        QMessageBox::warning(this, "错误", "请选择一个串口");
        OperationLogger::instance().logWarning("未选择串口");
        return;
    }

    if (serialPort->open(portName, baudRate))
    {
        updateConnectionStatus(true);
        OperationLogger::instance().logSerialConnect(portName, baudRate);
    }
    else
    {
        QMessageBox::critical(this, "错误", "无法打开串口: " + portName);
        OperationLogger::instance().logError(QString("无法打开串口: %1").arg(portName));
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
        ui->statusLabel->setText("Connected");
        ui->statusLabel->setProperty("connectionStatus", "connected");
        ui->connectButton->setText("Disconnect");

        ui->terminalInput->setEnabled(true);
        ui->portComboBox->setEnabled(false);
        ui->baudRateSpinBox->setEnabled(false);
    }
    else
    {
        ui->statusLabel->setText("Disconnected");
        ui->statusLabel->setProperty("connectionStatus", "disconnected");
        ui->connectButton->setText("Connect");
        ui->connectButton->setEnabled(true);
        ui->terminalInput->setEnabled(false);
        ui->portComboBox->setEnabled(true);
        ui->baudRateSpinBox->setEnabled(true);
    }
    
    // 重新应用样式以更新属性选择器
    ui->statusLabel->style()->unpolish(ui->statusLabel);
    ui->statusLabel->style()->polish(ui->statusLabel);
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
    ui->lineEndComboBox->setCurrentIndex(configManager->getLineEndIndex());

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
    configManager->setLineEndIndex(ui->lineEndComboBox->currentIndex());
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
        // 复选框（行选择）
        QCheckBox *checkbox = new QCheckBox();
        checkbox->setObjectName("commandRowCheckbox");  // 设置 id，方便 QSS 引用
        commandCheckboxes.push_back(checkbox);
        ui->commandTableLayout->addWidget(checkbox, i + 1, 0, Qt::AlignCenter);
        
        // 连接复选框的 toggled 信号，用于更新表头复选框状态
        connect(checkbox, &QCheckBox::toggled, this, [this]() {
            // 检查是否全部选中或全部未选中
            bool allChecked = !commandCheckboxes.empty();
            bool allUnchecked = !commandCheckboxes.empty();
            
            for (QCheckBox *cb : commandCheckboxes) {
                if (!cb->isChecked()) {
                    allChecked = false;
                }
                if (cb->isChecked()) {
                    allUnchecked = false;
                }
            }
            
            // 更新表头复选框状态
            ui->headerCheck->blockSignals(true);
            if (allChecked) {
                ui->headerCheck->setChecked(true);
            } else if (allUnchecked) {
                ui->headerCheck->setChecked(false);
            } else {
                // 中间状态（部分选中）- 设置为不确定状态
                ui->headerCheck->setTristate(true);
                ui->headerCheck->setCheckState(Qt::PartiallyChecked);
            }
            ui->headerCheck->blockSignals(false);
        });

        // 发送按钮
        QPushButton *button = new QPushButton(QString("Send %1").arg(i + 1));
        button->setMaximumWidth(80);
        commandButtons.push_back(button);
        ui->commandTableLayout->addWidget(button, i + 1, 1);
        connect(button, &QPushButton::clicked, this, [this, i]()
                { onQuickCommandButtonClicked(i); });

        // 数据输入框（占据尽可能多的空间）
        QLineEdit *inputField = new QLineEdit();
        inputField->setObjectName("commandInput");  // 设置 id
        inputField->setPlaceholderText("Input command...");
        commandInputs.push_back(inputField);
        ui->commandTableLayout->addWidget(inputField, i + 1, 2);
        connect(inputField, &QLineEdit::returnPressed, this, [this, i]()
                { onQuickCommandReturnPressed(i); });

        // HEX 复选框
        QCheckBox *hexCheckbox = new QCheckBox();
        hexCheckbox->setObjectName("commandHexCheckbox");  // 设置 id
        hexCheckbox->setToolTip("以十六进制发送");
        commandHexCheckboxes.push_back(hexCheckbox);
        ui->commandTableLayout->addWidget(hexCheckbox, i + 1, 3, Qt::AlignCenter);

        // 加回车复选框
        QCheckBox *endCheckbox = new QCheckBox();
        endCheckbox->setObjectName("commandEndCheckbox");  // 设置 id
        endCheckbox->setChecked(true);
        endCheckbox->setToolTip("自动添加换行符");
        commandEndCheckboxes.push_back(endCheckbox);
        ui->commandTableLayout->addWidget(endCheckbox, i + 1, 4, Qt::AlignCenter);

        // 间隔输入框
        QLineEdit *intervalField = new QLineEdit();
        intervalField->setObjectName("commandInterval");  // 设置 id
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

QString MainWindow::getLineEndSuffix() const
{
    int index = ui->lineEndComboBox->currentIndex();
    switch (index) {
        case 0:  // 0D0A (CRLF)
            return "\r\n";
        case 1:  // 0A (LF)
            return "\n";
        case 2:  // 0D (CR)
            return "\r";
        case 3:  // None
        default:
            return "";
    }
}

void MainWindow::loadTerminalHistory()
{
    if (!configManager) {
        return;
    }
    
    QStringList history = configManager->getTerminalHistory();
    
    // 清空现有项目（如果有）
    ui->terminalInput->clear();
    
    // 添加历史记录到下拉框
    for (const QString &cmd : history) {
        if (!cmd.isEmpty()) {
            ui->terminalInput->addItem(cmd);
        }
    }
    
    qDebug() << "[MainWindow] Loaded" << history.size() << "terminal history items";
}

void MainWindow::addTerminalHistory(const QString &command)
{
    if (command.isEmpty() || !configManager) {
        return;
    }
    
    // 检查是否已存在此命令
    int existingIndex = ui->terminalInput->findText(command);
    if (existingIndex >= 0) {
        // 如果已存在，将其移到最前面
        ui->terminalInput->removeItem(existingIndex);
    }
    
    // 添加到开头
    ui->terminalInput->insertItem(0, command);
    ui->terminalInput->setCurrentIndex(0);
    
    // 限制历史记录数量（最多保存 50 条）
    while (ui->terminalInput->count() > 50) {
        ui->terminalInput->removeItem(ui->terminalInput->count() - 1);
    }
    
    // 保存历史记录到配置
    QStringList history;
    for (int i = 0; i < ui->terminalInput->count(); ++i) {
        history.append(ui->terminalInput->itemText(i));
    }
    configManager->setTerminalHistory(history);
    configManager->saveConfig();
    
    qDebug() << "[MainWindow] Added terminal history:" << command;
}

void MainWindow::onHeaderCheckBoxToggled(bool checked)
{
    // 遍历所有快捷指令复选框，根据表头复选框的状态设置它们
    for (QCheckBox *checkbox : commandCheckboxes) {
        if (checkbox) {
            // 使用 blockSignals 防止递归回调
            checkbox->blockSignals(true);
            checkbox->setChecked(checked);
            checkbox->blockSignals(false);
        }
    }
    
    qDebug() << "[MainWindow] Header checkbox" << (checked ? "checked - 全选" : "unchecked - 取消全选");
}

void MainWindow::onSwitchToMain()
{
    // Show main window components (all components visible)
    ui->settingsGroupBox->setVisible(true);
    ui->hotkeysGroupBox->setVisible(true);
    ui->receivedDataGroupBox->setVisible(true);
    ui->commandScrollArea->setVisible(true);
    
    // Hide AT Command page
    if (atCommandPage) {
        atCommandPage->setVisible(false);
    }
    
    // Update window title
    setWindowTitle("SCOM-X");
    
    debugLog("[MainWindow] Switched to Main window");
    OperationLogger::instance().logPageSwitch("主页面");
}

void MainWindow::onSwitchToATCommand()
{
    // Hide main components
    ui->settingsGroupBox->setVisible(false);
    ui->hotkeysGroupBox->setVisible(false);
    ui->receivedDataGroupBox->setVisible(false);
    ui->commandScrollArea->setVisible(false);
    
    // Show AT Command page as a child widget with proper geometry
    if (atCommandPage) {
        atCommandPage->setParent(centralWidget());
        
        // 计算正确的几何大小：从菜单栏下方到状态栏上方
        QRect contentRect = centralWidget()->rect();
        atCommandPage->setGeometry(contentRect);
        
        atCommandPage->setVisible(true);
        atCommandPage->raise();
        atCommandPage->setFocus();
    }
    
    // Update window title
    setWindowTitle("SCOM-X - AT Command");
    
    debugLog("[MainWindow] Switched to AT Command window");
    OperationLogger::instance().logPageSwitch("AT 命令页面");
}


void MainWindow::onSwitchToLog()
{
    // Hide AT Command page
    if (atCommandPage) {
        atCommandPage->setVisible(false);
    }
    
    // Hide AT Command specific components
    ui->settingsGroupBox->setVisible(false);
    ui->hotkeysGroupBox->setVisible(false);
    ui->commandScrollArea->setVisible(false);
    
    // Show only received data
    ui->receivedDataGroupBox->setVisible(true);
    
    // Update window title
    setWindowTitle("SCOM-X - Log");
    
    debugLog("[MainWindow] Switched to Log window");
    OperationLogger::instance().logPageSwitch("日志页面");
}

void MainWindow::onSwitchToReceiveData()
{
    // Hide AT Command page
    if (atCommandPage) {
        atCommandPage->setVisible(false);
    }
    
    // Hide main components
    ui->settingsGroupBox->setVisible(false);
    ui->hotkeysGroupBox->setVisible(false);
    ui->commandScrollArea->setVisible(false);
    
    // Show Receive Data page with proper geometry
    if (receiveDataPage) {
        receiveDataPage->setParent(centralWidget());
        QRect contentRect = centralWidget()->rect();
        receiveDataPage->setGeometry(contentRect);
        
        receiveDataPage->setVisible(true);
        receiveDataPage->raise();
        receiveDataPage->setFocus();
    }
    
    // Update window title
    setWindowTitle("SCOM-X - Receive Data");
    
    debugLog("[MainWindow] Switched to Receive Data window");
    OperationLogger::instance().logPageSwitch("数据接收页面");
}

void MainWindow::onShowLogViewer()
{
    // 如果日志查看器还未创建，则创建它
    if (!logViewerDialog) {
        logViewerDialog = new LogViewerDialog(this);
    }
    
    // 显示日志查看器窗口
    logViewerDialog->show();
    logViewerDialog->raise();
    logViewerDialog->activateWindow();
    
    debugLog("[MainWindow] Opened Log Viewer");
}



