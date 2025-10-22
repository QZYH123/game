/**
 * @file SettingsPage.cpp
 * @brief 游戏设置页面实现
 */

#include "SettingsPage.h"
#include <QPalette>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>

/**
 * @brief 构造设置页面窗口并初始化界面
 * @param parent 父窗口指针
 */
SettingsPage::SettingsPage(QWidget *parent)
    : QMainWindow(parent)
{
    GameSettings& settings = GameSettings::getInstance();
    
    // 加载UI
    ui_load();
    
    // 从设置更新UI
    updateUIFromSettings();
}

/**
 * @brief 加载设置界面元素并设置样式与布局
 */
void SettingsPage::ui_load() {
    // 设置窗口大小
    this->resize(1280, 720);
    
    // 设置背景图片
    QPixmap back(":/back/Picture/setting_page.png");
    QPixmap scaledback = back.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Window, scaledback);
    this->setPalette(palette);
    
    // 创建中央部件和布局
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    // 创建标题标签
    QLabel* titleLabel = new QLabel("游戏设置", this);
    titleLabel->setStyleSheet("font-size: 36px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    
    // 创建滚动区域来容纳设置选项
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; border: none; }"
                             "QScrollBar:vertical { background: rgba(0, 0, 0, 100); width: 10px; margin: 0px; }"
                             "QScrollBar::handle:vertical { background: rgba(255, 255, 255, 150); min-height: 20px; border-radius: 5px; }"
                             "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
                             "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }");
    
    QWidget* scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(10, 10, 10, 10);
    scrollArea->setWidget(scrollContent);
    
    // 创建设置组
    QGroupBox* audioGroup = new QGroupBox("音频设置", this);
    audioGroup->setStyleSheet("QGroupBox { color: white; font-size: 18px; font-weight: bold; }");
    QVBoxLayout* audioLayout = new QVBoxLayout(audioGroup);
    
    // 音乐设置
    QHBoxLayout* musicLayout = new QHBoxLayout();
    musicEnabledCheckBox = new QCheckBox("启用背景音乐", this);
    musicEnabledCheckBox->setStyleSheet("color: white; font-size: 16px;");
    musicLayout->addWidget(musicEnabledCheckBox);
    
    musicVolumeSlider = new QSlider(Qt::Horizontal, this);
    musicVolumeSlider->setRange(0, 100);
    musicVolumeSlider->setStyleSheet("QSlider::groove:horizontal { background: #555; height: 8px; border-radius: 4px; }"
                                    "QSlider::handle:horizontal { background: #ffff00; width: 18px; margin: -5px 0; border-radius: 9px; }");
    musicLayout->addWidget(musicVolumeSlider, 1);
    
    musicVolumeLabel = new QLabel("80%", this);
    musicVolumeLabel->setStyleSheet("color: white; font-size: 16px;");
    musicVolumeLabel->setMinimumWidth(50);
    musicLayout->addWidget(musicVolumeLabel);
    
    audioLayout->addLayout(musicLayout);
    
    // 音效设置
    QHBoxLayout* soundLayout = new QHBoxLayout();
    soundEnabledCheckBox = new QCheckBox("启用音效", this);
    soundEnabledCheckBox->setStyleSheet("color: white; font-size: 16px;");
    soundLayout->addWidget(soundEnabledCheckBox);
    
    soundVolumeSlider = new QSlider(Qt::Horizontal, this);
    soundVolumeSlider->setRange(0, 100);
    soundVolumeSlider->setStyleSheet("QSlider::groove:horizontal { background: #555; height: 8px; border-radius: 4px; }"
                                    "QSlider::handle:horizontal { background: #ffff00; width: 18px; margin: -5px 0; border-radius: 9px; }");
    soundLayout->addWidget(soundVolumeSlider, 1);
    
    soundVolumeLabel = new QLabel("80%", this);
    soundVolumeLabel->setStyleSheet("color: white; font-size: 16px;");
    soundVolumeLabel->setMinimumWidth(50);
    soundLayout->addWidget(soundVolumeLabel);
    
    audioLayout->addLayout(soundLayout);
    scrollLayout->addWidget(audioGroup);
    scrollLayout->addSpacing(10);
    
    // 显示设置
    QGroupBox* displayGroup = new QGroupBox("显示设置", this);
    displayGroup->setStyleSheet("QGroupBox { color: white; font-size: 18px; font-weight: bold; }");
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    
    // 全屏设置
    fullscreenCheckBox = new QCheckBox("全屏模式", this);
    fullscreenCheckBox->setStyleSheet("color: white; font-size: 16px;");
    displayLayout->addWidget(fullscreenCheckBox);
    
    // 分辨率设置
    QHBoxLayout* resolutionLayout = new QHBoxLayout();
    QLabel* resolutionLabel = new QLabel("分辨率:", this);
    resolutionLabel->setStyleSheet("color: white; font-size: 16px;");
    resolutionLayout->addWidget(resolutionLabel);
    
    resolutionComboBox = new QComboBox(this);
    resolutionComboBox->addItem("1280 x 720");
    resolutionComboBox->addItem("1920 x 1080");
    resolutionComboBox->addItem("2560 x 1440");
    resolutionComboBox->setStyleSheet("QComboBox { background-color: #333; color: white; padding: 5px; min-width: 150px; }"
                                     "QComboBox::drop-down { width: 20px; }"
                                     "QComboBox QAbstractItemView { background-color: #333; color: white; selection-background-color: #555; min-width: 150px; }");
    resolutionLayout->addWidget(resolutionComboBox);
    resolutionLayout->addStretch();
    
    displayLayout->addLayout(resolutionLayout);
    scrollLayout->addWidget(displayGroup);
    scrollLayout->addSpacing(10);
    
    // 游戏设置
    QGroupBox* gameGroup = new QGroupBox("游戏设置", this);
    gameGroup->setStyleSheet("QGroupBox { color: white; font-size: 18px; font-weight: bold; }");
    QVBoxLayout* gameLayout = new QVBoxLayout(gameGroup);
    
    // 难度设置
    QHBoxLayout* difficultyLayout = new QHBoxLayout();
    QLabel* difficultyLabel = new QLabel("游戏难度:", this);
    difficultyLabel->setStyleSheet("color: white; font-size: 16px;");
    difficultyLayout->addWidget(difficultyLabel);
    
    difficultyComboBox = new QComboBox(this);
    difficultyComboBox->addItem("简单");
    difficultyComboBox->addItem("普通");
    difficultyComboBox->addItem("困难");
    difficultyComboBox->setStyleSheet("QComboBox { background-color: #333; color: white; padding: 5px; min-width: 150px; }"
                                     "QComboBox::drop-down { width: 20px; }"
                                     "QComboBox QAbstractItemView { background-color: #333; color: white; selection-background-color: #555; min-width: 150px; }");
    difficultyLayout->addWidget(difficultyComboBox);
    difficultyLayout->addStretch();
    
    gameLayout->addLayout(difficultyLayout);
    
    // 教程设置
    showTutorialCheckBox = new QCheckBox("显示游戏教程", this);
    showTutorialCheckBox->setStyleSheet("color: white; font-size: 16px;");
    gameLayout->addWidget(showTutorialCheckBox);
    
    scrollLayout->addWidget(gameGroup);
    scrollLayout->addSpacing(10);
    
    // 控制设置
    QGroupBox* controlGroup = new QGroupBox("控制设置", this);
    controlGroup->setStyleSheet("QGroupBox { color: white; font-size: 18px; font-weight: bold; }");
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);
    
    // Y轴反转
    invertYAxisCheckBox = new QCheckBox("反转Y轴", this);
    invertYAxisCheckBox->setStyleSheet("color: white; font-size: 16px;");
    controlLayout->addWidget(invertYAxisCheckBox);
    
    // 灵敏度设置
    QHBoxLayout* sensitivityLayout = new QHBoxLayout();
    QLabel* sensitivityTextLabel = new QLabel("控制灵敏度:", this);
    sensitivityTextLabel->setStyleSheet("color: white; font-size: 16px;");
    sensitivityLayout->addWidget(sensitivityTextLabel);
    
    sensitivitySlider = new QSlider(Qt::Horizontal, this);
    sensitivitySlider->setRange(1, 10);
    sensitivitySlider->setStyleSheet("QSlider::groove:horizontal { background: #555; height: 8px; border-radius: 4px; }"
                                    "QSlider::handle:horizontal { background: #ffff00; width: 18px; margin: -5px 0; border-radius: 9px; }");
    sensitivityLayout->addWidget(sensitivitySlider, 1);
    
    sensitivityLabel = new QLabel("5", this);
    sensitivityLabel->setStyleSheet("color: white; font-size: 16px;");
    sensitivityLabel->setMinimumWidth(30);
    sensitivityLayout->addWidget(sensitivityLabel);
    
    // 连接灵敏度滑块的值变化信号
    connect(sensitivitySlider, &QSlider::valueChanged, this, [this](int value) {
        sensitivityLabel->setText(QString::number(value));
    });
    
    controlLayout->addLayout(sensitivityLayout);
    scrollLayout->addWidget(controlGroup);
    
    // 添加滚动区域到主布局
    mainLayout->addWidget(scrollArea);
    
    // 添加按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    // 使用与菜单相同的按钮样式
    QPixmap buttonImg(":/ui/Picture/button_y.png");
    QPixmap scaledButtonImg = buttonImg.scaled(200, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 返回按钮
    backButton = new QPushButton("返回", this);
    backButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: white;
            font-size: 25px;
            font-family: 'Script MT';
            font-style: italic;
            font-weight: bold;
            border: none;
        }
        QPushButton:hover {
            color: #ffff00;
        }
        QPushButton:pressed {
            color: #ff557f;
        }
    )");
    
    // 保存按钮
    saveButton = new QPushButton("保存", this);
    saveButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: white;
            font-size: 25px;
            font-family: 'Script MT';
            font-style: italic;
            font-weight: bold;
            border: none;
        }
        QPushButton:hover {
            color: #ffff00;
        }
        QPushButton:pressed {
            color: #ff557f;
        }
    )");
    
    // 创建按钮容器
    QWidget* backBtnContainer = new QWidget();
    QHBoxLayout* backBtnLayout = new QHBoxLayout(backBtnContainer);
    backBtnLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* backBtnLabel = new QLabel();
    backBtnLabel->setPixmap(scaledButtonImg);
    backBtnLayout->addWidget(backBtnLabel);
    backBtnContainer->setLayout(backBtnLayout);
    backBtnContainer->setFixedSize(180, 80);
    backButton->setParent(backBtnContainer);
    backButton->setGeometry(0, 0, 180, 80);
    backButton->raise();
    
    QWidget* saveBtnContainer = new QWidget();
    QHBoxLayout* saveBtnLayout = new QHBoxLayout(saveBtnContainer);
    saveBtnLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* saveBtnLabel = new QLabel();
    saveBtnLabel->setPixmap(scaledButtonImg);
    saveBtnLayout->addWidget(saveBtnLabel);
    saveBtnContainer->setLayout(saveBtnLayout);
    saveBtnContainer->setFixedSize(180, 80);
    saveButton->setParent(saveBtnContainer);
    saveButton->setGeometry(0, 0, 180, 80);
    saveButton->raise();
    
    // 添加左侧间距
    buttonLayout->addSpacing(50);
    buttonLayout->addWidget(backBtnContainer);
    // 添加中间弹性间距
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveBtnContainer);
    // 添加右侧间距
    buttonLayout->addSpacing(50);
    
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(backButton, SIGNAL(clicked()), this, SLOT(onBackButtonClicked()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
    connect(musicVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onMusicVolumeChanged(int)));
    connect(soundVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onSoundVolumeChanged(int)));
    
    // 设置中央部件
    this->setCentralWidget(centralWidget);
}

