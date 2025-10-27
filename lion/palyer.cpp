#include "player.h"
#include "Config.h"
#include "AudioController.h"

extern int map[GRID_WIDTH][GRID_HEIGHT];
player::player(QWidget *parent) : animation(new LionAnimation(parent))
{
    x = X, y = Y, h = H, w = W;//初始化角色位置和大小
    vx = 0, vy = 0; // 初始化速度
    isJump = 0;
    v0 = 0,t = 0;
    isRight = true;
    animation->loadAnimationFrames();
    moveSpeed = MOVE_SPEED;
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
        x = (x + moveSpeed < XSIZE - w) ? x + moveSpeed : XSIZE - w;
    isRight = true;
   // qDebug()<<moveSpeed;
    animation->startRightLoop();
}
void player::left()
{
    if(!left_touch())
        x = (x - moveSpeed > 0) ? x - moveSpeed : 0;
    isRight = false;
    animation->startLeftLoop();
}
void player::jump()
{
    v0 = -sqrt(2 * G * HEIGHT);
    isJump = 1;
    animation->startJumpLoop();
    fall();
    AudioController::getInstance().playSound(SoundType::Jump);
}

void player::fall()
{
    // 使用基于实际帧率的时间步长
    t = GAME_TICK / 1000.0; // 将毫秒转换为秒
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
void player::update()
{
    // 1. 处理跳跃/下落逻辑
    if (!is_ground()) {
        isJump = true;
    }
    if (isJump) {
        fall(); // 下落计算
    }

    // 2. 处理移动逻辑（基于按键状态）
    if (isLeftPress && !left_touch()) {
        x = (x - moveSpeed > 0) ? x - moveSpeed : 0;
        isRight = false;
        qDebug() << "角色左移，当前移动速度：" << moveSpeed;
    }
    if (isRightPress && !right_touch()) {
        x = (x + moveSpeed < XSIZE - w) ? x + moveSpeed : XSIZE - w;
        isRight = true;
        qDebug() << "角色右移，当前移动速度：" << moveSpeed;
    }

    // 3. 处理动画状态更新
    updateAnimationState();
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

void player::setMoveSpeedScale(double scale)
{
    if (scale < 0.2) scale = 0.2;
    if (scale > 2.0) scale = 2.0;
    moveSpeed = qMax(1, (int)(MOVE_SPEED * scale));
}

void player::resetMoveSpeed()
{
    moveSpeed = MOVE_SPEED;
}

void player::resetKeyStates()
{
    isLeftPress = false;
    isRightPress = false;
    qDebug() << "玩家按键状态已重置";
}
