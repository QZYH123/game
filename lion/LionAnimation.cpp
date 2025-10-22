#include "LionAnimation.h"
#include <QPainter>
#include <QDebug>
#include "Config.h"

LionAnimation::LionAnimation(QWidget *parent) : QWidget(parent)
    , currentFrame(0)
    , currentType(None)
{
    // 初始化定时器
    frameTimer = new QTimer(this);
    frameTimer->setInterval(GAME_TICK);
    connect(frameTimer, &QTimer::timeout, this, &LionAnimation::onFrameTimerTimeout);

    // 加载动画帧
    loadAnimationFrames();
}

// 加载帧
void LionAnimation::loadAnimationFrames() {
    // 清空原有帧
    left_frames.clear();
    right_frames.clear();
    up_frames.clear();
    down_frames.clear();

    // 加载向左帧
    for (int i = 1; i <= 4; ++i) {
        QPixmap img(QString(":/lion/Picture/left_%1.png").arg(i));
        if (img.isNull()) {
            qDebug() << "向左帧" << i << "加载失败！路径：" << QString(":/lion/Picture/left_%1.png").arg(i);
        } else {
            // 缩放帧到合适大小（根据控件尺寸调整）
            left_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // 加载向右帧
    for (int i = 1; i <= 4; ++i) {
        QPixmap img(QString(":/lion/Picture/right_%1.png").arg(i));
        if (img.isNull()) {
            qDebug() << "向右帧" << i << "加载失败！路径：" << QString(":/lion/Picture/right_%1.png").arg(i);
        } else {
            right_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // 加载跳跃帧
    for (int i = 1; i <= 2; ++i) {
        QPixmap img(QString(":/lion/Picture/jump_%1.png").arg(i));
        if (img.isNull()) {
            qDebug() << "跳跃帧" << i << "加载失败！路径：" << QString(":/lion/Picture/jump_%1.png").arg(i);
        } else {
            up_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    //加载向下帧
    for (int i = 1; i <= 2; ++i) {
        QPixmap img(QString(":/lion/Picture/down_%1.png").arg(i));
        if (img.isNull()) {
            qDebug() << "向下帧" << i << "加载失败！路径：" << QString(":/lion/Picture/down_%1.png").arg(i);
        } else {
            down_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

// 启动向左循环动画
void LionAnimation::startLeftLoop() {
    if (left_frames.isEmpty()) {
        qDebug() << "没有向左帧，无法播放动画";
        return;
    }
    currentType = Left;
    currentFrame = 0;  // 从第0帧开始
    currentHold = holdLeft;      // 设置该动画的帧停留次数
    frameHoldCounter = 0;        // 重置计数器
    frameTimer->start();  // 启动定时器（开始循环）
    update();  // 触发重绘
}

// 启动向右循环动画
void LionAnimation::startRightLoop() {
    if (right_frames.isEmpty()) {
        qDebug() << "没有向右帧，无法播放动画";
        return;
    }
    currentType = Right;
    currentFrame = 0;
    currentHold = holdRight;     // 设置该动画的帧停留次数
    frameHoldCounter = 0;        // 重置计数器
    frameTimer->start();
    update();
}

// 启动跳跃循环动画
void LionAnimation::startUpLoop() {
    if (up_frames.isEmpty()) {
        qDebug() << "没有跳跃帧，无法播放动画";
        return;
    }
    currentType = Up;
    currentHold = holdUp;        // 设置该动画的帧停留次数
    frameHoldCounter = 0;        // 重置计数器
    if(isRight){
        currentFrame = 1;
    }
    else{
        currentFrame = 0;
    }
    //frameTimer->start();
    update();
}

void LionAnimation::startDownLoop() {
    if (down_frames.isEmpty()) {
        qDebug() << "没有向下帧，无法播放动画";
        return;
    }
    currentType = Down;
    currentHold = holdDown;      // 设置该动画的帧停留次数
    frameHoldCounter = 0;        // 重置计数器
    if(isRight){
        currentFrame = 1;
    }
    else{
        currentFrame = 0;
    }
    //frameTimer->start();
    update();
}

void LionAnimation::stop_start() {
    if (down_frames.isEmpty()) {
        qDebug() << "没有向下帧，无法播放动画";
        return;
    }
    if(isRight){
        currentType = Right;
        currentHold = holdRight;
    }
    else{
        currentType = Left;
        currentHold = holdLeft;
    }
    frameHoldCounter = 0;        // 重置计数器
    currentFrame = 0;
    update();
}
// 定时器触发：切换到下一帧
void LionAnimation::onFrameTimerTimeout() {
    // 根据当前动画的停留次数控制是否推进帧
    if (currentType == None) {
        return;
    }
    frameHoldCounter++;
    int hold = currentHold > 0 ? currentHold : 1;
    if (frameHoldCounter % hold != 0) {
        // 不推进帧，但可以保持当前帧显示
        return;
    }

    switch (currentType) {
    case Left:
        currentFrame = (currentFrame + 1) % left_frames.size();  // 循环切换
        break;
    case Right:
        currentFrame = (currentFrame + 1) % right_frames.size();
        break;
    case Up:
        currentFrame = (currentFrame + 1) % up_frames.size();
        break;
    case Down:
        currentFrame = (currentFrame + 1) % down_frames.size();
        break;
    default:
        return;
    }
    update();  // 触发重绘，显示新帧
}

// 重绘事件：显示当前帧
void LionAnimation::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // 绘制当前帧（居中显示）
    QPixmap currentImg;
    switch (currentType) {
    case Left:
        if (currentFrame < left_frames.size()) {
            currentImg = left_frames[currentFrame];
        }
        break;
    case Right:
        if (currentFrame < right_frames.size()) {
            currentImg = right_frames[currentFrame];
        }
        break;
    case Up:
        if (currentFrame < up_frames.size()) {
            currentImg = up_frames[currentFrame];
        }
        break;
    case Down:
        if(currentFrame < down_frames.size()){
            currentImg = down_frames[currentFrame];
        }
        break;
    default:
        // 未播放动画时，显示提示文字
        painter.drawText(rect(), Qt::AlignCenter, "点击按钮播放动画");
        return;
    }

    // 绘制图片（居中）
    if (!currentImg.isNull()) {
        int x = (width() - currentImg.width()) / 2;
        int y = (height() - currentImg.height()) / 2;
        painter.drawPixmap(x, y, currentImg);
    } else {
        painter.drawText(rect(), Qt::AlignCenter, "缺少动画帧资源");
    }
}
