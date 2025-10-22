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
    void startUpLoop();    // 向上循环
    void startDownLoop();    // 向下循环
    void stop_start();
    // 帧切换定时器槽函数
    void onFrameTimerTimeout();
public:
    QVector<QPixmap> left_frames;   // 向左帧序列
    QVector<QPixmap> right_frames;  // 向右帧序列
    QVector<QPixmap> up_frames;   // 跳跃帧序列
    QVector<QPixmap> down_frames;  //向下帧序列
    bool isRight;

    QTimer* frameTimer;  // 控制帧切换的定时器
    int currentFrame;    // 当前显示的帧索引
    enum AnimationType {  // 动画类型枚举
        Left,
        Right,
        Up,
        Down,
        None
    } currentType;  // 当前播放的动画类型

    // 新增：帧停留控制（不影响其他模块）
    int frameHoldCounter = 0;     // 帧停留计数器
    int currentHold = 1;          // 当前动画每帧停留的tick数
    int holdLeft = 3;             // 左右动画更慢（每帧停留3个tick）
    int holdRight = 3;
    int holdUp = 4;               // 跳跃/下落更慢（每帧停留4个tick）
    int holdDown = 4;
public:
    // 公共接口：获取当前帧
    QPixmap getCurrentFrame() const {
        switch (currentType) {
        case Left: return left_frames[currentFrame];
        case Right: return right_frames[currentFrame];
        case Up: return up_frames[currentFrame];
        case Down: return down_frames[currentFrame];
        default: return QPixmap();  // 默认返回空图
        }
    }

    // 重写绘制事件，显示当前帧
    void paintEvent(QPaintEvent *event) override;
};

#endif // LIONANIMATION_H
