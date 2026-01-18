#ifndef AT_COMMAND_PAGE_H
#define AT_COMMAND_PAGE_H

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QLabel>
#include <memory>
#include <vector>

namespace Ui {
    class ATCommandPage;
}

class ConfigManager;

class ClickableLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit ClickableLineEdit(QWidget *parent = nullptr);
    
signals:
    void doubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

/**
 * @class ATCommandPage
 * @brief AT Command File Editor Page
 * 
 * Provides interface for editing multiple AT command JSON files
 * with path selection and file management capabilities
 */
class ATCommandPage : public QWidget {
    Q_OBJECT

public:
    explicit ATCommandPage(ConfigManager *configManager, QWidget *parent = nullptr);
    ~ATCommandPage();

private slots:
    // File operations
    void onSaveClicked();
    void onImportClicked();
    void onExpandClicked();
    
    // Path management
    void onRadioButtonToggled(int index, bool checked);
    void onPathInputDoubleClicked(int index);
    void onPathInputTextChanged(int index);
    
    // Text editing
    void onTextChanged();

private:
    // Initialize UI components
    void initializePathSelector();
    void setupConnections();
    void loadPathConfigs();
    void loadATCommandFile(int pathIndex);
    void saveATCommandFile();
    
    // File handling
    void selectJsonFile(QLineEdit *pathInput);
    QString getATCommandFilePath() const;
    
    // UI updates
    void updateFileStatus();
    void setPathRadioButtonChecked(int index);

    std::unique_ptr<Ui::ATCommandPage> ui;
    ConfigManager *configManager;
    
    // Path management
    std::vector<QString> pathConfigs;
    std::vector<QRadioButton*> pathRadioButtons;
    std::vector<ClickableLineEdit*> pathInputs;
    int currentPathIndex = 0;
    
    // Status tracking
    bool textModified = false;
};

#endif // AT_COMMAND_PAGE_H

