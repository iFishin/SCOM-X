#include "preferences_dialog.h"
#include "config_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent, ConfigManager *configManager)
    : QDialog(parent), configManager(configManager), settingsApplied(false) {
    setWindowTitle("首选项");
    setModal(true);
    setMinimumWidth(400);
    setupUI();
    loadSettings();
}

PreferencesDialog::~PreferencesDialog() {
}

void PreferencesDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 串口设置分组（波特率和端口在主界面配置）
    QGroupBox *serialGroupBox = new QGroupBox("串口高级设置", this);
    QVBoxLayout *serialLayout = new QVBoxLayout();

    // 数据位
    QHBoxLayout *dataBitsLayout = new QHBoxLayout();
    dataBitsLayout->addWidget(new QLabel("数据位:"));
    dataBitsComboBox = new QComboBox();
    dataBitsComboBox->addItems({"5", "6", "7", "8"});
    dataBitsLayout->addWidget(dataBitsComboBox);
    dataBitsLayout->addStretch();
    serialLayout->addLayout(dataBitsLayout);

    // 奇偶校验
    QHBoxLayout *parityLayout = new QHBoxLayout();
    parityLayout->addWidget(new QLabel("奇偶校验:"));
    parityComboBox = new QComboBox();
    parityComboBox->addItems({"None", "Even", "Odd", "Space", "Mark"});
    parityLayout->addWidget(parityComboBox);
    parityLayout->addStretch();
    serialLayout->addLayout(parityLayout);

    // 停止位
    QHBoxLayout *stopBitsLayout = new QHBoxLayout();
    stopBitsLayout->addWidget(new QLabel("停止位:"));
    stopBitsComboBox = new QComboBox();
    stopBitsComboBox->addItems({"1", "1.5", "2"});
    stopBitsLayout->addWidget(stopBitsComboBox);
    stopBitsLayout->addStretch();
    serialLayout->addLayout(stopBitsLayout);

    // DTR 复选框
    dtrCheckBox = new QCheckBox("DTR (数据终端就绪)");
    serialLayout->addWidget(dtrCheckBox);

    // RTS 复选框
    rtsCheckBox = new QCheckBox("RTS (请求发送)");
    serialLayout->addWidget(rtsCheckBox);

    // 流控制
    QHBoxLayout *flowControlLayout = new QHBoxLayout();
    flowControlLayout->addWidget(new QLabel("流控制:"));
    flowControlComboBox = new QComboBox();
    flowControlComboBox->addItems({"None", "RTS/CTS", "XON/XOFF"});
    flowControlLayout->addWidget(flowControlComboBox);
    flowControlLayout->addStretch();
    serialLayout->addLayout(flowControlLayout);

    serialGroupBox->setLayout(serialLayout);
    mainLayout->addWidget(serialGroupBox);

    mainLayout->addStretch();

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    applyButton = new QPushButton("应用");
    okButton = new QPushButton("确定");
    cancelButton = new QPushButton("取消");

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // 连接信号
    connect(applyButton, &QPushButton::clicked, this, &PreferencesDialog::onApplyClicked);
    connect(okButton, &QPushButton::clicked, this, &PreferencesDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &PreferencesDialog::onCancelClicked);
}

void PreferencesDialog::loadSettings() {
    if (!configManager) {
        qWarning() << "[PreferencesDialog] ConfigManager not provided";
        return;
    }

    // 加载串口高级设置（端口和波特率在主界面配置）
    dataBitsComboBox->setCurrentText(QString::number(configManager->getDataBits()));
    parityComboBox->setCurrentText(configManager->getParity());
    stopBitsComboBox->setCurrentText(configManager->getStopBits());
    dtrCheckBox->setChecked(configManager->getDTR());
    rtsCheckBox->setChecked(configManager->getRTS());
    flowControlComboBox->setCurrentText(configManager->getFlowControl());

    qDebug() << "[PreferencesDialog] Settings loaded from config";
}

void PreferencesDialog::applySettings() {
    if (!configManager) {
        return;
    }

    // 应用串口高级设置（端口和波特率在主界面配置）
    configManager->setDataBits(dataBitsComboBox->currentText().toInt());
    configManager->setParity(parityComboBox->currentText());
    configManager->setStopBits(stopBitsComboBox->currentText());
    configManager->setDTR(dtrCheckBox->isChecked());
    configManager->setRTS(rtsCheckBox->isChecked());
    configManager->setFlowControl(flowControlComboBox->currentText());

    // 保存配置到文件
    if (configManager->saveConfig()) {
        qDebug() << "[PreferencesDialog] Settings applied and saved";
        settingsApplied = true;
    } else {
        qWarning() << "[PreferencesDialog] Failed to save settings";
    }
}

void PreferencesDialog::onApplyClicked() {
    applySettings();
}

void PreferencesDialog::onOkClicked() {
    applySettings();
    accept();
}

void PreferencesDialog::onCancelClicked() {
    reject();
}
