#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include "qtimer.h"
#include "QKeyEvent"
#include <QResizeEvent>
#include <QLabel>
#include <QFont>
#include <QPushButton>
#include "player.h"
#include "qdebug.h"
#include "qpainter.h"
#include <QString>
#include <QVector>
#include <QPointF>
#include "Config.h"
#include "LevelData.h"
#include "LevelManager.h"
#include <QElapsedTimer>
#include <QJsonObject>
class BackGround
{
public:
    BackGround();
    BackGround(int num);
    void mappositionr();
    void mappositionl();

    QPixmap map1;
    QPixmap map2;
    QPixmap map3;
    int map1_x;
    int map2_x;
    int map3_x;
    int m_scroll_speed;
};

namespace Ui {
class GameScene;
}

class GameScene : public QWidget
{
    Q_OBJECT

public:
    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene();
private:
    // +++ 新增：残影结构体
    struct Afterimage {
        QPixmap frame;      // 捕捉的帧
        QRectF rect;        // 当时的位置
        qint64 spawnTime;   // 生成时间
    };

    QList<Afterimage> afterimages; // 存储所有残影的列表

    // +++ 新增：残影常量
    const int AFTERIMAGE_LIFETIME = 300; // 残影持续时间 (ms)
    const int AFTERIMAGE_INTERVAL = 50;  // 残影生成间隔 (ms)
    qint64 lastAfterimageTime;           // 上次生成残影的时间

    // +++ 新增：更新残影的私有方法
    void updateAfterimages();
public:
    QTimer Timer;
    BackGround background;
    player pl;
    QPixmap block5;
    QFont font;
    QLabel *label1,*label2;
    QLabel *labelblood1,*labelblood2;
    QLabel *labeltime1,*labeltime2;
    QString print,printblood,printtime;
    int updatenum=0;
    bool leftpress=0;
    double time=0.000001;
    bool rightpress=0;
    bool begin=false;
    
    // === 新增：关卡系统相关 ===
    LevelData* current_level_data;          ///< 当前关卡数据
    QVector<GameElement> collected_items;   ///< 已收集的物品
    QLabel* objective_label;                ///< 目标显示标签
    QLabel* tutorial_label;                 ///< 教学提示标签
    QPixmap vegetable_texture;              ///< 青菜纹理
    QPixmap exit_texture;                   ///< 出口纹理
    QPixmap water_texture;                  ///< 水纹理（可为空使用颜色）
    QPixmap lava_texture;                   ///< 岩浆纹理（可为空使用颜色）
    QPixmap arrow_texture;                  ///< 箭矢纹理（可为空使用颜色）
    QPixmap arrow_trap_right_texture;       ///< 向右箭机关纹理
    QPixmap arrow_trap_left_texture;        ///< 向左箭机关纹理
    QPixmap arrow_trap_up_texture;          ///< 向上箭机关纹理
    QPixmap arrow_trap_down_texture;        ///< 向下箭机关纹理
    QPushButton* back_button;               ///< 返回按钮
    
    // === 移动平台和开关门纹理 ===
    QPixmap horizontal_platform_texture;    ///< 水平移动平台纹理
    QPixmap vertical_platform_texture;      ///< 垂直移动平台纹理
    QPixmap switch_texture;                 ///< 开关纹理
    QPixmap door_texture;                   ///< 门纹理
    
    // 计时与状态
    QElapsedTimer level_timer;              ///< 关卡计时器
    bool is_dead = false;                   ///< 玩家死亡状态
    bool is_in_water = false;               ///< 玩家在水中（减速）
    int water_slow_counter = 0;             ///< 水减速计数
    int tick_counter = 0;                   ///< 场景tick计数
    
    // === 暂停功能相关 ===
    bool is_paused = false;                 ///< 游戏是否暂停
    QWidget* pause_menu = nullptr;          ///< 暂停菜单
    QPushButton* resume_button = nullptr;   ///< 继续游戏按钮
    QPushButton* restart_button = nullptr;  ///< 重新开始按钮
    QPushButton* main_menu_button = nullptr; ///< 返回主菜单按钮
    
    // 箭矢投射物
    struct Projectile { QPointF pos; QPointF vel; QPointF size; bool active; };
    QVector<Projectile> projectiles;
    
    // === 移动平台状态 ===
    struct MovingPlatformState {
        QPointF current_pos;        ///< 当前位置
        QPointF start_pos;          ///< 起始位置
        QPointF end_pos;            ///< 结束位置
        QPointF velocity;           ///< 移动速度
        bool moving_to_end;         ///< 是否正在向终点移动
        int element_index;          ///< 对应的游戏元素索引
    };
    QVector<MovingPlatformState> moving_platforms;
    
