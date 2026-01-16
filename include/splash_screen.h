#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QScreen>

class SplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    explicit SplashScreen(const QPixmap &pixmap = QPixmap(), QWidget *parent = nullptr);
    ~SplashScreen();

    /**
     * @brief 显示加载消息并更新进度
     * @param message 消息文本
     * @param progress 进度（0-100）
     */
    void showLoadingMessage(const QString &message, int progress = -1);

    /**
     * @brief 获取当前进度
     */
    int currentProgress() const;

protected:
    void drawContents(QPainter *painter) override;

private:
    void setupUI();
    void setupStyle();

    QString currentMessage;
    int currentProgressValue;
    QStringList loadingSteps;
    int currentStep;
};

#endif // SPLASH_SCREEN_H
