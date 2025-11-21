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
    // 优先定位源码根目录的 data：通过标识文件（CMakeLists.txt、Pic.qrc）识别工程根
    QString appDir = QApplication::applicationDirPath();
    QDir dir(appDir);

    // 先尝试识别工程根目录（包含 CMakeLists.txt 与 Pic.qrc）
    for (int i = 0; i < 8; ++i) {
        QString root = dir.absolutePath();
        bool hasCMake = QFileInfo(QDir(root).absoluteFilePath("CMakeLists.txt")).isFile();
        bool hasQrc   = QFileInfo(QDir(root).absoluteFilePath("Pic.qrc")).isFile();
        if (hasCMake && hasQrc) {
            QString data = QDir(root).absoluteFilePath("data");
            if (QFileInfo(data).isDir()) {
                return data; // 返回 lion/data
            }
        }
        if (!dir.cdUp()) break;
    }

    // 次优先：向上查找存在的 data，但跳过位于 build/ 或 Desktop_Qt_* 目录下的 data
    dir = QDir(appDir);
    for (int i = 0; i < 8; ++i) {
        QString parentPath = dir.absolutePath();
        QString data = dir.absoluteFilePath("data");
        if (QFileInfo(data).isDir()) {
            // 跳过构建目录下的 data
            bool isBuildDir = parentPath.contains("build", Qt::CaseInsensitive) ||
                              parentPath.contains("Desktop_Qt_", Qt::CaseInsensitive);
            if (!isBuildDir) {
                return data;
            }
        }
        if (!dir.cdUp()) break;
    }

    // 兜底：返回最近的 data（可能在构建目录中）或当前运行目录下的 data
    dir = QDir(appDir);
    for (int i = 0; i < 6; ++i) {
        QString data = dir.absoluteFilePath("data");
        if (QFileInfo(data).isDir()) {
            return data;
        }
        if (!dir.cdUp()) break;
    }
    return QDir(appDir).absoluteFilePath("data");
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
#define BACK_GROUND1 ":/images/background.png"
#define BACK_GROUND2 ":/images/background.png"
#define BLOCK5 ":/images/platform.png"

#endif // CONFIG_H
