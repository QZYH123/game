/**
 * @file LevelData.h
 * @brief 关卡数据结构定义，支持可复用的关卡系统
 * @author 开发团队
 * @date 2025-10-22
 * @version 1.0.0
 */

#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <QVector>
#include <QPointF>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include "Config.h"

/**
 * @enum GameElementType
 * @brief 游戏元素类型枚举
 */
enum class GameElementType {
    Empty = 0,              ///< 空白区域
    SolidBlock = 1,         ///< 实心方块（地面、墙壁）
    Vegetable = 2,          ///< 青菜（收集目标）
    Spike = 3,              ///< 尖刺（危险元素）
    MovingPlatform = 4,     ///< 移动平台（已废弃，保留兼容性）
    Checkpoint = 5,         ///< 检查点
    PlayerStart = 6,        ///< 玩家起始位置
    LevelExit = 7,          ///< 关卡出口
    ArrowTrap = 8,          ///< 箭机关（发射箭矢）
    Water = 9,              ///< 水（减速）
    Lava = 10,              ///< 岩浆（死亡）
    HorizontalPlatform = 11,///< 水平移动平台
    VerticalPlatform = 12,  ///< 垂直移动平台
    Switch = 13,            ///< 开关
    Door = 14               ///< 门
};

/**
 * @enum ArrowDirection
 * @brief 箭机关发射方向
 */
enum class ArrowDirection {
    Right = 0,  ///< 向右
    Left = 1,   ///< 向左
    Up = 2,     ///< 向上
    Down = 3    ///< 向下
};

/**
 * @struct GameElement
 * @brief 游戏元素结构体
 */
struct GameElement {
    GameElementType element_type;   ///< 元素类型
    QPointF position;              ///< 位置坐标
    QPointF size;                  ///< 元素大小
    QString texture_path;          ///< 纹理路径
    QJsonObject properties;        ///< 额外属性（如移动速度、伤害值等）
    
    /**
     * @brief 默认构造函数
     */
    GameElement() : element_type(GameElementType::Empty), position(0, 0), size(32, 32) {}
    
    /**
     * @brief 构造函数
     * @param type 元素类型
     * @param pos 位置
     * @param sz 大小
     */
    GameElement(GameElementType type, const QPointF& pos, const QPointF& sz = QPointF(32, 32))
        : element_type(type), position(pos), size(sz) {}
};

/**
 * @struct LevelObjective
 * @brief 关卡目标结构体
 */
struct LevelObjective {
    QString objective_type;         ///< 目标类型（如"collect_vegetables", "reach_exit"）
    int target_count;              ///< 目标数量
    int current_count;             ///< 当前完成数量
    QString description;           ///< 目标描述
    
    /**
     * @brief 默认构造函数
     */
    LevelObjective() : target_count(0), current_count(0) {}
    
    /**
     * @brief 检查目标是否完成
     * @return bool 是否完成
     */
    bool isCompleted() const { return current_count >= target_count; }
};

class LevelData
{
public:
    /**
     * @brief 构造函数
     * @param width 关卡宽度（格子数）
     * @param height 关卡高度（格子数）
     */
    explicit LevelData(int width = GRID_WIDTH, int height = GRID_HEIGHT);
    
    /**
     * @brief 析构函数
     */
    ~LevelData() = default;

    bool isCustomLevel() const { return is_custom_level; }
    QString getFilePath() const { return file_path; }
    void setCustomLevel(bool is_custom, const QString& path = "") {
        is_custom_level = is_custom;
        file_path = path;
    }
    
    // === 基本属性访问 ===
    
    /**
     * @brief 获取关卡宽度
     * @return int 宽度（格子数）
     */
    int getWidth() const { return level_width; }
    
    /**
     * @brief 获取关卡高度
     * @return int 高度（格子数）
     */
    int getHeight() const { return level_height; }
    
    /**
     * @brief 获取关卡名称
     * @return QString 关卡名称
     */
    QString getLevelName() const { return level_name; }
    
    /**
     * @brief 设置关卡名称
     * @param name 关卡名称
     */
    void setLevelName(const QString& name) { level_name = name; }
    
    /**
     * @brief 获取关卡描述
     * @return QString 关卡描述
     */
    QString getLevelDescription() const { return level_description; }
    
    /**
     * @brief 设置关卡描述
     * @param description 关卡描述
     */
    void setLevelDescription(const QString& description) { level_description = description; }
    
    // === 地图数据操作 ===
    
