#ifndef PLAYER_H
#define PLAYER_H
#include "qpixmap.h"
#include <QSoundEffect>
#include "LionAnimation.h"
#include "qdebug.h"
#include "AudioController.h"
class player
{
public:
    player(QWidget *parent = nullptr);
    int x;
    int y;//位置
    double vx; // X轴速度
    double vy; // Y轴速度
    int h;
    int w;//大小
    int h1;//移动距离
    double t;//离地时间
    double v0;//向下的速度
    bool isJump;//是否跳跃
    bool isRight;//角色朝向
    bool onGround;//是否在地面上
    bool onMovingPlatform;//是否在移动平台上
    double platformRelativeX; // 玩家相对于平台的X位置
    double platformRelativeY; // 玩家相对于平台的Y位置
    int currentPlatformIndex; // 当前所在平台的索引
    int picNum;//现在是动画第几帧

public:
    void startDash(); // 触发冲刺的函数
    bool getIsDashing() const { return isDashing; } // 供GameScene判断是否在冲刺
    void resetAirDash();
    // (在 player.h 的 private: 部分添加)
private:
    bool airDashUsed;
    bool isDashing;          // 是否正在冲刺
    qint64 dashStartTime;    // 冲刺开始时间 (ms)
    int dashSpeed;           // 冲刺速度
    const qint64 DASH_DURATION = 200; // 冲刺持续时间 (200 ms)
public:
    LionAnimation* animation;
    virtual void left();
    virtual void right();
    void jump();
    void fall();
    void update();
    void setLeftPressed(bool pressed) { isLeftPress = pressed; }
    void setRightPressed(bool pressed) { isRightPress = pressed; }
    bool getLeftPressed() const { return isLeftPress; }
    bool getRightPressed() const { return isRightPress; }
    void updateAnimationState();
    QPixmap getCurrentAnimationFrame();
    virtual bool left_touch();
    virtual bool right_touch();
    bool head_touch();
    bool is_ground();
    // 运行时可调移动速度（用于水域减速）
    void setMoveSpeedScale(double scale);
    // 重置移动速度为默认值
    void resetMoveSpeed();
    // 重置按键状态
    void resetKeyStates();
private:
    bool isLeftPress;  // 记录左键是否按下
    bool isRightPress; // 记录右键是否按下
    LionAnimation::AnimationType lastAnimType;
    int moveSpeed;     // 当前移动速度（默认MOVE_SPEED）
};

#endif // PLAYER_H
