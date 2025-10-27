#include "AudioController.h"
#include "GameSettings.h"
#include <QDebug>

// 1. 初始化音效路径（保留原逻辑，精简调试）
const QMap<SoundType, QString> initSoundPaths() {
    QMap<SoundType, QString> paths;
    paths[SoundType::Jump] = "qrc:/sounds/Jump.mp3";
    paths[SoundType::Click] = "qrc:/sounds/Click.mp3";
    paths[SoundType::Float] = "qrc:/sounds/Float.mp3";
    paths[SoundType::Win] = "qrc:/sounds/Win.mp3";
    paths[SoundType::Collect] = "qrc:/sounds/Pick.mp3";
    return paths;
}
AudioController& AudioController::getInstance(QObject *parent) {
    static AudioController instance(parent);
    return instance;
}

// 2. 构造函数：初始化 Qt 6 音频组合（关键修改）
AudioController::AudioController(QObject *parent)
    : QObject(parent)
    , bgmPlayer(new QMediaPlayer(this))
    , bgmAudioOutput(new QAudioOutput(this))  // 初始化音量控制器
    , settings(GameSettings::getInstance())
    , soundEffect(new QSoundEffect)
    , soundPaths(initSoundPaths())
{
    // 绑定 Player 和 AudioOutput（Qt 6 强制要求）
    bgmPlayer->setAudioOutput(bgmAudioOutput);

    // 初始化BGM列表（仅保留核心路径，精简冗余）
    bgmFiles << "qrc:/sounds/bgm1.mp3"
             << "qrc:/sounds/bgm2.mp3"
             << "qrc:/sounds/bgm3.mp3";

    // 初始化音量（从配置读取，0-100转0.0-1.0浮点值）
    float initVol = settings.musicVolume / 100.0f;
    bgmAudioOutput->setVolume(initVol);  // Qt 6 音量控制入口
    soundEffect->setVolume(settings.soundVolume / 100.0f);

    // // 音效线程初始化（保留原逻辑）
    // soundEffect->moveToThread(&soundThread);
    // soundThread.start();
    connect(this, &AudioController::triggerPlaySound, this, &AudioController::onPlaySound);
    // BGM结束信号：绑定到切换逻辑（仅绑定1次，避免重复）
    connect(bgmPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            onBgmEnded();  // 抽离的切换逻辑
        }
    });
}

// 3. 析构函数：补充 AudioOutput 释放（避免内存泄漏）
AudioController::~AudioController() {
    // 1. 先停止所有播放，强制释放底层资源
    bgmPlayer->stop();
    bgmPlayer->setSource(QUrl());  // 清除媒体源，触发底层接口释放
    soundEffect->stop();

    // 2. 断开所有信号连接（防止析构时触发槽函数）
    bgmPlayer->disconnect();
    soundEffect->disconnect();
    this->disconnect();

    // 3. 按“依赖倒序”销毁：先销毁依赖者（bgmPlayer），再销毁被依赖者（bgmAudioOutput）
    delete bgmPlayer;
    bgmPlayer = nullptr;  // 必须先销毁Player

    delete bgmAudioOutput;
    bgmAudioOutput = nullptr;  // 再销毁AudioOutput

    // 4. 销毁音效对象（确保在主线程销毁）
    delete soundEffect;
    soundEffect = nullptr;

    // 5. 若使用了音效线程，确保线程已退出（彻底移除跨线程逻辑）
    if (soundThread.isRunning()) {
        soundThread.quit();
        soundThread.wait(500);  // 等待500ms确保线程退出
    }
}

// 4. 简化后的BGM播放逻辑（核心）
void AudioController::playBackgroundMusic() {
    // 边界检查：BGM列表为空则返回
    if (bgmFiles.isEmpty()|| !settings.musicEnabled) {
        qDebug() << "[Audio] BGM列表为空或已禁用";
        return;
    }
    // 设置当前BGM源并播放（1行代码完成核心操作）
    bgmPlayer->setSource(QUrl(bgmFiles[currentBGMIndex]));
    bgmPlayer->play();
    qDebug() << "[Audio] 播放BGM：" << bgmFiles[currentBGMIndex];
}

// 5. 简化后的BGM停止逻辑
void AudioController::stopBackgroundMusic() {
    bgmPlayer->stop();
    currentBGMIndex = 0;  // 停止后重置索引（下次播放从第一首开始）
    qDebug() << "[Audio] 停止BGM";
}

// 6. 新增：BGM结束自动切换（抽离逻辑，代码更清晰）
void AudioController::onBgmEnded() {
    // 循环切换索引（精简计算）
    currentBGMIndex = (currentBGMIndex + 1) % bgmFiles.size();
    // 直接调用播放方法，复用逻辑
    playBackgroundMusic();
}

// 7. 新增：对外提供BGM音量调整接口（供设置页面调用）
void AudioController::setBgmVolume(int volume) {
    // 范围限制：确保音量在0-100之间
    volume = qBound(0, volume, 100);
    // 转换为 Qt 6 要求的 0.0-1.0 浮点值
    bgmAudioOutput->setVolume(volume / 100.0f);
    // 同步更新配置（确保重启后生效）
    settings.musicVolume = volume;
    qDebug() << "[Audio] BGM音量更新为：" << volume;
}
void AudioController::setSoundVolume(int volume) {
    // 范围限制
    volume = qBound(0, volume, 100);
    // 更新音效音量
    soundEffect->setVolume(volume / 100.0f);
    // 同步更新配置
    settings.soundVolume = volume;
    qDebug() << "[Audio] 音效音量更新为：" << volume;
}

// 8. 音效播放逻辑（保留原逻辑，无修改）
void AudioController::playSound(SoundType type) {
    if (!settings.soundEnabled || !soundPaths.contains(type)) {
        qDebug() << "[Audio] 音效禁用或类型无效";
        return;
    }
    emit triggerPlaySound(type);
}

void AudioController::onPlaySound(SoundType type) {
    soundEffect->stop();
    const QString &path = soundPaths[type];
    soundEffect->setSource(QUrl(path));
    soundEffect->setVolume(settings.soundVolume / 100.0f);
    soundEffect->play();
    qDebug() << "[Audio] 播放音效：" << path;
}