    /**
     * @brief 获取指定位置的元素类型
     * @param x X坐标
     * @param y Y坐标
     * @return GameElementType 元素类型
     */
    GameElementType getElementAt(int x, int y) const;
    
    /**
     * @brief 设置指定位置的元素类型
     * @param x X坐标
     * @param y Y坐标
     * @param type 元素类型
     */
    void setElementAt(int x, int y, GameElementType type);
    
    /**
     * @brief 添加游戏元素
     * @param element 游戏元素
     */
    void addGameElement(const GameElement& element);
    
    /**
     * @brief 获取所有游戏元素
     * @return QVector<GameElement> 游戏元素列表
     */
    const QVector<GameElement>& getGameElements() const { return game_elements; }
    
    /**
     * @brief 清空所有游戏元素
     */
    void clearGameElements() { game_elements.clear(); }
    
    // 从指定格子删除所有游戏元素，并将网格置空
    void removeElementsAt(int grid_x, int grid_y);
    
    // === 关卡目标管理 ===
    
    /**
     * @brief 添加关卡目标
     * @param objective 关卡目标
     */
    void addObjective(const LevelObjective& objective);
    
    /**
     * @brief 获取所有关卡目标
     * @return QVector<LevelObjective> 目标列表
     */
    const QVector<LevelObjective>& getObjectives() const { return level_objectives; }
    
    /**
     * @brief 更新目标进度
     * @param objectiveType 目标类型
     * @param increment 增加的数量
     */
    void updateObjectiveProgress(const QString& objectiveType, int increment = 1);
    
    /**
     * @brief 检查是否所有目标都已完成
     * @return bool 是否全部完成
     */
    bool areAllObjectivesCompleted() const;
    
    /**
     * @brief 重置所有目标的进度
     */
    void resetObjectiveProgress();
    
    // === 玩家起始位置 ===
    
    /**
     * @brief 获取玩家起始位置
     * @return QPointF 起始位置
     */
    QPointF getPlayerStartPosition() const { return player_start_position; }
    
    /**
     * @brief 设置玩家起始位置
     * @param position 起始位置
     */
    void setPlayerStartPosition(const QPointF& position) { player_start_position = position; }
    
    // === 文件操作 ===
    
    /**
     * @brief 从JSON文件加载关卡数据
     * @param filePath 文件路径
     * @return bool 是否加载成功
     */
    bool loadFromFile(const QString& filePath);
    
    /**
     * @brief 保存关卡数据到JSON文件
     * @param filePath 文件路径
     * @return bool 是否保存成功
     */
    bool saveToFile(const QString& filePath) const;
    
    /**
     * @brief 从JSON对象加载数据
     * @param jsonObj JSON对象
     * @return bool 是否加载成功
     */
    bool loadFromJson(const QJsonObject& jsonObj);
    
    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const;
    
    // === 兼容性接口（与现有代码兼容） ===
    
    /**
     * @brief 获取传统地图数组（兼容现有代码）
     * @param mapArray 输出的地图数组[24][24]
     */
    void getLegacyMapArray(int mapArray[24][24]) const;
    
    /**
     * @brief 从传统地图数组设置数据（兼容现有代码）
     * @param mapArray 输入的地图数组[24][24]
     */
    void setFromLegacyMapArray(const int mapArray[24][24]);
    
    /**
     * @brief 获取动态大小的地图数组
     * @param mapArray 输出的地图数组指针
     * @param width 数组宽度
     * @param height 数组高度
     */
    void getMapArray(int** mapArray, int width, int height) const;

private:
    int level_width;                        ///< 关卡宽度
    int level_height;                       ///< 关卡高度
    QString level_name;                     ///< 关卡名称
    QString level_description;              ///< 关卡描述
    
    QVector<QVector<GameElementType>> level_grid;  ///< 关卡网格数据
    QVector<GameElement> game_elements;            ///< 游戏元素列表
    QVector<LevelObjective> level_objectives;      ///< 关卡目标列表
    
    QPointF player_start_position;          ///< 玩家起始位置

    bool is_custom_level;                   ///< 是否为自定义关卡
    QString file_path;                      ///< 关卡文件路径（仅自定义关卡）
    
    /**
     * @brief 初始化关卡网格
     */
    void initializeGrid();
    
    /**
     * @brief 验证坐标是否有效
     * @param x X坐标
     * @param y Y坐标
     * @return bool 是否有效
     */
    bool isValidCoordinate(int x, int y) const;
};

#endif // LEVELDATA_H
