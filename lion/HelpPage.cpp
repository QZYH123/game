#include "HelpPage.h"
#include "menu.h"  // 包含HoverSoundButton的定义
#include "AudioController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPixmap>
#include <QFont>

HelpPage::HelpPage(QWidget *parent)
    : QWidget(parent)
    , backButton(nullptr)
    , scrollArea(nullptr)
    , contentWidget(nullptr)
{
    setupUI();
}

HelpPage::~HelpPage()
{
}

void HelpPage::setupUI()
{
    // 设置窗口大小和背景
    resize(1280, 720);
    
    // 设置背景图片
    QPixmap background(":/images/menu.jpg");
    QPixmap scaledBackground = background.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Window, scaledBackground);
    setPalette(palette);
    
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 30, 50, 30);
    mainLayout->setSpacing(20);
    
    // 创建标题
    QLabel* titleLabel = new QLabel("游戏帮助", this);
    titleLabel->setStyleSheet(R"(
        color: white;
        font-size: 36px;
        font-weight: bold;
        font-family: 'Microsoft YaHei';
        background-color: rgba(0, 0, 0, 100);
        border: 2px solid #ffaa00;
        border-radius: 10px;
        padding: 15px;
    )");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 创建滚动区域
    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: rgba(30, 30, 30, 200);
            border: 2px solid #ffaa00;
            border-radius: 10px;
        }
        QScrollBar:vertical {
            background-color: rgba(50, 50, 50, 150);
            width: 15px;
            border-radius: 7px;
        }
        QScrollBar::handle:vertical {
            background-color: #ffaa00;
            border-radius: 7px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #ffcc00;
        }
    )");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    createContent();
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // 创建返回按钮
    backButton = new HoverSoundButton("返回主菜单", this);
    backButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            font-size: 20px;
            font-weight: bold;
            font-family: 'Microsoft YaHei';
            border: none;
            border-radius: 10px;
            padding: 15px 30px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
    )");
    
    connect(backButton, &QPushButton::clicked, [this]() {
        AudioController::getInstance().playSound(SoundType::Click);
        emit backToMenu();
    });
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void HelpPage::createContent()
{
    contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(25);
    
    // 通用标签样式
    QString sectionStyle = R"(
        color: #ffaa00;
        font-size: 24px;
        font-weight: bold;
        font-family: 'Microsoft YaHei';
        background-color: rgba(0, 0, 0, 150);
        border-radius: 8px;
        padding: 10px;
    )";
    
    QString contentStyle = R"(
        color: white;
        font-size: 18px;
        font-family: 'Microsoft YaHei';
        background-color: rgba(0, 0, 0, 100);
        border-radius: 8px;
        padding: 15px;
        line-height: 1.6;
    )";
    
    // 游戏目标
    QLabel* goalTitle = new QLabel("🎯 游戏目标", contentWidget);
    goalTitle->setStyleSheet(sectionStyle);
    contentLayout->addWidget(goalTitle);
    
    QLabel* goalContent = new QLabel(
        "控制小狮子收集所有蔬菜，避开危险的岩浆和飞箭，到达终点完成关卡！\n"
        "每个关卡都有不同的挑战，需要巧妙运用跳跃和移动技巧。", contentWidget);
    goalContent->setStyleSheet(contentStyle);
    goalContent->setWordWrap(true);
    contentLayout->addWidget(goalContent);
    
    // 操作说明
    QLabel* controlTitle = new QLabel("🎮 操作说明", contentWidget);
    controlTitle->setStyleSheet(sectionStyle);
    contentLayout->addWidget(controlTitle);
    
    QLabel* controlContent = new QLabel(
        "• A 键 / ← 键：向左移动\n"
        "• D 键 / → 键：向右移动\n"
        "• K 键 ：跳跃\n"
        "• SHIFT: 冲刺\n"
        "• ESC 键：暂停游戏\n", contentWidget);
    controlContent->setStyleSheet(contentStyle);
    controlContent->setWordWrap(true);
    contentLayout->addWidget(controlContent);
    
    // 游戏元素
    QLabel* elementsTitle = new QLabel("🧩 游戏元素", contentWidget);
    elementsTitle->setStyleSheet(sectionStyle);
    contentLayout->addWidget(elementsTitle);
    
    QLabel* elementsContent = new QLabel(
        "• 🥕 蔬菜：收集所有蔬菜才能完成关卡\n"
        "• 🔥 岩浆：接触后会死亡，需要小心避开\n"
        "• 🏹 箭矢陷阱：会定期发射箭矢，注意躲避\n"
        "• 📦 移动平台：可以站立的移动平台\n"
        "• 🚪 开关门：踩踏开关可以开启对应的门\n"
        "• 💧 水域：在水中移动速度会变慢", contentWidget);
    elementsContent->setStyleSheet(contentStyle);
    elementsContent->setWordWrap(true);
    contentLayout->addWidget(elementsContent);
    
    // 游戏提示
    QLabel* tipsTitle = new QLabel("💡 游戏提示", contentWidget);
    tipsTitle->setStyleSheet(sectionStyle);
    contentLayout->addWidget(tipsTitle);
    
    QLabel* tipsContent = new QLabel(
        "• 仔细观察关卡布局，规划最佳路线\n"
        "• 注意箭矢陷阱的发射规律，找准时机通过\n"
        "• 善用移动平台到达高处或跨越障碍\n"
        "• 开关门需要踩踏对应的开关才能开启\n"
        "• 在水中时要预留更多时间进行移动\n"
        "• 佩戴耳机可以获得更好的游戏体验", contentWidget);
    tipsContent->setStyleSheet(contentStyle);
    tipsContent->setWordWrap(true);
    contentLayout->addWidget(tipsContent);
    
    // 开发信息
    QLabel* devTitle = new QLabel("👨‍💻 开发信息", contentWidget);
    devTitle->setStyleSheet(sectionStyle);
    contentLayout->addWidget(devTitle);
    
    QLabel* devContent = new QLabel(
        "游戏名称：醒狮跃境\n"
        "开发团队：游戏开发小组\n"
        "开发框架：Qt 6 + C++\n"
        "版本：1.0.0\n"
        "感谢您的游玩！", contentWidget);
    devContent->setStyleSheet(contentStyle);
    devContent->setWordWrap(true);
    contentLayout->addWidget(devContent);
    
    contentLayout->addStretch();
}
