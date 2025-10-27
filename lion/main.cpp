/**
 * @file main.cpp
 * @brief 应用入口，创建并展示主菜单窗口
 */

#include "menu.h"
#include <QApplication>
#include "SettingsPage.h"
#include "AudioController.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 先初始化GameSettings（确保生命周期最长）
    GameSettings::getInstance();
    // 再初始化AudioController
    AudioController::getInstance();

    menu w;
    w.show();
    return a.exec();
}
