/**
 * @file SettingsPage.h
 * @brief 游戏设置页面类声明
 * @author 开发团队
 * @date 2025-10-16
 * @version 0.1.0
 */

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "GameSettings.h"

/**
 * @class SettingsPage
 * @brief 游戏设置页面，负责设置界面加载与展示
 */
class SettingsPage : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造设置页面窗口
     * @param parent 父窗口指针
     */
    explicit SettingsPage(QWidget *parent = nullptr);

    /**
     * @brief 析构函数，释放UI资源
     */
    ~SettingsPage();

    /**
     * @brief 加载并布局设置界面
     */
    void ui_load();

    /**
     * @brief 获取当前设置
     * @return 当前游戏设置引用
     */
    GameSettings& getSettings() const;

signals:
    /**
     * @brief 返回菜单信号
     */
    void backToMenu();

private slots:
    /**
     * @brief 处理返回按钮点击事件
     */
    void onBackButtonClicked();

    /**
     * @brief 处理保存按钮点击事件
     */
    void onSaveButtonClicked();

    /**
     * @brief 处理音乐音量滑块变化
     * @param value 新的音量值
     */
    void onMusicVolumeChanged(int value);

    /**
     * @brief 处理音效音量滑块变化
     * @param value 新的音量值
     */
    void onSoundVolumeChanged(int value);

private:
    /**
     * @brief 从UI控件更新设置
     */
    void updateSettingsFromUI();

    /**
     * @brief 从设置更新UI控件
     */
    void updateUIFromSettings();

    // UI元素
    QPushButton* backButton;
    QPushButton* saveButton;
    
    // 音频设置
    QCheckBox* musicEnabledCheckBox;
    QSlider* musicVolumeSlider;
    QLabel* musicVolumeLabel;
    
    QCheckBox* soundEnabledCheckBox;
    QSlider* soundVolumeSlider;
    QLabel* soundVolumeLabel;
    
    // 显示设置
    QCheckBox* fullscreenCheckBox;
    QComboBox* resolutionComboBox;
    
    // 游戏设置
    QComboBox* difficultyComboBox;
    QCheckBox* showTutorialCheckBox;
    
    // 控制设置
    QCheckBox* invertYAxisCheckBox;
    QSlider* sensitivitySlider;
    QLabel* sensitivityLabel;
};

#endif // SETTINGSPAGE_H