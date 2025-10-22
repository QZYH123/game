/**
 * @file menu.cpp
 * @brief 主菜单窗口实现
 */

#include "menu.h"
#include "ui_menu.h"
#include "LevelSelect.h"
#include "SettingsPage.h"
#include "LevelEditor.h"
#include "Config.h"
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>

/**
 * @brief 构造主菜单窗口并初始化界面
 * @param parent 父窗口指针
 */
menu::menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::menu)
    , level_select(nullptr)
    , settings_page(nullptr)
    , level_editor(nullptr)
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
    buttons = new QPushButton*[6]; // 增加一个按钮用于加载自定义关卡

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
    for (int i = 0; i < 6; ++i) {
        buttons[i] = new QPushButton(this);
    }
    buttons[0]->setText("Start");
    buttons[1]->setText("Continue");
    buttons[2]->setText("Custom Level"); // 新增自定义关卡按钮
    buttons[3]->setText("Level Editor");
    buttons[4]->setText("Settings");
    buttons[5]->setText("Exit");
    for (int i = 0; i < 6; ++i)
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
    connect(buttons[1], SIGNAL(clicked()), this, SLOT(onContinueButtonClicked()));
    connect(buttons[2], SIGNAL(clicked()), this, SLOT(onCustomLevelButtonClicked()));
    connect(buttons[3], SIGNAL(clicked()), this, SLOT(onLevelEditorButtonClicked()));
    connect(buttons[4], SIGNAL(clicked()), this, SLOT(onSettingsButtonClicked()));
    connect(buttons[5], SIGNAL(clicked()), this, SLOT(close()));
    connect(level_select, &LevelSelect::levelSelected, this, &menu::onLevelSelected);
}
void menu::onStartButtonClicked()
{
    // 隐藏主菜单，显示选关界面
    this->hide();
    level_select->loadGameProgress();  // 重新加载游戏进度
    level_select->show();
}

void menu::onContinueButtonClicked()
{
    // 检查是否有保存的游戏进度
    QString saveDir = getDataDirectory();
    QString saveFilePath = saveDir + "/game_progress.json";
    
    QFile saveFile(saveFilePath);
    if (!saveFile.exists()) {
        // 没有游戏记录，显示提示
        QMessageBox::information(this, "提示", "暂无游戏记录，请先开始新游戏！");
        return;
    }
    
    if (!saveFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取游戏进度文件！");
        return;
    }
    
    // 读取JSON数据
    QByteArray saveData = saveFile.readAll();
    saveFile.close();
    
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject saveObject = loadDoc.object();
    
    if (saveObject.contains("lastLevel") && saveObject["lastLevel"].isDouble()) {
        int lastLevel = saveObject["lastLevel"].toInt();
        
        // 直接启动上次挑战的关卡
        this->hide();
        
        if (gameScene) {
            delete gameScene;
        }
        
        gameScene = new GameScene(this);
        gameScene->loadLevel(lastLevel);
        gameScene->show();
        
        // 连接游戏结束信号
        connect(gameScene, &GameScene::gameFinished, this, [this]() {
            gameScene->hide();
            this->show();
            // 重置游戏状态，为下次游戏做准备
            if (gameScene) {
                gameScene->resetLevel();
                // 重新启动游戏，确保定时器正常工作
                gameScene->gameStart();
            }
        });
    } else {
        QMessageBox::warning(this, "错误", "游戏进度文件格式错误！");
    }
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

    // 初始化游戏场景
    if (!gameScene) {
        gameScene = new GameScene(nullptr);
        
        // 连接游戏场景返回信号
        connect(gameScene, &GameScene::backToMainMenu, this, [this]() {
            gameScene->hide();
            this->show();
        });
        
        // 连接游戏结束信号，确保游戏状态重置
        connect(gameScene, &GameScene::gameFinished, this, [this]() {
            // 游戏结束时重置游戏状态，为下次游戏做准备
            if (gameScene) {
                gameScene->resetLevel();
                // 重新启动游戏，确保定时器正常工作
                gameScene->gameStart();
            }
        });
        
        connect(gameScene, &GameScene::backToLevelSelect, this, [this]() {
            gameScene->hide();
            level_select->loadGameProgress();  // 重新加载游戏进度
            level_select->show();
        });
    }
    
    // 加载指定关卡
    gameScene->loadLevel(levelIndex);
    qDebug() << "显示GameScene"; // 确认是否执行到此处
    gameScene->show();
}
/**
 * @brief 析构并释放UI资源
 */
void menu::onLevelEditorButtonClicked()
{
    // 创建关卡编辑器（如果还没有创建）
    if (!level_editor) {
        level_editor = new LevelEditor(nullptr);
        
        // 连接返回信号
        connect(level_editor, &LevelEditor::backToMenu, this, &menu::onBackFromLevelEditor);
    }
    
    // 隐藏主菜单，显示关卡编辑器
    this->hide();
    level_editor->show();
}

void menu::onBackFromLevelEditor()
{
    // 隐藏关卡编辑器，显示主菜单
    if (level_editor) {
        level_editor->hide();
    }
    this->show();
}

void menu::onCustomLevelButtonClicked()
{
    // 创建文件选择对话框
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择关卡文件",
        LevelManager::getInstance().getLevelsDirectory(),
        "关卡文件 (*.json)"
    );
    
    // 如果用户取消了选择，直接返回
    if (filePath.isEmpty()) {
        return;
    }
    
    // 初始化游戏场景
    if (!gameScene) {
        gameScene = new GameScene(nullptr);
        
        // 连接游戏场景返回信号
        connect(gameScene, &GameScene::backToMainMenu, this, [this]() {
            gameScene->hide();
            this->show();
        });
        
        connect(gameScene, &GameScene::backToLevelSelect, this, [this]() {
            gameScene->hide();
            level_select->loadGameProgress();  // 重新加载游戏进度
            level_select->show();
        });
    }
    
    // 从文件加载关卡
    if (gameScene->loadLevelFromFile(filePath)) {
        // 隐藏主菜单，显示游戏场景
        this->hide();
        gameScene->show();
    } else {
        QMessageBox::warning(this, "加载失败", "无法加载选择的关卡文件，请确认文件格式正确。");
    }
}

menu::~menu()
{
    delete ui;
    delete level_select;
    delete settings_page;
}
