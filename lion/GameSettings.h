/**
 * @file GameSettings.h
 * @brief 游戏设置结构体和配置管理
 * @author 开发团队
 * @date 2025-10-16
 * @version 0.1.0
 */

#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include "Config.h"

/**
 * @class GameSettings
 * @brief 游戏设置类，存储所有可配置项，使用单例模式
 */
class GameSettings {
public:
    /**
     * @brief 获取单例实例
     * @return GameSettings的单例实例引用
     */
    static GameSettings& getInstance() {
        static GameSettings instance;
        return instance;
    }
    
    // 音频设置
    bool musicEnabled;       ///< 背景音乐开关
    int musicVolume;         ///< 背景音乐音量 (0-100)
    bool soundEnabled;       ///< 音效开关
    int soundVolume;         ///< 音效音量 (0-100)
    
    // 显示设置
    bool fullscreen;         ///< 全屏模式
    int resolution;          ///< 分辨率选项 (0:1280x720, 1:1920x1080, 2:2560x1440)
    
    // 游戏设置
    int difficulty;          ///< 游戏难度 (0:简单, 1:普通, 2:困难)
    bool showTutorial;       ///< 是否显示教程
    
    // 控制设置
    bool invertYAxis;        ///< 是否反转Y轴
    int sensitivity;         ///< 控制灵敏度 (1-10)
    
    /**
     * @brief 保存设置到文件
     * @return 是否保存成功
     */
    bool saveToFile() const {
        QString filePath = getDataDirectory() + "/settings.cfg";
        QFile file(filePath);
        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "无法打开设置文件进行写入: " << filePath;
            return false;
        }
        
        QTextStream out(&file);
        out << "# 醒狮跃境游戏设置文件\n";
        out << "musicEnabled=" << (musicEnabled ? "true" : "false") << "\n";
        out << "musicVolume=" << musicVolume << "\n";
        out << "soundEnabled=" << (soundEnabled ? "true" : "false") << "\n";
        out << "soundVolume=" << soundVolume << "\n";
        out << "fullscreen=" << (fullscreen ? "true" : "false") << "\n";
        out << "resolution=" << resolution << "\n";
        out << "difficulty=" << difficulty << "\n";
        out << "showTutorial=" << (showTutorial ? "true" : "false") << "\n";
        out << "invertYAxis=" << (invertYAxis ? "true" : "false") << "\n";
        out << "sensitivity=" << sensitivity << "\n";
        
        file.close();
        return true;
    }
    
    /**
     * @brief 从文件加载设置
     * @return 是否加载成功
     */
    bool loadFromFile() {
        QString filePath = getDataDirectory() + "/settings.cfg";
        QFile file(filePath);
        
        if (!file.exists()) {
            qDebug() << "设置文件不存在，将使用默认设置";
            return false;
        }
        
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开设置文件进行读取: " << filePath;
            return false;
        }
        
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            
            // 跳过注释和空行
            if (line.startsWith('#') || line.isEmpty()) {
                continue;
            }
            
            QStringList parts = line.split('=');
            if (parts.size() != 2) {
                continue;
            }
            
            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();
            
            if (key == "musicEnabled") {
                musicEnabled = (value == "true");
            } else if (key == "musicVolume") {
                musicVolume = value.toInt();
            } else if (key == "soundEnabled") {
                soundEnabled = (value == "true");
            } else if (key == "soundVolume") {
                soundVolume = value.toInt();
            } else if (key == "fullscreen") {
                fullscreen = (value == "true");
            } else if (key == "resolution") {
                resolution = value.toInt();
            } else if (key == "difficulty") {
                difficulty = value.toInt();
            } else if (key == "showTutorial") {
                showTutorial = (value == "true");
            } else if (key == "invertYAxis") {
                invertYAxis = (value == "true");
            } else if (key == "sensitivity") {
                sensitivity = value.toInt();
            }
        }
        
        file.close();
        return true;
    }

private:
    /**
     * @brief 私有构造函数，防止外部创建实例
     */
    GameSettings() {
        // 设置默认值
        musicEnabled = true;
        musicVolume = 80;
        soundEnabled = true;
        soundVolume = 80;
        
        fullscreen = false;
        resolution = 0;  // 默认1280x720
        
        difficulty = 1;  // 默认普通难度
        showTutorial = true;
        
        invertYAxis = false;
        sensitivity = 5;
        
        // 尝试从文件加载设置
        loadFromFile();
    }
    
    /**
     * @brief 删除拷贝构造函数
     */
    GameSettings(const GameSettings&) = delete;
    
    /**
     * @brief 删除赋值运算符
     */
    GameSettings& operator=(const GameSettings&) = delete;
};

#endif // GAMESETTINGS_H