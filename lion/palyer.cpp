#include "player.h"
#include "Config.h"

extern int map[24][24];
player::player(QWidget *parent) : animation(new LionAnimation(parent))
{
    x = X, y = Y, h = H, w = W;//初始化角色位置和大小
    isJump = 0;
    v0 = 0,t = 0;
    isRight = true;
    animation->loadAnimationFrames();
}
bool player::is_ground()
{
    if(map[(x+5)/B0][(y+H)/B0]!=0||map[(x+w-5)/B0][(y+H)/B0]!=0)
    {
        if(map[(x+5)/B0][(y+H)/B0]==1||map[(x+w-5)/B0][(y+H)/B0]==1)
            return 1;
    }
    return 0;
}
bool player::right_touch(){
    if(map[x/B0+1][y/B0]!=1)return 0;
    else return 1;
}
bool player::left_touch(){
    if(map[(x-5)/B0][y/B0]!=1)return 0;
    else return 1;
}
bool player::head_touch(){
    if( map[(x+5)/B0][(y)/B0]==1||map[(x+w-5)/B0][(y)/B0]==1)
        return 1;
    else return 0;
}
void player::right()
{
    if(!right_touch())
        x = (x + MOVE_SPEED < XSIZE - w) ? x + MOVE_SPEED : XSIZE - w;
    isRight = true;
    animation->startRightLoop();
}
void player::left()
{
    if(!left_touch())
        x = (x - MOVE_SPEED > 0) ? x - MOVE_SPEED : 0;
    isRight = false;
    animation->startLeftLoop();
}
void player::jump()
{
    v0 = -sqrt(2 * G * HEIGHT);
    isJump = 1;
    animation->startJumpLoop();
    fall();
}

void player::fall()
{
    t = sqrt(2 * HEIGHT / G) / 14;
    h1=v0*t+G*pow(t,2)/2;
    y+=(int)(h1+0.5);

    if(v0>0){
        if(is_ground()){
            y=y/B0*B0;
            v0=0;
            isJump=0;
        }
    }
    else {
        if(head_touch()){
            v0 = 0;
            h1 = 0;
            y = y / h * h + h;
        }
        y+=(int)(h1+0.5);
    }

    v0=v0+G*t;
}
void player::updateAnimationState()
{
    LionAnimation::AnimationType newType = LionAnimation::None;

    // 优先级：跳跃动画 > 移动动画 > 静止
    if (isJump) {
        newType = LionAnimation::Jump;
    } else if (isLeftPress) {
        newType = LionAnimation::Left;
    } else if (isRightPress) {
        newType = LionAnimation::Right;
    }
    if (newType != lastAnimType) {
        switch (newType) {
        case LionAnimation::Left:
            animation->startLeftLoop();
            break;
        case LionAnimation::Right:
            animation->startRightLoop();
            break;
        case LionAnimation::Jump:
            animation->startJumpLoop();
            break;
        default:
            animation->frameTimer->stop(); // 静止时停止动画
            break;
        }
        lastAnimType = newType;
    }
}
QPixmap player::getCurrentAnimationFrame()
{
    return animation->getCurrentFrame();
}
