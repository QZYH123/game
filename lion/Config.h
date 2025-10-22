#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QApplication>
#include <QDir>
#include <QFileInfo>

/**
 * @brief 获取游戏数据目录路径
 * @return 数据目录的绝对路径
 * 
 * 此函数会自动检测当前运行环境：
 * - 如果在build目录中运行，会返回源码目录的data路径
 * - 如果在源码目录中运行，会返回当前目录的data路径
 */
inline QString getDataDirectory() {
    QString appDir = QApplication::applicationDirPath();
    QDir currentDir(appDir);
    
    // 检查是否在build目录中（包含Desktop_Qt_*_MinGW_*等模式）
    QString dirName = currentDir.dirName();
    if (dirName.contains("Desktop_Qt_") && dirName.contains("MinGW")) {
        // 在build目录中，需要回到源码目录
        // 从 build/Desktop_Qt_6_2_4_MinGW_64_bit-Debug 回到源码根目录
        currentDir.cdUp(); // 回到build目录
        currentDir.cdUp(); // 回到源码根目录
    }
    
    QString dataPath = currentDir.absolutePath() + "/data";
    return dataPath;
}

/**
 * @brief 获取设置文件目录路径
 * @return 设置文件目录的绝对路径
 */
inline QString getSettingsDirectory() {
    return getDataDirectory(); // 设置文件也放在data目录中
}

//主角初始位置
#define X 400
#define Y 640
//主角的宽和高
#define W 32
#define H 32
//主角移动速度
#define MOVE_SPEED 3
//主角条约最大高度
#define HEIGHT (2*W+1)
//16ms刷新一次 (约60FPS)
#define GAME_TICK 16
//重力加速度 (像素/秒²，适应真实物理计算)
#define G 800.0
#define B0 32  //方块边长
#define XSIZE 1280                 //屏幕大小
#define YSIZE 720
//关卡编辑器网格大小
#define GRID_WIDTH (XSIZE / B0)    //水平格子数：40
#define GRID_HEIGHT (YSIZE / B0)   //垂直格子数：22
#define TITLE "Lion Jump"
#define BG_SPEED 1
#define BACK_GROUND1 ":/back/Picture/gamebkg.jpg"
#define BACK_GROUND2 ":/back/Picture/gamebkg.jpg"
#define BLOCK5 ":/res/stone11.png"

#endif // CONFIG_H
