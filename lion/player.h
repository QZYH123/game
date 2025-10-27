#ifndef PLAYER_H
#define PLAYER_H
#include "qpixmap.h"
#include <QSoundEffect>
#include "LionAnimation.h"
#include "qdebug.h"
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
    int picNum;//现在是动画第几帧
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
