/**
 * @file menu.h
 * @brief 主菜单窗口类声明
 * @author 开发团队
 * @date 2025-10-16
 * @version 0.1.0
 */

#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QPushButton>
#include "GameScene.h"

// 前向声明
class LevelSelect;
class SettingsPage;
class LevelEditor;

namespace Ui {
class menu;
}

/**
 * @class menu
 * @brief 主菜单窗口，负责菜单界面加载与展示
 *
 * 说明：此类仅进行 UI 组装与布局，不涉及业务逻辑。
 */
class menu : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造主菜单窗口
     * @param parent 父窗口指针
     */
    explicit menu(QWidget *parent = nullptr);

    /**
     * @brief 析构函数，释放UI资源
     */
    ~menu();

    /**
     * @brief 加载并布局主菜单界面
     * @note 只进行界面初始化与样式设置
     */
    void ui_load();

    QPushButton** buttons;        ///< 菜单按钮数组，顺序为 Start/Continue/LevelEditor/Settings/Exit

private slots:
    /**
     * @brief 处理开始游戏按钮点击事件
     */
    void onStartButtonClicked();
    
    /**
     * @brief 处理继续游戏按钮点击事件
     */
    void onContinueButtonClicked();
    
    /**
     * @brief 处理从选关界面返回的事件
     */
    void onBackFromLevelSelect();
    
    /**
     * @brief 处理Settings按钮点击事件
     */
    void onSettingsButtonClicked();
    
    /**
     * @brief 处理从设置页面返回的事件
     */
    void onBackFromSettings();

    /**
     * @brief 处理关卡选择事件
     */
    void onLevelSelected(int levelIndex);
    
    /**
     * @brief 处理关卡编辑器按钮点击事件
     */
    void onLevelEditorButtonClicked();
    
    /**
     * @brief 处理从关卡编辑器返回的事件
     */
    void onBackFromLevelEditor();
    
    /**
     * @brief 处理自定义关卡按钮点击事件
     */
    void onCustomLevelButtonClicked();

private:
    Ui::menu *ui;                 ///< Qt Designer 生成的 UI 指针
    LevelSelect* level_select;    ///< 选关界面指针
    SettingsPage* settings_page;  ///< 设置页面指针
    LevelEditor* level_editor;    ///< 关卡编辑器指针
    GameScene* gameScene;
};

#endif // MENU_H
