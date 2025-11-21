/**
 * @file main.cpp
 * @brief 应用入口，创建并展示主菜单窗口
 */

#include "menu.h"
#include "SplashScreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建启动画面
    SplashScreen* splash = new SplashScreen();
    
    // 创建主菜单
    menu* w = new menu();
    
    // 连接启动画面结束信号到主菜单显示
    QObject::connect(splash, &SplashScreen::finished, [w, splash]() {
        w->show();
        splash->deleteLater();
    });
    
    // 显示启动画面
    splash->show();
    
    return a.exec();
}
