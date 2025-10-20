#ifndef PLAYER_H
#define PLAYER_H
#include "qpixmap.h"
#include <QSoundEffect>
#include "LionAnimation.h"
class player
{
public:
    player(QWidget *parent = nullptr);
    int x;
    int y;//位置
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
    void setMoveState(bool left, bool right) {
        isLeftPress = left;
        isRightPress = right;
    }
    void updateAnimationState();
    QPixmap getCurrentAnimationFrame();
    virtual bool left_touch();
    virtual bool right_touch();
    bool head_touch();
    bool is_ground();
private:
    bool isLeftPress;  // 记录左键是否按下
    bool isRightPress; // 记录右键是否按下
    LionAnimation::AnimationType lastAnimType;
};

#endif // PLAYER_H
