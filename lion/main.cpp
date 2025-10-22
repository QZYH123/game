/**
 * @file main.cpp
 * @brief 应用入口，创建并展示主菜单窗口
 */

#include "menu.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    menu w;
    w.show();
    return a.exec();
}
