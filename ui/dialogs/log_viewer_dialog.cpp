#include "log_viewer_dialog.h"
#include "operation_logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QApplication>
#include <QTabWidget>

LogViewerDialog::LogViewerDialog(QWidget *parent)
    : QDialog(parent)
{
    // 设置日志文件路径
    debugLogFilePath = QString("%1/.config/SCOM-X/debug.log").arg(QDir::homePath());
    operationLogFilePath = QString("%1/.config/SCOM-X/operation.log").arg(QDir::homePath());

    setWindowTitle("程序日志");
    setGeometry(200, 200, 1000, 700);
    
    setupUI();
    loadLogsFromFile();
    
    // 连接操作日志信号
    connect(&OperationLogger::instance(), &OperationLogger::logMessageAdded, 
            this, &LogViewerDialog::onLogMessageAdded);
    
    // 设置自动刷新（每1秒检查一次日志更新）
    connect(&refreshTimer, &QTimer::timeout, this, &LogViewerDialog::updateLogDisplay);
    refreshTimer.start(1000);
}

LogViewerDialog::~LogViewerDialog()
{
    refreshTimer.stop();
}

void LogViewerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 搜索和控制工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    QLabel *searchLabel = new QLabel("搜索:");
    QLineEdit *searchLineEdit = new QLineEdit();
    searchLineEdit->setPlaceholderText("输入关键词搜索日志...");
    searchLineEdit->setMaximumWidth(300);
    
    toolbarLayout->addWidget(searchLabel);
    toolbarLayout->addWidget(searchLineEdit);
    toolbarLayout->addStretch();

    refreshButton = new QPushButton("刷新");
    refreshButton->setMaximumWidth(80);
    connect(refreshButton, &QPushButton::clicked, this, &LogViewerDialog::onRefreshClicked);
    
    clearButton = new QPushButton("清空");
    clearButton->setMaximumWidth(80);
    connect(clearButton, &QPushButton::clicked, this, &LogViewerDialog::onClearClicked);
    
    exportButton = new QPushButton("导出");
    exportButton->setMaximumWidth(80);
    connect(exportButton, &QPushButton::clicked, this, &LogViewerDialog::onExportClicked);

    toolbarLayout->addWidget(refreshButton);
    toolbarLayout->addWidget(clearButton);
    toolbarLayout->addWidget(exportButton);

    mainLayout->addLayout(toolbarLayout);

    // 标签页用于显示不同类型的日志
    tabWidget = new QTabWidget();

    // 调试日志标签页
    QWidget *debugLogTab = new QWidget();
    QVBoxLayout *debugLogLayout = new QVBoxLayout(debugLogTab);
    debugLogLayout->setContentsMargins(5, 5, 5, 5);
    
    QHBoxLayout *debugSearchLayout = new QHBoxLayout();
    QLabel *debugSearchLabel = new QLabel("搜索调试日志:");
    debugSearchLineEdit = new QLineEdit();
    debugSearchLineEdit->setPlaceholderText("输入关键词...");
    debugSearchLayout->addWidget(debugSearchLabel);
    debugSearchLayout->addWidget(debugSearchLineEdit);
    debugLogLayout->addLayout(debugSearchLayout);
    
    debugLogDisplay = new QPlainTextEdit();
    debugLogDisplay->setReadOnly(true);
    debugLogDisplay->setFont(QFont("Consolas", 10));
    debugLogDisplay->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #d4d4d4;"
        "  border: 1px solid #333333;"
        "}"
    );
    debugLogLayout->addWidget(debugLogDisplay, 1);
    
    connect(debugSearchLineEdit, &QLineEdit::textChanged, this, [this]() {
        applySearchFilter();
    });

    tabWidget->addTab(debugLogTab, "调试日志");

    // 操作日志标签页
    QWidget *operationLogTab = new QWidget();
    QVBoxLayout *operationLogLayout = new QVBoxLayout(operationLogTab);
    operationLogLayout->setContentsMargins(5, 5, 5, 5);
    
    QHBoxLayout *operationSearchLayout = new QHBoxLayout();
    QLabel *operationSearchLabel = new QLabel("搜索操作日志:");
    operationSearchLineEdit = new QLineEdit();
    operationSearchLineEdit->setPlaceholderText("输入关键词...");
    operationSearchLineEdit->setMaximumWidth(400);
    operationSearchLayout->addWidget(operationSearchLabel);
    operationSearchLayout->addWidget(operationSearchLineEdit);
    operationSearchLayout->addStretch();
    
    operationAutoScrollCheckBox = new QCheckBox("自动滚动");
    operationAutoScrollCheckBox->setChecked(true);
    operationSearchLayout->addWidget(operationAutoScrollCheckBox);
    
    operationLogLayout->addLayout(operationSearchLayout);
    
    operationLogDisplay = new QPlainTextEdit();
    operationLogDisplay->setReadOnly(true);
    operationLogDisplay->setFont(QFont("Consolas", 10));
    operationLogDisplay->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #00ff00;"
        "  border: 1px solid #333333;"
        "}"
    );
    operationLogLayout->addWidget(operationLogDisplay, 1);
    
    connect(operationSearchLineEdit, &QLineEdit::textChanged, this, [this]() {
        applySearchFilter();
    });

    tabWidget->addTab(operationLogTab, "操作日志");

    mainLayout->addWidget(tabWidget, 1);

    // 状态栏
    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLabel = new QLabel("就绪");
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    mainLayout->addLayout(statusLayout);

    // 关闭按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    closeButton = new QPushButton("关闭");
    closeButton->setMaximumWidth(100);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);
}

