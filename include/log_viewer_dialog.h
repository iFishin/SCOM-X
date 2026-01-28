#ifndef LOG_VIEWER_DIALOG_H
#define LOG_VIEWER_DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QTabWidget>

class LogViewerDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogViewerDialog(QWidget *parent = nullptr);
    ~LogViewerDialog();

public slots:
    void addLogMessage(const QString &message);
    void clear();

private slots:
    void onRefreshClicked();
    void onClearClicked();
    void onExportClicked();
    void onSearchChanged();
    void onAutoScrollToggled(bool checked);
    void onLogMessageAdded(const QString &message);
    void updateLogDisplay();

private:
    void setupUI();
    void loadLogsFromFile();
    void applySearchFilter();

    // 标签页
    QTabWidget *tabWidget;
    
    // 调试日志标签页
    QPlainTextEdit *debugLogDisplay;
    QLineEdit *debugSearchLineEdit;
    
    // 操作日志标签页
    QPlainTextEdit *operationLogDisplay;
    QLineEdit *operationSearchLineEdit;
    QCheckBox *operationAutoScrollCheckBox;
    
    // 通用控件
    QPushButton *refreshButton;
    QPushButton *clearButton;
    QPushButton *exportButton;
    QPushButton *closeButton;
    QCheckBox *autoScrollCheckBox;
    QLabel *statusLabel;
    QTimer refreshTimer;

    QString allDebugLogsContent;
    QString allOperationLogsContent;
    QString debugLogFilePath;
    QString operationLogFilePath;
};

#endif // LOG_VIEWER_DIALOG_H
