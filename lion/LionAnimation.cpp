#include "LionAnimation.h"
#include <QPainter>
#include <QDebug>

LionAnimation::LionAnimation(QWidget *parent) : QWidget(parent)
    , currentFrame(0)
    , currentType(None)
{
    // 初始化定时器（每100ms切换一帧，可调整速度）
    frameTimer = new QTimer(this);
    frameTimer->setInterval(100);  // 100ms/帧 = 10帧/秒
    connect(frameTimer, &QTimer::timeout, this, &LionAnimation::onFrameTimerTimeout);

    // 加载动画帧（确保图片已添加到qrc）
    loadAnimationFrames();
}

// 加载帧（原函数，注意文件格式与qrc匹配）
void LionAnimation::loadAnimationFrames() {
    // 清空原有帧
    left_frames.clear();
    right_frames.clear();
    jump_frames.clear();

    // 加载向左帧（注意：如果实际是.png，这里要改后缀）
    for (int i = 1; i <= 4; ++i) {
        QPixmap img(QString(":/lion/Picture/left_%1.jpg").arg(i));
        if (img.isNull()) {
            qDebug() << "向左帧" << i << "加载失败！路径：" << QString(":/lion/left_%1.jpg").arg(i);
        } else {
            // 缩放帧到合适大小（根据控件尺寸调整）
            left_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // 加载向右帧
    for (int i = 1; i <= 4; ++i) {
        QPixmap img(QString(":/lion/Picture/right_%1.jpg").arg(i));
        if (img.isNull()) {
            qDebug() << "向右帧" << i << "加载失败！路径：" << QString(":/lion/right_%1.jpg").arg(i);
        } else {
            right_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // 加载跳跃帧
    for (int i = 1; i <= 4; ++i) {
        QPixmap img(QString(":/lion/Picture/jump_%1.jpg").arg(i));
        if (img.isNull()) {
            qDebug() << "跳跃帧" << i << "加载失败！路径：" << QString(":/lion/jump_%1.jpg").arg(i);
        } else {
            jump_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    frameTimer->start();
    update();
}

// 启动跳跃循环动画
void LionAnimation::startJumpLoop() {
    if (jump_frames.isEmpty()) {
        qDebug() << "没有跳跃帧，无法播放动画";
        return;
    }
    currentType = Jump;
    currentFrame = 0;
    frameTimer->start();
    update();
}

// 定时器触发：切换到下一帧
void LionAnimation::onFrameTimerTimeout() {
    switch (currentType) {
    case Left:
        currentFrame = (currentFrame + 1) % left_frames.size();  // 循环切换
        break;
    case Right:
        currentFrame = (currentFrame + 1) % right_frames.size();
        break;
    case Jump:
        currentFrame = (currentFrame + 1) % jump_frames.size();
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
    case Jump:
        if (currentFrame < jump_frames.size()) {
            currentImg = jump_frames[currentFrame];
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
