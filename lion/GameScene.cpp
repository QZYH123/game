/**
 * @file GameScene.cpp
 * @brief 游戏场景类实现（含动画控制测试）
 * @author sqbsayori
 * @date 2025-10-19
 */

#include "GameScene.h"
#include "LionAnimation.h"
#include <QPushButton>
#include "qpainter.h"
#include "QKeyEvent"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QDebug>
#include "Config.h"
extern int map[24][24];
int map[24][24];
int map1[24][24];
GameScene::GameScene(QWidget* parent): QWidget(parent){

    // 设置场景基本属性
    setWindowTitle(TITLE);
    setFixedSize(XSIZE, YSIZE);  // 匹配项目分辨率
    memset(map,0,sizeof(map));
    background=BackGround(1);
    init();
    gameStart();
    updatenum=0;
}
GameScene::~GameScene() {
    // 若有动态分配的资源，在此释放
}
void GameScene::init()
{
    setFixedSize(XSIZE, YSIZE);
    setWindowTitle(TITLE);
    Timer.setInterval(GAME_TICK);
    mapInit();
}
void GameScene::mapInit(){
    for(int i=0;i<24;i++)
        for(int j=23;j>20;j--)
            map[i][j]=1;
    for(int i=0;i<17;i++)
        map[i][19]=1;
    for(int i=23;i>17;i--)
        map[i][18]=1;
    for(int i=7;i<22;i++)
        map[i][16]=1;
    for(int i=0;i<14;i++)
        map[i][13]=1;
    for(int i=4;i<9;i++)
        map[i][5]=1;
    for(int i=0;i<3;i++)
        map[i][2]=1;
    map[4][18]=map[20][15]=map[13][14]=map[16][14]=
        map[17][14]=map[18][14]=map[21][14]=map[22][13]=
        map[22][12]=map[20][12]=map[19][12]=map[18][12]=
        map[2][11]=map[3][11]=map[22][11]=map[13][10]=
        map[16][10]=map[17][10]=map[21][10]=map[0][9]=
        map[1][9]=map[6][9]=map[7][9]=map[5][9]=
        map[12][9]=map[19][9]=map[20][9]=map[21][9]=
        map[11][8]=map[15][8]=map[16][8]=map[2][7]=
        map[3][7]=map[10][7]=map[9][6]=map[17][6]=
        map[18][6]=map[19][6]=map[23][6]=map[15][5]=
        map[14][5]=map[20][5]=map[22][5]=map[12][4]=
        map[13][4]=map[22][4]=map[4][3]=map[5][3]=
        map[11][3]=map[10][2]=map[9][1]=map[9][0]=1;
    for(int i=0;i<24;i++)
        for(int j=0;j<24;j++)
            map1[i][j]=map[i][j];
}
void GameScene::gameStart()
{
    Timer.start();
    pl.setMoveState(leftpress,rightpress);
    connect(&Timer,&QTimer::timeout,[=](){
            if(!pl.is_ground())pl.isJump=1;//运动部分
            if(pl.isJump)pl.fall();
            if (!pl.isJump) { // 落地状态
                if (leftpress || rightpress) {
                    // 落地且有移动输入：确保动画定时器在运行
                    if (!pl.animation->frameTimer->isActive()) {
                        if (pl.isRight) {
                            pl.animation->startRightLoop();
                        } else {
                            pl.animation->startLeftLoop();
                        }
                    }
                } else {
                    // 落地且无移动输入：停止动画（静止状态）
                    pl.animation->frameTimer->stop();
                }
            } else {
                // 空中状态：动画由跳跃逻辑控制（已实现）
            }
            if(leftpress){
                background.mappositionl();
                pl.left();
                if(begin==false)
                    begin=true;
            }
            if(rightpress){
                background.mappositionr();
                pl.right();
                if(begin==false)
                    begin=true;
            }
            update();

    });
}
void GameScene::keyPressEvent(QKeyEvent *event) //按键事件
{
    if (event->key() == Qt::Key_A && event->type())
    {
        leftpress = 1;
        pl.setMoveState(leftpress, rightpress);
    }
    if (event->key() == Qt::Key_D)
    {
        rightpress = 1;
        pl.setMoveState(leftpress, rightpress);
    }
    if (event->key() == Qt::Key_K && !pl.isJump)
    {
        pl.jump();
    }
    update();
}
void GameScene::keyReleaseEvent(QKeyEvent *event)//松开按键事件
{
    if (event->key() == Qt::Key_A && event->type())
    {
        leftpress = 0;
    }
    if (event->key() == Qt::Key_D)
    {
        rightpress = 0;
    }
    update();
}
void GameScene::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(background.map1_x, 0,XSIZE+5,YSIZE, background.map1); //绘制背景图
    painter.drawPixmap(background.map2_x, 0,XSIZE+5,YSIZE, background.map2);
    painter.drawPixmap(background.map3_x, 0,XSIZE+5,YSIZE, background.map3);
    QPixmap currentFrame = pl.getCurrentAnimationFrame();
    if (!currentFrame.isNull()) {
        painter.drawPixmap(pl.x, pl.y, pl.w, pl.h,
                           currentFrame.scaled(pl.w, pl.h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    for(int i=0;i<24;i++)
        for(int j=0;j<24;j++)
        {
            switch(map1[i][j]){
            case 1:
                painter.drawPixmap(i*B0, j*B0,W,W, block5);
                break;
            }
        }
}

BackGround::BackGround()
{
    map1_x = XSIZE;
    map2_x = 0;
    map3_x = -XSIZE;
    m_scroll_speed = BG_SPEED;
}
BackGround::BackGround(int num)
{
    if(num==1)
    {
        map1.load(BACK_GROUND1); //设置背景
        map2.load(BACK_GROUND1);
        map3.load(BACK_GROUND1);
    }
    if(num==2)
    {
        map1.load(BACK_GROUND2); //设置背景
        map2.load(BACK_GROUND2);
        map3.load(BACK_GROUND2);
    }
    map1_x = XSIZE;
    map2_x = 0;
    map3_x = -XSIZE;
    m_scroll_speed = BG_SPEED;
}
void BackGround::mappositionr()
{
    map1_x -= BG_SPEED, map2_x -= BG_SPEED, map3_x -= BG_SPEED;
    if (map1_x <= 0)
        map1_x = XSIZE, map2_x = 0, map3_x = -XSIZE;
}
void BackGround::mappositionl()
{
    map1_x += BG_SPEED, map2_x += BG_SPEED, map3_x += BG_SPEED;
    if (map2_x >= XSIZE)
        map1_x = XSIZE, map2_x = 0, map3_x = -XSIZE;
}