/**
 * @brief 从UI控件更新设置
 */
void SettingsPage::updateSettingsFromUI() {
    GameSettings& settings = GameSettings::getInstance();
    settings.musicEnabled = musicEnabledCheckBox->isChecked();
    settings.musicVolume = musicVolumeSlider->value();
    settings.soundEnabled = soundEnabledCheckBox->isChecked();
    settings.soundVolume = soundVolumeSlider->value();
    
    settings.fullscreen = fullscreenCheckBox->isChecked();
    settings.resolution = resolutionComboBox->currentIndex();
    
    settings.difficulty = difficultyComboBox->currentIndex();
    settings.showTutorial = showTutorialCheckBox->isChecked();
    
    settings.invertYAxis = invertYAxisCheckBox->isChecked();
    settings.sensitivity = sensitivitySlider->value();
}

/**
 * @brief 从设置更新UI控件
 */
void SettingsPage::updateUIFromSettings() {
    GameSettings& settings = GameSettings::getInstance();
    musicEnabledCheckBox->setChecked(settings.musicEnabled);
    musicVolumeSlider->setValue(settings.musicVolume);
    musicVolumeLabel->setText(QString::number(settings.musicVolume) + "%");
    
    soundEnabledCheckBox->setChecked(settings.soundEnabled);
    soundVolumeSlider->setValue(settings.soundVolume);
    soundVolumeLabel->setText(QString::number(settings.soundVolume) + "%");
    
    fullscreenCheckBox->setChecked(settings.fullscreen);
    resolutionComboBox->setCurrentIndex(settings.resolution);
    
    difficultyComboBox->setCurrentIndex(settings.difficulty);
    showTutorialCheckBox->setChecked(settings.showTutorial);
    
    invertYAxisCheckBox->setChecked(settings.invertYAxis);
    sensitivitySlider->setValue(settings.sensitivity);
    sensitivityLabel->setText(QString::number(settings.sensitivity));
}

