#include "SplashScreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QApplication>

SplashScreen::SplashScreen(QWidget *parent)
    : QWidget(parent)
    , imageLabel(nullptr)
    , textLabel(nullptr)
    , displayTimer(nullptr)
    , fadeAnimation(nullptr)
{
    setupUI();

    // 设置显示时间为2.5秒后开始淡出
    displayTimer = new QTimer(this);
    displayTimer->setSingleShot(true);
    displayTimer->setInterval(2500);
    connect(displayTimer, &QTimer::timeout, this, &SplashScreen::startFadeOut);

    // 显示后启动计时器
    displayTimer->start();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::setupUI()
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);


    // setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(1280, 720);

    // 设置黑色背景，因为移除了透明背景属性，这个黑色现在会实实在在地填充整个 1280x720 区域
    setStyleSheet("background-color: black;");

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // 设置布局边距和间距，确保内容布局正确
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 添加上方空白区域
    mainLayout->addStretch(2);

    // 创建图片标签
    imageLabel = new QLabel(this);
    QPixmap startImage(":/images/start.png");
    if (!startImage.isNull()) {
        // 缩放图片到合适大小
        QPixmap scaledImage = startImage.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledImage);
    } else {
        // 如果图片不存在，显示文字
        imageLabel->setText("GAME START");
        imageLabel->setStyleSheet("color: white; font-size: 48px; font-weight: bold; font-family: 'Script MT';");
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(imageLabel);

    // 添加中间空白区域
    mainLayout->addStretch(1);

    // 创建提示文字标签
    textLabel = new QLabel("佩戴耳机以获得更好体验", this);
    textLabel->setStyleSheet(R"(
        color: white;
        font-size: 24px;
        font-weight: normal;
        font-family: 'Microsoft YaHei';
        background-color: transparent;
    )");
    textLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(textLabel);

    // 添加下方空白区域
    mainLayout->addStretch(2);

}

void SplashScreen::startFadeOut()
{
    fadeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    fadeAnimation->setDuration(1000); // 1秒淡出
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);

    connect(fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
        emit finished();
        this->close();
    });

    fadeAnimation->start();
}
