/**
 * @file GameScene.h
 * @brief 游戏场景类，用于测试狮子动画渲染及关卡逻辑
 * @author 开发团队
 * @date 2024-12-15
 */

#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include "LionAnimation.h"  // 引入之前实现的狮子动画类
#include <QVBoxLayout>
#include <QPushButton>

/**
 * @class GameScene
 * @brief 游戏场景类，管理关卡内实体渲染与交互
 */
class GameScene : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param levelIndex 关卡索引（用于区分不同关卡配置）
     * @param parent 父窗口指针
     */
    explicit GameScene(int levelIndex, QWidget* parent = nullptr);

    /**
     * @brief 初始化场景（加载关卡资源、创建实体）
     */
    void initScene();

signals:
    /**
     * @brief 返回选关界面信号
     */
    void backToLevelSelect();

private slots:
    /**
     * @brief 处理返回按钮点击事件
     */
    void onBackButtonClicked();

private:
    /**
     * @brief 统一设置按钮样式
     * @param btn 需要设置样式的按钮指针
     */
    void styleButton(QPushButton* btn);

private:
    int currentLevel;               ///< 当前关卡索引
    LionAnimation* lionAnimation;   ///< 狮子动画实例
    QPushButton* backButton;        ///< 返回按钮
    QPushButton* btnLeft;           ///< 向左移动动画按钮
    QPushButton* btnRight;          ///< 向右移动动画按钮
    QPushButton* btnJump;           ///< 跳跃动画按钮
};

#endif // GAMESCENE_H
