/**
 * @file menu.cpp
 * @brief 主菜单窗口实现
 */

#include "menu.h"
#include "ui_menu.h"
#include "LevelSelect.h"
#include "SettingsPage.h"
#include <QLabel>

/**
 * @brief 构造主菜单窗口并初始化界面
 * @param parent 父窗口指针
 */
menu::menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::menu)
    , level_select(nullptr)
    , settings_page(nullptr)
    //新增gameScene初始化
    ,gameScene(nullptr)
{
    ui->setupUi(this);

    
    // 创建选关界面
    level_select = new LevelSelect();
    level_select->hide();
    
    // 创建设置页面
    settings_page = new SettingsPage();
    settings_page->hide();
    ui_load();
    
    // 连接信号槽
    connect(level_select, SIGNAL(backToMenu()), this, SLOT(onBackFromLevelSelect()));
    connect(settings_page, SIGNAL(backToMenu()), this, SLOT(onBackFromSettings()));
}
/**
 * @brief 加载主菜单界面元素并设置样式与布局
 */
void menu::ui_load(){
    const int btnHeight = 80;
    const int btnWidth = 180;
    const int intervalY = 110;
    const int xPos = 1020;
    const int startY = 80;
    buttons = new QPushButton*[4];

    QPixmap back(":/back/Picture/menu.jpg");
    // 背景按窗口比例平滑缩放，避免失真
    QPixmap scaledback = back.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Window, scaledback);
    this->setPalette(palette);
    this->resize(1280, 720);
    //背景
    QPixmap title(":/ui/Picture/title.png");
    QLabel *title_label = new QLabel(this);
    QPixmap scaledtitle = title.scaled(540,300, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    title_label->setPixmap(scaledtitle);
    title_label->setGeometry(35, 410, 540, 300);//9:5
    title_label->show();
    //标识
    QPixmap button(":/ui/Picture/button_y.png");
    QPixmap scaledbutton = button.scaled(200, 100,
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);
    for (int i = 0; i < 4; ++i) {
        buttons[i] = new QPushButton(this);
    }
    buttons[0]->setText("Start");
    buttons[1]->setText("Continue");
    buttons[2]->setText("Settings");
    buttons[3]->setText("Exit");
    for (int i = 0; i < 4; ++i)
    {
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(scaledbutton);
        imageLabel->setGeometry(xPos, startY + i * intervalY, btnWidth, btnHeight);
        imageLabel->show();
        /*    ui->Exit_Button->setStyleSheet("QPushButton { color: #ffffff; }"
                                   "QPushButton:hover { color: #ffff00; }"
                                   "QPushButton:pressed { color: #ff557f; }");*/
        buttons[i]->setStyleSheet(R"(
                QPushButton {
                    background-color: transparent; /* 背景透明 */
                    color: white;                  /* 字体白色 */
                    font-size: 25px;               /* 字体大小（可选） */
                    font-family: 'Script MT';
                    font-style: italic;
                    font-weight: bold;
                    border: none;                  /* 去掉边框（可选，更简洁） */
                    /* 若需要边框可改为：border: 1px solid rgba(255,255,255,0.5); */
                }
                QPushButton:hover {
                    color: #ffff00; /* 鼠标悬停时轻微高亮（可选） */
                }
                QPushButton:pressed {
                    color: #ff557f; /* 点击时加深高亮（可选） */
                }
            )");

        buttons[i]->setGeometry(xPos, startY + i * intervalY, btnWidth, btnHeight);
        buttons[i]->raise();
    }
    
    // 连接按钮的信号槽
    connect(buttons[0], SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(buttons[2], SIGNAL(clicked()), this, SLOT(onSettingsButtonClicked()));
    connect(level_select, &LevelSelect::levelSelected, this, &menu::onLevelSelected);
}
void menu::onStartButtonClicked()
{
    // 隐藏主菜单，显示选关界面
    this->hide();
    level_select->show();
}

void menu::onBackFromLevelSelect()
{
    // 隐藏选关界面，显示主菜单
    level_select->hide();
    this->show();
}

void menu::onSettingsButtonClicked()
{
    // 隐藏主菜单，显示设置页面
    this->hide();
    settings_page->show();
}

void menu::onBackFromSettings()
{
    // 隐藏设置页面，显示主菜单
    settings_page->hide();
    this->show();
}
// 实现关卡选择处理函数
void menu::onLevelSelected(int levelIndex)
{
    qDebug() << "收到关卡索引：" << levelIndex; // 检查是否打印
    // 隐藏选关界面
    level_select->hide();

    // 初始化游戏场景（仅测试第一关，即levelIndex=0）
    if (levelIndex == 0) {
        if (!gameScene) {
            gameScene = new GameScene(nullptr);
            // 连接游戏场景返回信号
        //     connect(gameScene, &GameScene::backToLevelSelect, this, [this]() {
        //         gameScene->hide();
        //         level_select->show();
        //     });
         }
        qDebug() << "显示GameScene"; // 确认是否执行到此处
        gameScene->show();
    }
}
/**
 * @brief 析构并释放UI资源
 */
menu::~menu()
{
    delete ui;
    delete level_select;
    delete settings_page;
}
