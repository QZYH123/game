/**
 * @file GameScene.cpp
 * @brief 游戏场景类实现（含动画控制测试）
 * @author 开发团队
 * @date 2025-10-19
 */

#include "GameScene.h"
#include "LionAnimation.h"  // 确保包含狮子动画类头文件
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QDebug>

GameScene::GameScene(int levelIndex, QWidget* parent)
    : QWidget(parent)
    , currentLevel(levelIndex)
    , lionAnimation(nullptr)
    , backButton(nullptr)
    , btnLeft(nullptr)
    , btnRight(nullptr)
    , btnJump(nullptr)
{
    // 设置场景基本属性
    setWindowTitle(QString("醒狮跃境 - 关卡 %1（动画测试）").arg(levelIndex + 1));
    setFixedSize(1280, 720);  // 匹配项目分辨率

    // 初始化场景元素（含动画控制按钮）
    initScene();
}

void GameScene::initScene()
{
    // 创建主布局（垂直布局：动画区 + 控制区 + 返回区）
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);  // 整体边距

    // 1. 关卡信息标签
    QLabel* levelInfo = new QLabel(
        QString("关卡 %1 - 动画测试\n点击按钮切换狮子动作（循环播放）")
            .arg(currentLevel + 1),
        this
        );
    levelInfo->setStyleSheet("color: white; font-size: 18px; "
                             "background-color: rgba(0,0,0,0.5); "
                             "padding: 10px; border-radius: 5px;");
    mainLayout->addWidget(levelInfo, 0, Qt::AlignLeft);
    mainLayout->addSpacing(10);  // 间距

    // 2. 狮子动画显示区（核心）
    lionAnimation = new LionAnimation(this);
    lionAnimation->setFixedSize(600, 600);  // 放大动画显示区域，方便观察
    lionAnimation->setStyleSheet("background-color: rgba(255,255,255,0.1);");  // 半透明白背景，突出动画
    mainLayout->addWidget(lionAnimation, 1, Qt::AlignCenter);  // 居中显示
    mainLayout->addSpacing(20);

    // 3. 动画控制按钮区（水平布局：向左 + 向右 + 跳跃）
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(30);  // 按钮间距

    // 向左动画按钮
    btnLeft = new QPushButton("循环播放：向左移动", this);
    styleButton(btnLeft);  // 统一样式
    controlLayout->addWidget(btnLeft);

    // 向右动画按钮
    btnRight = new QPushButton("循环播放：向右移动", this);
    styleButton(btnRight);
    controlLayout->addWidget(btnRight);

    // 跳跃动画按钮
    btnJump = new QPushButton("循环播放：跳跃", this);
    styleButton(btnJump);
    controlLayout->addWidget(btnJump);

    mainLayout->addLayout(controlLayout, 0);  // 添加到主布局
    mainLayout->addSpacing(20);

    // 4. 返回按钮
    backButton = new QPushButton("返回选关", this);
    backButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(0,0,0,0.7);
            color: white;
            font-size: 18px;
            padding: 12px 30px;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: rgba(255,255,0,0.8);
            color: black;
        }
    )");
    mainLayout->addWidget(backButton, 0, Qt::AlignRight);

    // 连接信号槽
    connect(backButton, &QPushButton::clicked, this, &GameScene::onBackButtonClicked);
    connect(btnLeft, &QPushButton::clicked, lionAnimation, &LionAnimation::startLeftLoop);
    connect(btnRight, &QPushButton::clicked, lionAnimation, &LionAnimation::startRightLoop);
    connect(btnJump, &QPushButton::clicked, lionAnimation, &LionAnimation::startJumpLoop);

    // 设置背景
    QPalette palette;
    QPixmap bg(":/back/Picture/gamebkg.jpg");
    if (bg.isNull()) {
        qDebug() << "警告：背景图加载失败，使用默认颜色背景";
        palette.setBrush(QPalette::Window, Qt::darkCyan);  // 备用颜色
    } else {
        palette.setBrush(QPalette::Window, bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    setPalette(palette);
}

// 按钮样式统一设置
void GameScene::styleButton(QPushButton* btn)
{
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(0,100,200,0.7);
            color: white;
            font-size: 16px;
            padding: 12px 20px;
            border-radius: 8px;
            min-width: 180px;
        }
        QPushButton:hover {
            background-color: rgba(0,150,255,0.9);
            transform: scale(1.05);  //  hover时轻微放大
        }
        QPushButton:pressed {
            background-color: rgba(0,80,180,0.9);
        }
    )");
}

void GameScene::onBackButtonClicked()
{
    emit backToLevelSelect();  // 触发返回选关界面信号
}
