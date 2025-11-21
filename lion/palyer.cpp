#include "player.h"
#include "Config.h"
#include"LevelData.h"
#include <QDateTime>

extern int map[GRID_WIDTH][GRID_HEIGHT];
player::player(QWidget *parent) : animation(new LionAnimation(parent))
{
    x = X, y = Y, h = H, w = W;//初始化角色位置和大小
    vx = 0, vy = 0; // 初始化速度
    isJump = 0;
    v0 = 0,t = 0;
    isRight = true;
    onGround = false;
    onMovingPlatform = false;
    platformRelativeX = 0.0;
    platformRelativeY = 0.0;
    currentPlatformIndex = -1;
    // 初始化按键与动画状态
    isLeftPress = false;
    isRightPress = false;
    lastAnimType = LionAnimation::None;
    animation->loadAnimationFrames();
    moveSpeed = MOVE_SPEED;

    airDashUsed = false; // +++ 新增：初始化空中冲刺标记

    // +++ 新增：初始化冲刺变量
    isDashing = false;
    dashStartTime = 0;
    dashSpeed = (int)(MOVE_SPEED * 2.5); // 冲刺速度设为2.5倍

    // 初始显示面向右的静态首帧
    animation->startIdleRight();
}

void player::startDash()
{
    // 如果正在冲刺，则不允许再次触发
    if (isDashing) return;

    // +++ 新增：检查空中冲刺限制
    // isJump 意味着不在地上
    if (isJump && airDashUsed) {
        // 已经在空中冲刺过了，直到落地前不能再冲
        return;
    }

    isDashing = true;
    dashStartTime = QDateTime::currentMSecsSinceEpoch();

    // +++ 新增：如果这次是在空中发起的，标记
    if (isJump) {
        airDashUsed = true;
    }

    // 在此添加冲刺音效
    // AudioController::getInstance().playSound(SoundType::Dash);
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
    // 检测角色右侧在头部与脚部两个采样点是否与砖块接触
    int rightCol = (x + w) / B0;           // 角色右侧相邻砖块列
    int topRow   = (y + 5) / B0;           // 头部偏下采样，避免边界抖动
    int bottomRow= (y + h - 5) / B0;       // 脚部偏上采样，避免误判台阶边缘
    if (map[rightCol][topRow]==1|| map[rightCol][bottomRow] == 1)
        return 1;
    else
        return 0;
}
bool player::left_touch(){
    // 检测角色左侧在头部与脚部两个采样点是否与砖块接触
    int leftCol  = (x - 5) / B0;           // 角色左侧相邻砖块列（留5像素余量）
    int topRow   = (y + 5) / B0;           // 头部偏下采样
    int bottomRow= (y + h - 5) / B0;       // 脚部偏上采样
    if (map[leftCol][topRow] == 1 || map[leftCol][bottomRow] == 1)
        return 1;
    else
        return 0;
}
bool player::head_touch(){
    // 检测角色头顶左右两个角是否触碰砖块
    int topRow = y / B0;
    int leftColCorner  = (x + 5) / B0;
    int rightColCorner = (x + w - 5) / B0;
    if (map[leftColCorner][topRow] == 1 || map[rightColCorner][topRow] == 1)
        return 1;
    else
        return 0;
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
    AudioController::getInstance().playSound(SoundType::Jump);
    // 将目标跳跃高度按上一版的视觉效果回归为约两倍 HEIGHT
    double targetHeight = HEIGHT * 2.0;
    v0 = -sqrt(2 * G * targetHeight);
    isJump = 1;
    animation->startJumpLoop();
    fall();
}

void player::fall()
{
    // 使用基于实际帧率的时间步长
    t = GAME_TICK / 1000.0; // 将毫秒转换为秒
    h1 = v0 * t + G * t * t / 2; // 本帧位移（像素）

    // 先按位移更新一次，再根据碰撞进行对齐修正
    int dy = (int)(h1 + 0.5);
    y += dy;

    if (v0 > 0) {
        // 下落：检测脚下碰撞并吸附到砖块顶面
        if (is_ground()) {
            int tileRow = (y + H) / B0; // 脚所在的砖块行
            y = tileRow * B0 - H;       // 顶对齐：砖块顶面减去角色高度
            v0 = 0;
            isJump = 0;
            onGround = true;  // 关键修复：设置onGround状态
            airDashUsed = false; // +++ 新增：在静态地面落地时，重置空中冲刺
        }
    } else {
        // 上升：检测头顶碰撞并压回到砖块下侧
        if (head_touch()) {
            int tileRow = y / B0;       // 头部所在的砖块行
            y = (tileRow + 1) * B0;     // 底对齐：砖块底面
            v0 = 0;
            h1 = 0;
        }
    }

    // 更新速度（重力）
    v0 = v0 + G * t;
}
void player::update()
{
    if (isDashing) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - dashStartTime > DASH_DURATION) {
            // 冲刺结束
            isDashing = false;
        } else {
            // 正在冲刺：应用冲刺移动
            if (isRight && !right_touch()) {
                x = (x + dashSpeed < XSIZE - w) ? x + dashSpeed : XSIZE - w;
            } else if (!isRight && !left_touch()) {
                x = (x - dashSpeed > 0) ? x - dashSpeed : 0;
            }
        }
    }
    // 1. 处理跳跃/下落逻辑
    // 如果不在移动平台上，检查静态地面；如果在移动平台上，onGround由GameScene设置
    if (!onMovingPlatform && !is_ground()) {
        isJump = true;
    }
    if (isJump) {
        fall(); // 下落计算
    }

    // 2. 处理普通移动逻辑 (仅在不冲刺时生效)
    if (!isDashing) {
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
        // 跳跃期间允许空中转向：仅更新朝向，不改变动画类型
        animation->setFacingRight(isRight);
    } else if (isLeftPress) {
        newType = LionAnimation::Left;
    } else if (isRightPress) {
        newType = LionAnimation::Right;
    }
    // 特殊处理：如果玩家在移动平台上但没有主动移动，强制显示静止动画
    else if (onMovingPlatform && onGround) {
        // 在移动平台上且没有按键输入时，显示静止动画
        newType = LionAnimation::None;
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
            // 静止：根据面向方向显示首帧
            if (isRight) animation->startIdleRight();
            else animation->startIdleLeft();
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

void player::resetAirDash()
{
    airDashUsed = false;
}
