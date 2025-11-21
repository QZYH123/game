/**
 * @file menu.cpp
 * @brief 主菜单窗口实现
 */

#include "menu.h"
#include "ui_menu.h"
#include "LevelSelect.h"
#include "SettingsPage.h"
#include "LevelEditor.h"
#include "LevelData.h"
#include "HelpPage.h"
#include "Config.h"
#include "AudioController.h"
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>

/**
 * @brief HoverSoundButton的enterEvent实现，播放悬浮音效
 */
void HoverSoundButton::enterEvent(QEnterEvent *event)
{
    AudioController::getInstance().playSound(SoundType::Float);
    QPushButton::enterEvent(event);
}

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
    , help_page(nullptr)
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
    
    // 创建帮助页面
    help_page = new HelpPage();
    help_page->hide();
    
    ui_load();
    
    // 连接信号槽
    connect(level_select, SIGNAL(backToMenu()), this, SLOT(onBackFromLevelSelect()));
    connect(settings_page, SIGNAL(backToMenu()), this, SLOT(onBackFromSettings()));
    connect(help_page, SIGNAL(backToMenu()), this, SLOT(onBackFromHelp()));
}
/**
 * @brief 加载主菜单界面元素并设置样式与布局
 */
void menu::ui_load(){
    AudioController::getInstance().playBackgroundMusic();
    const int btnHeight = 80;
    const int btnWidth = 180;
    const int intervalY = 110;
    const int xPos = 1020;
    const int startY = 80;
    buttons = new HoverSoundButton*[6]; // 添加Help按钮，共6个按钮

    QPixmap back(":/images/menu.jpg");
    // 背景按窗口比例平滑缩放，避免失真
    QPixmap scaledback = back.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Window, scaledback);
    this->setPalette(palette);
    this->resize(1280, 720);
    //背景
    QPixmap title(":/images/title.png");
    QLabel *title_label = new QLabel(this);
    QPixmap scaledtitle = title.scaled(540,300, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    title_label->setPixmap(scaledtitle);
    title_label->setGeometry(35, 410, 540, 300);//9:5
    title_label->show();
    //标识
    QPixmap button(":/images/button_y.png");
    QPixmap scaledbutton = button.scaled(200, 100,
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);
    for (int i = 0; i < 6; ++i) {
        buttons[i] = new HoverSoundButton(this);
    }
    // 重新分配按钮文本（添加 Help）
    buttons[0]->setText("Start");
    buttons[1]->setText("Custom Level");
    buttons[2]->setText("Level Editor");
    buttons[3]->setText("Settings");
    buttons[4]->setText("Help");
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
    connect(buttons[1], SIGNAL(clicked()), this, SLOT(onCustomLevelButtonClicked()));
    connect(buttons[2], SIGNAL(clicked()), this, SLOT(onLevelEditorButtonClicked()));
    connect(buttons[3], SIGNAL(clicked()), this, SLOT(onSettingsButtonClicked()));
    connect(buttons[4], SIGNAL(clicked()), this, SLOT(onHelpButtonClicked()));
    connect(buttons[5], SIGNAL(clicked()), this, SLOT(close()));
    connect(level_select, &LevelSelect::levelSelected, this, &menu::onLevelSelected);
}
void menu::onStartButtonClicked()
{
    AudioController::getInstance().playSound(SoundType::Click);
    // 隐藏主菜单，显示选关界面
    this->hide();
    level_select->loadGameProgress();  // 重新加载游戏进度
    level_select->show();
}

// 已移除继续游戏按钮与逻辑

void menu::onBackFromLevelSelect()
{
    AudioController::getInstance().playSound(SoundType::Click);
    // 隐藏选关界面，显示主菜单
    level_select->hide();
    this->show();
}

void menu::onSettingsButtonClicked()
{
    AudioController::getInstance().playSound(SoundType::Click);
    // 隐藏主菜单，显示设置页面
    this->hide();
    settings_page->show();
}

void menu::onBackFromSettings()
{
    // AudioController::getInstance().playSound(SoundType::Click);
    // 隐藏设置页面，显示主菜单
    settings_page->hide();
    this->show();
}
// 实现关卡选择处理函数
void menu::onLevelSelected(int levelIndex)
{
    AudioController::getInstance().playSound(SoundType::Click);
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
    AudioController::getInstance().playSound(SoundType::Click);
    // 创建关卡编辑器（如果还没有创建）
    if (!level_editor) {
        level_editor = new LevelEditor(nullptr);
        
        // 连接返回信号
        connect(level_editor, &LevelEditor::backToMenu, this, &menu::onBackFromLevelEditor);
        
        // 连接测试关卡信号
        connect(level_editor, &LevelEditor::testLevel, this, &menu::onTestLevel);
    }
    
    // 隐藏主菜单，显示关卡编辑器
    this->hide();
    level_editor->show();
}

void menu::onBackFromLevelEditor()
{
    AudioController::getInstance().playSound(SoundType::Click);
    // 隐藏关卡编辑器，显示主菜单
    if (level_editor) {
        level_editor->hide();
    }
    this->show();
}

void menu::onTestLevel(LevelData* levelData)
{
    AudioController::getInstance().playSound(SoundType::Click);
    
    // 初始化游戏场景（如果还没有创建）
    if (!gameScene) {
        gameScene = new GameScene(nullptr);
        
        // 连接游戏场景返回信号
        connect(gameScene, &GameScene::backToMainMenu, this, [this]() {
            gameScene->hide();
            this->show();
        });
        
        // 连接游戏结束信号
        connect(gameScene, &GameScene::gameFinished, this, [this]() {
            if (gameScene) {
                gameScene->resetLevel();
                gameScene->gameStart();
            }
        });
        
        // 连接返回关卡选择信号
        connect(gameScene, &GameScene::backToLevelSelect, this, [this]() {
            gameScene->hide();
            if (level_editor) {
                level_editor->show();
            } else {
                this->show();
            }
        });
    }
    
    // 隐藏关卡编辑器
    if (level_editor) {
        level_editor->hide();
    }
    
    // 使用临时文件加载关卡进行测试
    QString tempPath = QApplication::applicationDirPath() + "/temp_level.json";
    if (gameScene->loadLevelFromFile(tempPath)) {
        gameScene->show();
        qDebug() << "开始测试关卡";
    } else {
        QMessageBox::warning(this, "错误", "无法加载测试关卡");
        // 如果加载失败，重新显示关卡编辑器
        if (level_editor) {
            level_editor->show();
        }
    }
}

void menu::onCustomLevelButtonClicked()
{
    AudioController::getInstance().playSound(SoundType::Click);
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

void menu::onHelpButtonClicked()
{
    AudioController::getInstance().playSound(SoundType::Click);
    this->hide();
    help_page->show();
}

void menu::onBackFromHelp()
{
    help_page->hide();
    this->show();
}

menu::~menu()
{
    delete ui;
    delete[] buttons;
    delete level_select;
    delete settings_page;
    delete level_editor;
    delete help_page;
    delete gameScene;
}

#include "menu.moc"
