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

    // 独立动画帧间隔（毫秒），与 GAME_TICK 解耦
    static constexpr int FRAME_INTERVAL_MS = 100;

    // 加载动画帧（原函数保留）
    void loadAnimationFrames();

signals:

public slots:
    // 启动各动画循环
    void startLeftLoop();    // 向左移动循环
    void startRightLoop();   // 向右移动循环
    void startJumpLoop();    // 跳跃循环
    // 新增：空闲静态帧
    void startIdleLeft();
    void startIdleRight();

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
        IdleLeft,
        IdleRight,
        None
    } currentType;  // 当前播放的动画类型

    // 当前朝向（true=右，false=左），用于跳跃镜像
    bool facingRight = true;
public:
    // 公共接口：获取当前帧
    QPixmap getCurrentFrame() const {
        switch (currentType) {
        case Left:
            return left_frames.isEmpty() ? QPixmap() : (frameTimer && !frameTimer->isActive() ? left_frames.first() : left_frames[currentFrame]);
        case Right:
            return right_frames.isEmpty() ? QPixmap() : (frameTimer && !frameTimer->isActive() ? right_frames.first() : right_frames[currentFrame]);
        case Jump: {
            if (jump_frames.isEmpty()) return QPixmap();
            QPixmap base = (frameTimer && !frameTimer->isActive()) ? jump_frames.first() : jump_frames[currentFrame];
            if (!facingRight && !base.isNull()) {
                return QPixmap::fromImage(base.toImage().mirrored(true, false));
            }
            return base;
        }
        case IdleLeft:
            return left_frames.isEmpty() ? QPixmap() : left_frames.first();
        case IdleRight:
            return right_frames.isEmpty() ? QPixmap() : right_frames.first();
        default:
            return QPixmap();  // 默认返回空图
        }
    }

    // 重写绘制事件，显示当前帧
    void paintEvent(QPaintEvent *event) override;
};

#endif // LIONANIMATION_H
