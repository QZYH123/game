/**
 * @file LevelManager.h
 * @brief 关卡管理器类声明，负责统一管理所有关卡
 * @author 开发团队
 * @date 2025-10-22
 * @version 1.0.0
 */

#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "LevelData.h"
#include <QObject>
#include <QVector>
#include <QString>
#include <QDir>
#include <QStandardPaths>

/**
 * @class LevelManager
 * @brief 关卡管理器，使用单例模式管理所有关卡数据
 * 
 * 负责：
 * - 加载和保存关卡文件
 * - 管理关卡进度和解锁状态
 * - 提供关卡数据访问接口
 * - 创建默认教学关卡
 */
class LevelManager : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief 获取单例实例
     * @return LevelManager& 单例引用
     */
    static LevelManager& getInstance();
    
    /**
     * @brief 初始化关卡管理器
     * @return bool 是否初始化成功
     */
    bool initialize();
    
    // === 关卡数据访问 ===
    
    /**
     * @brief 获取关卡数量
     * @return int 关卡总数
     */
    int getLevelCount() const { return level_data_list.size(); }
    
    /**
     * @brief 获取指定关卡数据
     * @param levelIndex 关卡索引（从0开始）
     * @return LevelData* 关卡数据指针，失败返回nullptr
     */
    LevelData* getLevelData(int levelIndex);
    
    /**
     * @brief 获取当前关卡数据
     * @return LevelData* 当前关卡数据指针
     */
    LevelData* getCurrentLevelData() { return getLevelData(current_level_index); }
    
    /**
     * @brief 设置当前关卡
     * @param levelIndex 关卡索引
     * @return bool 是否设置成功
     */
    bool setCurrentLevel(int levelIndex);
    
    /**
     * @brief 获取当前关卡索引
     * @return int 当前关卡索引
     */
    int getCurrentLevelIndex() const { return current_level_index; }
    
    // === 关卡进度管理 ===
    
    /**
     * @brief 检查关卡是否已解锁
     * @param levelIndex 关卡索引
     * @return bool 是否已解锁
     */
    bool isLevelUnlocked(int levelIndex) const;
    
    /**
     * @brief 解锁关卡
     * @param levelIndex 关卡索引
     */
    void unlockLevel(int levelIndex);
    
    /**
     * @brief 标记关卡为已完成
     * @param levelIndex 关卡索引
     */
    void markLevelCompleted(int levelIndex);
    
    /**
     * @brief 检查关卡是否已完成
     * @param levelIndex 关卡索引
     * @return bool 是否已完成
     */
    bool isLevelCompleted(int levelIndex) const;
    
    // === 文件操作 ===
    
    /**
     * @brief 加载所有关卡文件
     * @return bool 是否加载成功
     */
    bool loadAllLevels();
    
    /**
     * @brief 从指定文件加载关卡
     * @param filePath 关卡文件路径
     * @return LevelData* 加载的关卡数据，失败返回nullptr
     */
    LevelData* loadLevelFromFile(const QString& filePath);
    
    /**
     * @brief 保存关卡数据
     * @param levelIndex 关卡索引
     * @return bool 是否保存成功
     */
    bool saveLevelData(int levelIndex);
    
    /**
     * @brief 保存所有关卡数据
     * @return bool 是否保存成功
     */
    bool saveAllLevels();
    
    /**
     * @brief 保存游戏进度
     * @return bool 是否保存成功
     */
    bool saveProgress();
    
    /**
     * @brief 加载游戏进度
     * @return bool 是否加载成功
     */
    bool loadProgress();
    
    // === 关卡创建和编辑 ===
    
    /**
     * @brief 创建新关卡
     * @param levelName 关卡名称
     * @param description 关卡描述
     * @return int 新关卡的索引，失败返回-1
     */
    int createNewLevel(const QString& levelName, const QString& description = "");
    
    /**
     * @brief 删除关卡
     * @param levelIndex 关卡索引
     * @return bool 是否删除成功
     */
    bool deleteLevel(int levelIndex);
    
    /**
     * @brief 复制关卡
     * @param sourceIndex 源关卡索引
     * @param newName 新关卡名称
     * @return int 新关卡索引，失败返回-1
     */
    int duplicateLevel(int sourceIndex, const QString& newName);
    
    // === 默认关卡创建 ===
    
    /**
     * @brief 创建默认教学关卡
     */
    void createDefaultTutorialLevel();
    
    /**
     * @brief 创建默认关卡集合
     */
    void createDefaultLevels();
    
    // === 路径管理 ===
    
    /**
     * @brief 获取关卡文件目录
     * @return QString 关卡目录路径
     */
    QString getLevelsDirectory() const { return levels_directory; }
    
    /**
     * @brief 获取关卡文件路径
     * @param levelIndex 关卡索引
     * @return QString 关卡文件路径
     */
    QString getLevelFilePath(int levelIndex) const;

signals:
    /**
     * @brief 关卡数据改变信号
     * @param levelIndex 改变的关卡索引
     */
    void levelDataChanged(int levelIndex);
    
    /**
     * @brief 关卡解锁状态改变信号
     * @param levelIndex 关卡索引
     * @param unlocked 是否解锁
     */
    void levelUnlockStatusChanged(int levelIndex, bool unlocked);
    
    /**
     * @brief 当前关卡改变信号
     * @param newLevelIndex 新的关卡索引
     */
    void currentLevelChanged(int newLevelIndex);

private:
    /**
     * @brief 私有构造函数（单例模式）
     */
    explicit LevelManager(QObject* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LevelManager() = default;
    
    // 禁用拷贝构造和赋值操作
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;
    
    QVector<LevelData*> level_data_list;        ///< 关卡数据列表
    QVector<bool> level_unlocked_status;        ///< 关卡解锁状态
    QVector<bool> level_completed_status;       ///< 关卡完成状态
    
    int current_level_index;                    ///< 当前关卡索引
    QString levels_directory;                   ///< 关卡文件目录
    QString progress_file_path;                 ///< 进度文件路径
    
    /**
     * @brief 初始化目录结构
     * @return bool 是否初始化成功
     */
    bool initializeDirectories();
    
    /**
     * @brief 确保关卡索引有效
     * @param levelIndex 关卡索引
     * @return bool 是否有效
     */
    bool isValidLevelIndex(int levelIndex) const;
    
    /**
     * @brief 生成关卡文件名
     * @param levelIndex 关卡索引
     * @return QString 文件名
     */
    QString generateLevelFileName(int levelIndex) const;
};

#endif // LEVELMANAGER_H