/**
 * @brief 处理返回按钮点击事件
 */
void SettingsPage::onBackButtonClicked() {
    emit backToMenu();
}

/**
 * @brief 处理保存按钮点击事件
 */
void SettingsPage::onSaveButtonClicked() {
    updateSettingsFromUI();
    if (GameSettings::getInstance().saveToFile()) {
        QMessageBox::information(this, "保存成功", "设置已成功保存！");
    } else {
        QMessageBox::warning(this, "保存失败", "无法保存设置，请检查文件权限。");
    }
}

/**
 * @brief 处理音乐音量滑块变化
 * @param value 新的音量值
 */
void SettingsPage::onMusicVolumeChanged(int value) {
    musicVolumeLabel->setText(QString::number(value) + "%");
}

/**
 * @brief 处理音效音量滑块变化
 * @param value 新的音量值
 */
void SettingsPage::onSoundVolumeChanged(int value) {
    soundVolumeLabel->setText(QString::number(value) + "%");
}

/**
 * @brief 获取当前设置
 * @return 当前游戏设置
 */
GameSettings& SettingsPage::getSettings() const {
    return GameSettings::getInstance();
}

/**
 * @brief 析构并释放UI资源
 */
SettingsPage::~SettingsPage() {
    // Qt会自动释放子控件
}