    // === 开关门状态 ===
    struct SwitchDoorState {
        bool is_activated = false;          ///< 开关是否被激活
        int switch_element_index = -1;   ///< 开关元素索引
        int door_element_index = -1;     ///< 门元素索引
        bool door_is_open = false;          ///< 门是否打开
        
        bool operator==(const SwitchDoorState& other) const {
            return switch_element_index == other.switch_element_index &&
                   door_element_index == other.door_element_index;
        }
    };
    QVector<SwitchDoorState> switch_doors;
    void init();
    void mapInit();
    void gameStart();
    void gamewin();
    void gameover();
    void updatePosition();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    
    /**
     * @brief 加载指定关卡（公共接口）
     * @param levelIndex 关卡索引
     * @return bool 是否加载成功
     */
    bool loadLevel(int levelIndex);
    
    /**
     * @brief 从文件加载关卡（公共接口）
     * @param filePath 关卡文件路径
     * @return bool 是否加载成功
     */
    bool loadLevelFromFile(const QString& filePath);
    
    /**
     * @brief 重置关卡状态
     */
    void resetLevel();
    void restartLevel();

signals:
    /**
     * @brief 返回主菜单信号
     */
    void backToMainMenu();
    
    /**
     * @brief 返回关卡选择界面信号
     */
    void backToLevelSelect();
    
    /**
     * @brief 游戏结束信号（胜利或失败）
     */
    void gameFinished();

private:
    // === 新增：关卡系统方法 ===
    
    /**
     * @brief 加载指定关卡（内部实现）
     * @param levelIndex 关卡索引
     * @return bool 是否加载成功
     */
    bool loadLevelInternal(int levelIndex);
    
    /**
     * @brief 检查玩家与游戏元素的碰撞
     */
    void checkGameElementCollisions();
    
    /**
     * @brief 收集游戏物品
     * @param element 游戏元素
     */
    void collectItem(const GameElement& element);
    
    /**
     * @brief 更新目标显示
     */
    void updateObjectiveDisplay();
    
    /**
     * @brief 更新教学提示
     */
    void updateTutorialHints();
    
    /**
     * @brief 检查关卡完成条件
     * @return bool 是否完成
     */
    bool checkLevelCompletion();
    
    /**
     * @brief 绘制游戏元素
     * @param painter 绘制器
     */
    void drawGameElements(QPainter& painter);
    
    
    
    /**
     * @brief 显示游戏提示信息
     * @param message 提示信息内容
     * @param duration 显示持续时间（毫秒）
     */
    void showGameMessage(const QString& message, int duration = 3000);
    
    /**
     * @brief 保存游戏进度
     * @param levelIndex 当前关卡索引
     */
    void saveGameProgress(int levelIndex);
    
    /**
     * @brief 加载游戏进度
     * @return QJsonObject 进度数据
     */
    QJsonObject loadGameProgress();
    
    /**
     * @brief 检查关卡是否已解锁
     * @param levelIndex 关卡索引
     * @return bool 是否已解锁
     */
    bool isLevelUnlocked(int levelIndex);
    
    /**
     * @brief 解锁指定关卡
     * @param levelIndex 关卡索引
     */
    void unlockLevel(int levelIndex);
    
    // === 暂停功能方法 ===
    
    /**
     * @brief 暂停游戏
     */
    void pauseGame();
    
    /**
     * @brief 恢复游戏
     */
    void resumeGame();
    
    /**
     * @brief 创建暂停菜单
     */
    void createPauseMenu();
    
    /**
     * @brief 显示暂停菜单
     */
    void showPauseMenu();
    
    /**
     * @brief 隐藏暂停菜单
     */
    void hidePauseMenu();
    
    // === 移动平台和开关门方法 ===
    
    /**
     * @brief 初始化移动平台状态
     */
    void initializeMovingPlatforms();
    
    /**
     * @brief 更新移动平台位置
     */
    void updateMovingPlatforms();
    
    /**
     * @brief 初始化开关门状态
     */
    void initializeSwitchDoors();
    
    /**
     * @brief 检查玩家与开关的碰撞
     */
    void checkSwitchCollisions();
    
    /**
     * @brief 检查玩家与移动平台的碰撞
     */
    void checkMovingPlatformCollisions();
    
    /**
     * @brief 处理玩家跟随移动平台
     */
    void handlePlatformFollowing();
    
    /**
     * @brief 激活开关
     * @param switch_index 开关索引
     */
    void activateSwitch(int switch_index);
    
    /**
     * @brief 检查门的碰撞（关闭的门阻挡玩家）
     * @param playerRect 玩家矩形
     * @return bool 是否与关闭的门发生碰撞
     */
    bool checkDoorCollision(const QRectF& playerRect);
};

#endif
