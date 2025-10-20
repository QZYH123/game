#ifndef LIONANIMATION_H
#define LIONANIMATION_H

#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QTimer>

class LionAnimation : public QWidget
{
    Q_OBJECT
public:
    explicit LionAnimation(QWidget *parent = nullptr);

    // 加载动画帧（原函数保留）
    void loadAnimationFrames();

signals:

public slots:
    // 启动各动画循环
    void startLeftLoop();    // 向左移动循环
    void startRightLoop();   // 向右移动循环
    void startJumpLoop();    // 跳跃循环
    // 帧切换定时器槽函数
    void onFrameTimerTimeout();
public:
    QVector<QPixmap> left_frames;   // 向左帧序列
    QVector<QPixmap> right_frames;  // 向右帧序列
    QVector<QPixmap> jump_frames;   // 跳跃帧序列

    QTimer* frameTimer;  // 控制帧切换的定时器
    int currentFrame;    // 当前显示的帧索引
    enum AnimationType {  // 动画类型枚举
        Left,
        Right,
        Jump,
        None
    } currentType;  // 当前播放的动画类型
public:
    // 公共接口：获取当前帧
    QPixmap getCurrentFrame() const {
        switch (currentType) {
        case Left: return left_frames[currentFrame];
        case Right: return right_frames[currentFrame];
        case Jump: return jump_frames[currentFrame];
        default: return QPixmap();  // 默认返回空图
        }
    }

    // 重写绘制事件，显示当前帧
    void paintEvent(QPaintEvent *event) override;
};

#endif // LIONANIMATION_H
