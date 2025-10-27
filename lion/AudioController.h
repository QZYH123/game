#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H
#include <QObject>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>  // Qt 6 音量控制核心类
#include <QThread>
#include <QUrl>
#include <QMap>
class GameSettings;

enum class SoundType {
    Jump, Click, Float, Win, Collect
};
Q_DECLARE_METATYPE(SoundType)

class AudioController : public QObject
{
    Q_OBJECT
public:
    static AudioController& getInstance(QObject *parent = nullptr);
    ~AudioController() override;

    void playBackgroundMusic();  // 简化后的BGM播放
    void stopBackgroundMusic();  // 简化后的BGM停止
    void playSound(SoundType type);

    // 新增：对外提供音量调整接口（供设置页面调用）
    void setBgmVolume(int volume);  // volume: 0-100
    void setSoundVolume(int volume);  // 新增：设置音效音量

private:
    explicit AudioController(QObject *parent = nullptr);
    AudioController(const AudioController&) = delete;
    AudioController& operator=(const AudioController&) = delete;

    // 核心成员：Qt 6 音频组合（QMediaPlayer + QAudioOutput）
    QMediaPlayer *bgmPlayer;
    QAudioOutput *bgmAudioOutput;  // 负责BGM音量控制
    QStringList bgmFiles;          // BGM路径列表（保留精简版）
    int currentBGMIndex = 0;       // 当前BGM索引（默认初始值）

    // 音效相关（保留原逻辑，不受BGM简化影响）
    QThread soundThread;
    QSoundEffect *soundEffect;
    GameSettings &settings;
    QMap<SoundType, QString> soundPaths;

private slots:
    void onPlaySound(SoundType type);
    // 新增：BGM播放结束后自动切换（单独抽离，避免重复写逻辑）
    void onBgmEnded();
signals:
    void triggerPlaySound(SoundType type);
};
#endif // AUDIOCONTROLLER_H
