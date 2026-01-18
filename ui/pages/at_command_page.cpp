#include "at_command_page.h"
#include "ui_at_command_page.h"
#include "config_manager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <QMouseEvent>

// ClickableLineEdit implementation
ClickableLineEdit::ClickableLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

void ClickableLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubleClicked();
    QLineEdit::mouseDoubleClickEvent(event);
}

// ATCommandPage implementation
ATCommandPage::ATCommandPage(ConfigManager *configManager, QWidget *parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::ATCommandPage>())
    , configManager(configManager)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowFlags(Qt::Widget);
    
    // Initialize path configurations
    loadPathConfigs();
    initializePathSelector();
    setupConnections();
    
    // Load initial file
    if (configManager) {
        loadATCommandFile(0);
    }
    
    qDebug() << "[ATCommandPage] Initialized";
}

ATCommandPage::~ATCommandPage()
{
    // ui is automatically deleted via unique_ptr
}

void ATCommandPage::loadPathConfigs()
{
    pathConfigs.clear();
    
    // Set default path for first slot
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tmps/ATCommand.json";
    pathConfigs.push_back(defaultPath);
    
    // Set empty paths for remaining slots
    for (int i = 1; i < 16; ++i) {
        pathConfigs.push_back("");
    }
}

void ATCommandPage::initializePathSelector()
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->pathGroupBox->layout());
    if (!gridLayout) {
        return;
    }
    
    pathRadioButtons.clear();
    pathInputs.clear();
    
    for (int i = 0; i < 16; ++i) {
        // Create radio button
        QRadioButton *radioButton = new QRadioButton(QString("Path %1").arg(i + 1));
        connect(radioButton, &QRadioButton::toggled, this, [this, i](bool checked) {
            if (checked) {
                onRadioButtonToggled(i, true);
            }
        });
        
        // Create path input using ClickableLineEdit
        ClickableLineEdit *pathInput = new ClickableLineEdit();
        pathInput->setPlaceholderText("Double click to select JSON file");
        pathInput->setText(pathConfigs[i]);
        
        connect(pathInput, &QLineEdit::textChanged, this, [this, i](const QString &text) {
            onPathInputTextChanged(i);
        });
        
        connect(pathInput, &ClickableLineEdit::doubleClicked, this, [this, i]() {
            onPathInputDoubleClicked(i);
        });
        
        // Add to layout
        gridLayout->addWidget(radioButton, i, 0);
        gridLayout->addWidget(pathInput, i, 1);
        
        pathRadioButtons.push_back(radioButton);
        pathInputs.push_back(pathInput);
    }
    
    // Set first button as checked
    if (!pathRadioButtons.empty()) {
        pathRadioButtons[0]->setChecked(true);
        currentPathIndex = 0;
    }
    
    // Add stretch at the end
    gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 16, 0, 1, 2);
}

void ATCommandPage::setupConnections()
{
    connect(ui->saveButton, &QPushButton::clicked, this, &ATCommandPage::onSaveClicked);
    connect(ui->importButton, &QPushButton::clicked, this, &ATCommandPage::onImportClicked);
    connect(ui->expandButton, &QPushButton::clicked, this, &ATCommandPage::onExpandClicked);
    connect(ui->commandTextEdit, &QTextEdit::textChanged, this, &ATCommandPage::onTextChanged);
    
    qDebug() << "[ATCommandPage] Connections established";
}

void ATCommandPage::loadATCommandFile(int pathIndex)
{
    if (pathIndex < 0 || pathIndex >= static_cast<int>(pathConfigs.size())) {
        return;
    }
    
    currentPathIndex = pathIndex;
    
    QString filePath = pathConfigs[pathIndex];
    if (filePath.isEmpty()) {
        ui->commandTextEdit->clear();
        updateFileStatus();
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[ATCommandPage] Failed to open file:" << filePath;
        updateFileStatus();
        return;
    }
    
    QByteArray fileContent = file.readAll();
    file.close();
    
    ui->commandTextEdit->setPlainText(QString::fromUtf8(fileContent));
    textModified = false;
    updateFileStatus();
    
    qDebug() << "[ATCommandPage] Loaded AT command file:" << filePath;
}

void ATCommandPage::saveATCommandFile()
{
    QString filePath = getATCommandFilePath();
    if (filePath.isEmpty()) {
        qWarning() << "[ATCommandPage] No file path selected";
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[ATCommandPage] Failed to save file:" << filePath;
        return;
    }
    
    QString content = ui->commandTextEdit->toPlainText();
    file.write(content.toUtf8());
    file.close();
    
    textModified = false;
    updateFileStatus();
    
    qDebug() << "[ATCommandPage] Saved AT command file:" << filePath;
}

void ATCommandPage::onSaveClicked()
{
    saveATCommandFile();
}

void ATCommandPage::onImportClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import AT Command File",
        "",
        "JSON Files (*.json);;All Files (*)"
    );
    
    if (!filePath.isEmpty() && currentPathIndex >= 0 && currentPathIndex < static_cast<int>(pathInputs.size())) {
        pathInputs[currentPathIndex]->setText(filePath);
        pathConfigs[currentPathIndex] = filePath;
        loadATCommandFile(currentPathIndex);
    }
}

void ATCommandPage::onExpandClicked()
{
    bool visible = ui->pathScrollArea->isVisible();
    ui->pathScrollArea->setVisible(!visible);
    qDebug() << "[ATCommandPage] Path panel toggled:" << !visible;
}

void ATCommandPage::onRadioButtonToggled(int index, bool checked)
{
    if (checked) {
        // Save current file before switching
        if (textModified) {
            saveATCommandFile();
        }
        
        loadATCommandFile(index);
        qDebug() << "[ATCommandPage] Switched to path" << (index + 1);
    }
}

void ATCommandPage::onPathInputDoubleClicked(int index)
{
    if (index >= 0 && index < static_cast<int>(pathInputs.size())) {
        selectJsonFile(pathInputs[index]);
    }
}

void ATCommandPage::onPathInputTextChanged(int index)
{
    if (index == currentPathIndex) {
        if (index >= 0 && index < static_cast<int>(pathConfigs.size())) {
            pathConfigs[index] = pathInputs[index]->text();
        }
    }
}

void ATCommandPage::onTextChanged()
{
    textModified = true;
    updateFileStatus();
}

void ATCommandPage::selectJsonFile(QLineEdit *pathInput)
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select AT Command JSON File",
        "",
        "JSON Files (*.json);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        pathInput->setText(filePath);
    }
}

QString ATCommandPage::getATCommandFilePath() const
{
    if (currentPathIndex >= 0 && currentPathIndex < static_cast<int>(pathConfigs.size())) {
        return pathConfigs[currentPathIndex];
    }
    return "";
}

void ATCommandPage::updateFileStatus()
{
    QString statusText = getATCommandFilePath();
    if (textModified) {
        statusText += " *";
    }
    
    ui->fileStatusLabel->setText(statusText);
}

void ATCommandPage::setPathRadioButtonChecked(int index)
{
    if (index >= 0 && index < static_cast<int>(pathRadioButtons.size())) {
        pathRadioButtons[index]->setChecked(true);
    }
}

