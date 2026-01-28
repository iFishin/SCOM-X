#ifndef LOG_PAGE_H
#define LOG_PAGE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

/**
 * @class LogPage
 * @brief 日志显示页面
 * 
 * 用于查看和管理应用日志的专门页面
 */
class LogPage : public QWidget {
    Q_OBJECT

public:
    explicit LogPage(QWidget *parent = nullptr);
    ~LogPage();

    // 刷新日志显示
    void refreshLogs();

private slots:
    void onRefreshButtonClicked();
    void onClearLogsButtonClicked();
    void onExportButtonClicked();
    void onSearchTextChanged(const QString &text);

private:
    void setupUI();
    void connectSignals();

    // UI 组件
    QLineEdit *searchLineEdit = nullptr;
    QPushButton *refreshButton = nullptr;
    QPushButton *clearButton = nullptr;
    QPushButton *exportButton = nullptr;
    QPlainTextEdit *logDisplay = nullptr;
    QLabel *statsLabel = nullptr;

    // 完整的日志内容（用于搜索和过滤）
    QString fullLogContent;
};

#endif // LOG_PAGE_H
