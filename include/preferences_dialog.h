#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <memory>

class ConfigManager;

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr, ConfigManager *configManager = nullptr);
    ~PreferencesDialog();

private slots:
    void onApplyClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    void loadSettings();
    void applySettings();

    // 串口设置控件（波特率和端口在主界面配置）
    QComboBox *dataBitsComboBox;
    QComboBox *parityComboBox;
    QComboBox *stopBitsComboBox;
    QCheckBox *dtrCheckBox;
    QCheckBox *rtsCheckBox;
    QComboBox *flowControlComboBox;

    // 按钮
    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *cancelButton;

    // 配置管理器指针
    ConfigManager *configManager;

    // 临时保存应用设置的标志
    bool settingsApplied;
};

#endif // PREFERENCES_DIALOG_H
