#include "LevelSelect.h"
#include "Config.h"
#include <QLabel>
#include <QPixmap>
#include <QPalette>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QDebug>

LevelSelect::LevelSelect(QWidget *parent)
    : QMainWindow(parent)
    , level_buttons(nullptr)
    , back_button(nullptr)
    , back_button_label(nullptr)
{
    for (int i = 0; i < MAX_LEVELS; ++i) {
        level_unlocked[i] = (i == 0);
    }
    ui_load();
    loadGameProgress();
}

void LevelSelect::ui_load()
{
    this->resize(1280, 720);
    
    QPixmap back(":/back/Picture/pickbkg.jpg");
    QPixmap scaledBack = back.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Window, scaledBack);
    this->setPalette(palette);
    
    level_buttons = new QPushButton*[MAX_LEVELS];
    
    const int buttonSize = 60;
    const QPoint levelPositions[MAX_LEVELS] = {
        QPoint(200, 500),
        QPoint(350, 450),
        QPoint(500, 400),
        QPoint(650, 350),
        QPoint(800, 300),
        QPoint(950, 250)
    };
    
    for (int i = 0; i < MAX_LEVELS; ++i) {
        level_buttons[i] = new QPushButton(this);
        level_buttons[i]->setText(QString::number(i + 1));
        level_buttons[i]->setGeometry(levelPositions[i].x(), levelPositions[i].y(), buttonSize, buttonSize);
        level_buttons[i]->setProperty("levelIndex", i);
        connect(level_buttons[i], &QPushButton::clicked, this, &LevelSelect::onLevelButtonClicked);
        updateLevelButtonStyle(i);
        level_buttons[i]->show();
    }
    
    // 创建返回按钮背景图片
    QPixmap backBtnImg(":/ui/Picture/backbtn.png");
    const int backBtnWidth = 100;
    const int backBtnHeight = 40;
    QPixmap scaledBackBtn = backBtnImg.scaled(backBtnWidth, backBtnHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 创建背景标签
    back_button_label = new QLabel(this);
    back_button_label->setPixmap(scaledBackBtn);
    back_button_label->setGeometry(50, 50, backBtnWidth, backBtnHeight);
    back_button_label->show();
    
    // 创建透明按钮叠加在背景上
    back_button = new QPushButton("返回", this);
    back_button->setGeometry(20, 50, backBtnWidth, backBtnHeight);
    back_button->setStyleSheet("QPushButton { background-color: transparent; color: white; font-size: 16px; font-weight: bold; border: none; } QPushButton:hover { color: #ffff00; } QPushButton:pressed { color: #ff557f; }");
    connect(back_button, &QPushButton::clicked, this, &LevelSelect::onBackButtonClicked);
    back_button->raise();
    back_button->show();
}

void LevelSelect::setLevelUnlocked(int levelIndex, bool unlocked)
{
    if (levelIndex >= 0 && levelIndex < MAX_LEVELS) {
        level_unlocked[levelIndex] = unlocked;
        updateLevelButtonStyle(levelIndex);
    }
}

bool LevelSelect::isLevelUnlocked(int levelIndex) const
{
    if (levelIndex >= 0 && levelIndex < MAX_LEVELS) {
        return level_unlocked[levelIndex];
    }
    return false;
}

void LevelSelect::updateLevelButtonStyle(int levelIndex)
{
    if (levelIndex < 0 || levelIndex >= MAX_LEVELS || !level_buttons[levelIndex]) {
        return;
    }
    
    QPushButton* button = level_buttons[levelIndex];
    
    if (level_unlocked[levelIndex]) {
        button->setStyleSheet("QPushButton { background-color: rgba(0, 200, 0, 200); color: white; font-size: 20px; font-weight: bold; border: 3px solid white; border-radius: 30px; } QPushButton:hover { background-color: rgba(0, 255, 0, 220); } QPushButton:pressed { background-color: rgba(0, 150, 0, 200); }");
        button->setEnabled(true);
    } else {
        button->setStyleSheet("QPushButton { background-color: rgba(100, 100, 100, 200); color: rgba(255, 255, 255, 100); font-size: 20px; font-weight: bold; border: 3px solid rgba(255, 255, 255, 100); border-radius: 30px; }");
        button->setEnabled(false);
    }
}

void LevelSelect::onLevelButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int levelIndex = button->property("levelIndex").toInt();
        if (isLevelUnlocked(levelIndex)) {
            qDebug() << "发射关卡选择信号：" << levelIndex; // 调试输出
            emit levelSelected(levelIndex);
        }
    }
}

void LevelSelect::onBackButtonClicked()
{
    emit backToMenu();
}

LevelSelect::~LevelSelect()
{
    if (level_buttons) {
        for (int i = 0; i < MAX_LEVELS; ++i) {
            delete level_buttons[i];
        }
        delete[] level_buttons;
    }
}

void LevelSelect::loadGameProgress()
{
    QString saveDir = getDataDirectory();
    QString saveFilePath = saveDir + "/game_progress.json";
    
    QFile saveFile(saveFilePath);
    if (!saveFile.exists()) {
        // 如果文件不存在，只有第1关解锁
        qDebug() << "游戏进度文件不存在，只有第1关解锁";
        
        // 重置所有关卡为锁定状态
        for (int i = 0; i < MAX_LEVELS; ++i) {
            level_unlocked[i] = false;
        }
        
        // 第1关默认解锁
        level_unlocked[0] = true;
        
        // 更新所有按钮的样式
        for (int i = 0; i < MAX_LEVELS; ++i) {
            updateLevelButtonStyle(i);
        }
        
        return;
    }
    
    if (saveFile.open(QIODevice::ReadOnly)) {
        QByteArray saveData = saveFile.readAll();
        saveFile.close();
        
        QJsonDocument loadDoc = QJsonDocument::fromJson(saveData);
        QJsonObject progress = loadDoc.object();
        
        // 获取已解锁的关卡列表
        QJsonArray unlockedLevels = progress["unlockedLevels"].toArray();
        
        // 重置所有关卡为锁定状态
        for (int i = 0; i < MAX_LEVELS; ++i) {
            level_unlocked[i] = false;
        }
        
        // 根据进度数据解锁关卡
        for (const QJsonValue& value : unlockedLevels) {
            int levelIndex = value.toInt() - 1;  // 转换为0基索引
            if (levelIndex >= 0 && levelIndex < MAX_LEVELS) {
                level_unlocked[levelIndex] = true;
                qDebug() << "关卡" << (levelIndex + 1) << "已解锁";
            }
        }
        
        // 更新所有按钮的样式
        for (int i = 0; i < MAX_LEVELS; ++i) {
            updateLevelButtonStyle(i);
        }
        
        qDebug() << "游戏进度加载完成";
    } else {
        qDebug() << "无法读取游戏进度文件：" << saveFilePath;
    }
}
