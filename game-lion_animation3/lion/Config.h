#ifndef CONFIG_H
#define CONFIG_H
//主角初始位置
#define X 400
#define Y 640
//主角的宽和高
#define W 32
#define H 32
//主角移动速度
#define MOVE_SPEED 8
//主角条约最大高度
#define HEIGHT (2*W+1)
//20ms刷新一次
#define GAME_TICK 20
//重力加速度
#define G 9.8
#define B0 32  //方块边长
#define XSIZE 1280                 //屏幕大小
#define YSIZE 720
#define TITLE "Lion Jump"
#define BG_SPEED 1
#define BACK_GROUND1 ":/back/Picture/gamebkg.jpg"
#define BACK_GROUND2 ":/back/Picture/gamebkg.jpg"
#define BLOCK5 ":/res/stone11.png"

#endif // CONFIG_H
