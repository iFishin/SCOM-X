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
#include <QComboBox>
#include <QTextEdit>

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
    // 清空旧的布局
    if (ui->pathGroupBox->layout()) {
        QLayout *oldLayout = ui->pathGroupBox->layout();
        while (QLayoutItem *item = oldLayout->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // 创建新的专业布局结构
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->pathGroupBox);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // ========== 标题栏 ==========
    QLabel *titleLabel = new QLabel("AT 命令文件管理");
    titleLabel->setStyleSheet("font-size: 12pt; font-weight: bold; color: #2196F3;");
    mainLayout->addWidget(titleLabel);
    
    // 分隔线
    QFrame *line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line1);

    // ========== 配置文件选择 ==========
    QHBoxLayout *selectorLayout = new QHBoxLayout();
    selectorLayout->setSpacing(10);
    
    QLabel *configLabel = new QLabel("配置文件:");
    configLabel->setMinimumWidth(80);
    selectorLayout->addWidget(configLabel);
    
    QComboBox *pathComboBox = new QComboBox();
    pathComboBox->setMinimumWidth(250);
    pathComboBox->setMaximumHeight(32);
    
    for (int i = 0; i < 16; ++i) {
        pathComboBox->addItem(QString("配置 #%1").arg(i + 1));
    }
    pathComboBox->setCurrentIndex(0);
    selectorLayout->addWidget(pathComboBox);
    
    QPushButton *browseButton = new QPushButton("浏览");
    browseButton->setMaximumWidth(90);
    browseButton->setMaximumHeight(32);
    selectorLayout->addWidget(browseButton);
    
    selectorLayout->addStretch();
    mainLayout->addLayout(selectorLayout);

    // ========== 文件路径显示 ==========
    QHBoxLayout *pathDisplayLayout = new QHBoxLayout();
    pathDisplayLayout->setSpacing(10);
    
    QLabel *filePathLabel = new QLabel("文件路径:");
    filePathLabel->setMinimumWidth(80);
    pathDisplayLayout->addWidget(filePathLabel);
    
    ClickableLineEdit *filePathEdit = new ClickableLineEdit();
    filePathEdit->setReadOnly(true);
    filePathEdit->setPlaceholderText("双击浏览文件...");
    filePathEdit->setText(pathConfigs[0]);
    filePathEdit->setMaximumHeight(32);
    filePathEdit->setStyleSheet(
        "QLineEdit {"
        "  border: 1px solid #BDBDBD;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "  background-color: #F5F5F5;"
        "  color: #616161;"
        "}"
    );
    pathDisplayLayout->addWidget(filePathEdit);
    mainLayout->addLayout(pathDisplayLayout);
    
    // 分隔线
    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line2);

    // ========== 命令编辑器标签 ==========
    QLabel *editorLabel = new QLabel("命令内容 (JSON格式)");
    editorLabel->setStyleSheet("font-weight: bold; color: #333333;");
    mainLayout->addWidget(editorLabel);

    // ========== 命令编辑器 ==========
    mainLayout->addWidget(ui->commandTextEdit, 1);

    // 分隔线
    QFrame *line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line3);

    // ========== 操作按钮工具栏 ==========
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    
    QPushButton *loadButton = new QPushButton("⟲ 加载");
    loadButton->setMinimumWidth(80);
    loadButton->setMaximumHeight(32);
    
    QPushButton *saveButton = new QPushButton("💾 保存");
    saveButton->setMinimumWidth(80);
    saveButton->setMaximumHeight(32);
    
    QPushButton *importButton = new QPushButton("📥 导入");
    importButton->setMinimumWidth(80);
    importButton->setMaximumHeight(32);
    
    QPushButton *exportButton = new QPushButton("📤 导出");
    exportButton->setMinimumWidth(80);
    exportButton->setMaximumHeight(32);
    
    QPushButton *formatButton = new QPushButton("✎ 格式化");
    formatButton->setMinimumWidth(80);
    formatButton->setMaximumHeight(32);
    
    QPushButton *validateButton = new QPushButton("✓ 验证");
    validateButton->setMinimumWidth(80);
    validateButton->setMaximumHeight(32);
    
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(importButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addWidget(formatButton);
    buttonLayout->addWidget(validateButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);

    // ========== 状态栏 ==========
    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->setSpacing(10);
    
    QLabel *statusIconLabel = new QLabel("●");
    statusIconLabel->setStyleSheet("color: #4CAF50; font-size: 14pt;");
    statusLayout->addWidget(statusIconLabel);
    
    QLabel *statusLabel = new QLabel("就绪");
    statusLabel->setStyleSheet("color: #333333; font-weight: bold;");
    statusLayout->addWidget(statusLabel);
    
    statusLayout->addStretch();
    
    QLabel *sizeLabel = new QLabel("行数: 0");
    sizeLabel->setStyleSheet("color: #666666;");
    statusLayout->addWidget(sizeLabel);
    
    mainLayout->addLayout(statusLayout);

    // 保存指针供后续使用
    pathRadioButtons.clear();
    pathInputs.clear();
    
    // 连接信号
    connect(pathComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, pathComboBox, filePathEdit](int index) {
        currentPathIndex = index;
        loadATCommandFile(index);
        filePathEdit->setText(pathConfigs[index]);
    });
    
    connect(browseButton, &QPushButton::clicked, this, [this, pathComboBox, filePathEdit]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择 AT 命令文件", "", "JSON 文件 (*.json)");
        if (!fileName.isEmpty()) {
            pathConfigs[pathComboBox->currentIndex()] = fileName;
            filePathEdit->setText(fileName);
            loadATCommandFile(pathComboBox->currentIndex());
        }
    });
    
    connect(filePathEdit, &ClickableLineEdit::doubleClicked, this, [this, browseButton]() {
        browseButton->click();
    });
    
    connect(loadButton, &QPushButton::clicked, this, [this]() {
        loadATCommandFile(currentPathIndex);
    });
    
    connect(saveButton, &QPushButton::clicked, this, &ATCommandPage::onSaveClicked);
    connect(importButton, &QPushButton::clicked, this, &ATCommandPage::onImportClicked);
    
    connect(formatButton, &QPushButton::clicked, this, [this, statusLabel, statusIconLabel]() {
        // JSON 格式化
        QString text = ui->commandTextEdit->toPlainText();
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
        if (doc.isNull()) {
            statusLabel->setText("JSON 格式错误");
            statusIconLabel->setStyleSheet("color: #F44336; font-size: 14pt;");  // 红色
        } else {
            ui->commandTextEdit->setPlainText(QString::fromUtf8(doc.toJson()));
            statusLabel->setText("格式化完成");
            statusIconLabel->setStyleSheet("color: #4CAF50; font-size: 14pt;");  // 绿色
        }
    });
    
    connect(validateButton, &QPushButton::clicked, this, [this, statusLabel, statusIconLabel]() {
        QString text = ui->commandTextEdit->toPlainText();
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
        if (doc.isNull()) {
            statusLabel->setText("JSON 格式错误");
            statusIconLabel->setStyleSheet("color: #F44336; font-size: 14pt;");  // 红色
        } else {
            statusLabel->setText("JSON 格式正确");
            statusIconLabel->setStyleSheet("color: #4CAF50; font-size: 14pt;");  // 绿色
        }
    });
    
    connect(ui->commandTextEdit, &QTextEdit::textChanged, this, [this, sizeLabel]() {
        int lineCount = ui->commandTextEdit->document()->blockCount();
        sizeLabel->setText(QString("行数: %1").arg(lineCount));
    });
}

void ATCommandPage::setupConnections()
{
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
    // This function is no longer used - expand button removed from UI
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
    // File status label has been removed from UI
    // This function is kept for compatibility but does nothing
}

void ATCommandPage::setPathRadioButtonChecked(int index)
{
    if (index >= 0 && index < static_cast<int>(pathRadioButtons.size())) {
        pathRadioButtons[index]->setChecked(true);
    }
}

