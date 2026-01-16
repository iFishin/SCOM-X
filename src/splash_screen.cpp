#include "splash_screen.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QThread>

SplashScreen::SplashScreen(const QPixmap &pixmap, QWidget *parent)
    : QSplashScreen(pixmap)
    , currentProgressValue(0)
    , currentStep(0)
{
    // 如果没有提供pixmap，创建一个默认的
    if (pixmap.isNull())
    {
        QPixmap defaultPixmap(600, 400);
        defaultPixmap.fill(Qt::white);
        setPixmap(defaultPixmap);
    }

    // 设置窗口位置（居中）
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // 设置无边框窗口
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: #ffffff;");

    setupStyle();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::setupStyle()
{
    // 预定义加载步骤
    loadingSteps << "初始化应用程序..."
                 << "加载配置文件..."
                 << "加载样式表..."
                 << "初始化串口..."
                 << "建立信号连接..."
                 << "加载用户设置..."
                 << "准备完毕!";
}

void SplashScreen::showLoadingMessage(const QString &message, int progress)
{
    currentMessage = message;

    if (progress >= 0)
    {
        currentProgressValue = qMin(100, qMax(0, progress));
    }
    else
    {
        currentProgressValue = qMin(100, currentProgressValue + 15);
    }

    // 强制重绘
    repaint();

    // 处理事件，确保界面响应
    QApplication::processEvents();

    // 短暂延迟以显示进度
    QThread::msleep(100);
}

int SplashScreen::currentProgress() const
{
    return currentProgressValue;
}

void SplashScreen::drawContents(QPainter *painter)
{
    // 绘制背景渐变
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor("#f0f7ff"));
    gradient.setColorAt(1, QColor("#ffffff"));
    painter->fillRect(rect(), gradient);

    // 绘制顶部标题
    QFont titleFont("Segoe UI", 28, QFont::Bold);
    painter->setFont(titleFont);
    painter->setPen(QColor("#00a86b"));
    painter->drawText(QRect(0, 40, width(), 60), Qt::AlignCenter, "SCOM-X");

    // 绘制版本信息
    QFont versionFont("Segoe UI", 10);
    painter->setFont(versionFont);
    painter->setPen(QColor("#666666"));
    painter->drawText(QRect(0, 100, width(), 30), Qt::AlignCenter, "v1.0.0");

    // 绘制加载信息
    QFont messageFont("Segoe UI", 11);
    painter->setFont(messageFont);
    painter->setPen(QColor("#333333"));

    int messageY = height() / 2 + 40;
    painter->drawText(QRect(40, messageY, width() - 80, 40), Qt::AlignLeft | Qt::AlignVCenter,
                      currentMessage.isEmpty() ? "启动中..." : currentMessage);

    // 绘制进度条背景
    int progressBarWidth = width() - 80;
    int progressBarHeight = 8;
    int progressBarX = 40;
    int progressBarY = height() - 100;

    // 进度条背景
    painter->fillRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight, QColor("#e0e0e0"));

    // 进度条边框
    painter->setPen(QPen(QColor("#cccccc"), 1));
    painter->drawRect(progressBarX, progressBarY, progressBarWidth, progressBarHeight);

    // 绘制进度
    if (currentProgressValue > 0)
    {
        QLinearGradient progressGradient(progressBarX, progressBarY, progressBarX, progressBarY + progressBarHeight);
        progressGradient.setColorAt(0, QColor("#00a86b"));
        progressGradient.setColorAt(1, QColor("#008c47"));

        int filledWidth = (progressBarWidth * currentProgressValue) / 100;
        painter->fillRect(progressBarX, progressBarY, filledWidth, progressBarHeight, progressGradient);
    }

    // 绘制进度百分比
    QFont percentFont("Segoe UI", 10);
    painter->setFont(percentFont);
    painter->setPen(QColor("#666666"));
    painter->drawText(QRect(0, progressBarY + progressBarHeight + 10, width(), 20), Qt::AlignCenter,
                      QString("%1%").arg(currentProgressValue));

    // 绘制提示文字
    QFont tipFont("Segoe UI", 9);
    painter->setFont(tipFont);
    painter->setPen(QColor("#999999"));
    painter->drawText(QRect(0, height() - 40, width(), 30), Qt::AlignCenter, "正在加载必要的组件，请稍候...");
}
