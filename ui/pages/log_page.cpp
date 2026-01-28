#include "log_page.h"
#include "log_manager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

LogPage::LogPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    connectSignals();
}

LogPage::~LogPage()
{
}

void LogPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // 搜索工具栏
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("搜索日志...");
    searchLineEdit->setMaximumWidth(300);
    
    refreshButton = new QPushButton("刷新", this);
    refreshButton->setMaximumWidth(80);
    
    clearButton = new QPushButton("清空", this);
    clearButton->setMaximumWidth(80);
    
    exportButton = new QPushButton("导出", this);
    exportButton->setMaximumWidth(80);
    
    statsLabel = new QLabel("已加载 0 条日志", this);
    
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addStretch();
    searchLayout->addWidget(refreshButton);
    searchLayout->addWidget(clearButton);
    searchLayout->addWidget(exportButton);
    
    mainLayout->addLayout(searchLayout);
    
    // 日志显示区域
    logDisplay = new QPlainTextEdit(this);
    logDisplay->setReadOnly(true);
    logDisplay->setFont(QFont("Consolas", 10));
    mainLayout->addWidget(logDisplay);
    
    // 状态栏
    mainLayout->addWidget(statsLabel);
    
    setLayout(mainLayout);
}

void LogPage::connectSignals()
{
    connect(refreshButton, &QPushButton::clicked, this, &LogPage::onRefreshButtonClicked);
    connect(clearButton, &QPushButton::clicked, this, &LogPage::onClearLogsButtonClicked);
    connect(exportButton, &QPushButton::clicked, this, &LogPage::onExportButtonClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &LogPage::onSearchTextChanged);
}

void LogPage::refreshLogs()
{
    fullLogContent = LogManager::instance().getLastLogs(10000);
    
    logDisplay->setPlainText(fullLogContent);
    
    int lineCount = fullLogContent.split("\n").size();
    statsLabel->setText(QString("已加载 %1 条日志").arg(lineCount));
    
    // 滚动到底部
    logDisplay->moveCursor(logDisplay->textCursor().MoveOperation::End);
}

void LogPage::onRefreshButtonClicked()
{
    refreshLogs();
}

void LogPage::onClearLogsButtonClicked()
{
    if (QMessageBox::question(this, "确认清空", "确定要清空所有日志吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        logDisplay->clear();
        fullLogContent.clear();
        statsLabel->setText("已加载 0 条日志");
    }
}

void LogPage::onExportButtonClicked()
{
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (docsPath.isEmpty()) {
        docsPath = "./";
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "导出日志",
        docsPath + "/SCOM-X_logs.txt",
        "文本文件 (*.txt);;日志文件 (*.log)"
    );
    
    if (!fileName.isEmpty()) {
        LogManager::instance().exportLogs(fileName);
        QMessageBox::information(this, "成功", QString("日志已导出到：\n%1").arg(fileName));
    }
}

void LogPage::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        logDisplay->setPlainText(fullLogContent);
        return;
    }
    
    QStringList lines = fullLogContent.split("\n", Qt::KeepEmptyParts);
    QStringList filteredLines;
    
    for (const QString &line : lines) {
        if (line.contains(text, Qt::CaseInsensitive)) {
            filteredLines.append(line);
        }
    }
    
    logDisplay->setPlainText(filteredLines.join("\n"));
}