void LogViewerDialog::loadLogsFromFile()
{
    // 加载调试日志
    QFile debugFile(debugLogFilePath);
    if (!debugFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusLabel->setText(QString("无法打开调试日志: %1").arg(debugLogFilePath));
        return;
    }

    QTextStream debugIn(&debugFile);
    debugIn.setEncoding(QStringConverter::Utf8);
    allDebugLogsContent = debugIn.readAll();
    debugFile.close();

    // 加载操作日志
    QFile operationFile(operationLogFilePath);
    if (operationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream operationIn(&operationFile);
        operationIn.setEncoding(QStringConverter::Utf8);
        allOperationLogsContent = operationIn.readAll();
        operationFile.close();
    }

    updateLogDisplay();
}

void LogViewerDialog::updateLogDisplay()
{
    applySearchFilter();
}

void LogViewerDialog::applySearchFilter()
{
    // 处理调试日志搜索
    QString debugSearchText = debugSearchLineEdit->text();
    
    if (debugSearchText.isEmpty()) {
        debugLogDisplay->setPlainText(allDebugLogsContent);
    } else {
        QStringList filteredLines;
        const QStringList &lines = allDebugLogsContent.split('\n');
        
        for (const QString &line : lines) {
            if (line.contains(debugSearchText, Qt::CaseInsensitive)) {
                filteredLines.append(line);
            }
        }
        
        debugLogDisplay->setPlainText(filteredLines.join('\n'));
    }

    // 处理操作日志搜索
    QString operationSearchText = operationSearchLineEdit->text();
    
    if (operationSearchText.isEmpty()) {
        operationLogDisplay->setPlainText(allOperationLogsContent);
    } else {
        QStringList filteredLines;
        const QStringList &lines = allOperationLogsContent.split('\n');
        
        for (const QString &line : lines) {
            if (line.contains(operationSearchText, Qt::CaseInsensitive)) {
                filteredLines.append(line);
            }
        }
        
        operationLogDisplay->setPlainText(filteredLines.join('\n'));
    }

    // 自动滚动到底部
    if (operationAutoScrollCheckBox && operationAutoScrollCheckBox->isChecked()) {
        QTextCursor cursor = operationLogDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        operationLogDisplay->setTextCursor(cursor);
    }
}

void LogViewerDialog::addLogMessage(const QString &message)
{
    operationLogDisplay->appendPlainText(message);
    
    if (operationAutoScrollCheckBox && operationAutoScrollCheckBox->isChecked()) {
        QTextCursor cursor = operationLogDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        operationLogDisplay->setTextCursor(cursor);
    }
}

void LogViewerDialog::onRefreshClicked()
{
    loadLogsFromFile();
    statusLabel->setText("已刷新");
}

void LogViewerDialog::onClearClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认清空",
        "确定要清空所有日志吗?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // 清空调试日志
        QFile debugFile(debugLogFilePath);
        if (debugFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            debugFile.close();
        }
        
        // 清空操作日志
        QFile operationFile(operationLogFilePath);
        if (operationFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            operationFile.close();
        }
        
        allDebugLogsContent.clear();
        allOperationLogsContent.clear();
        debugLogDisplay->clear();
        operationLogDisplay->clear();
        statusLabel->setText("已清空日志");
    }
}

void LogViewerDialog::onExportClicked()
{
    int currentTab = tabWidget->currentIndex();
    QString defaultName = currentTab == 0 ? "debug_log" : "operation_log";
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "导出日志",
        QDir::homePath() + QString("/%1_%2.txt").arg(defaultName).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "文本文件 (*.txt)"
    );
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            
            if (currentTab == 0) {
                out << allDebugLogsContent;
            } else {
                out << allOperationLogsContent;
            }
            
            file.close();
            QMessageBox::information(this, "导出成功", QString("日志已导出到: %1").arg(fileName));
            statusLabel->setText("已导出日志");
        } else {
            QMessageBox::warning(this, "导出失败", "无法保存日志文件");
        }
    }
}

void LogViewerDialog::onSearchChanged()
{
    applySearchFilter();
}

void LogViewerDialog::onAutoScrollToggled(bool checked)
{
    if (checked) {
        QTextCursor cursor = operationLogDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        operationLogDisplay->setTextCursor(cursor);
    }
}

void LogViewerDialog::onLogMessageAdded(const QString &message)
{
    allOperationLogsContent.append(message + "\n");
    
    if (tabWidget->currentIndex() == 1) {  // 如果当前在操作日志标签
        addLogMessage(message);
    }
}

void LogViewerDialog::clear()
{
    debugLogDisplay->clear();
    operationLogDisplay->clear();
    allDebugLogsContent.clear();
    allOperationLogsContent.clear();
    statusLabel->setText("已清空日志");
}
