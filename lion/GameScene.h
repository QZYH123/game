#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include "qtimer.h"
#include "QKeyEvent"
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
    
    // 计时与状态
    QElapsedTimer level_timer;              ///< 关卡计时器
    bool is_dead = false;                   ///< 玩家死亡状态
    bool is_in_water = false;               ///< 玩家在水中（减速）
    int water_slow_counter = 0;             ///< 水减速计数
    int tick_counter = 0;                   ///< 场景tick计数
    
    // 箭矢投射物
    struct Projectile { QPointF pos; QPointF vel; QPointF size; bool active; };
    QVector<Projectile> projectiles;
    void init();
    void mapInit();
    void gameStart();
    void gamewin();
    void gameover();
    void updatePosition();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    
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
     * @brief 解锁关卡
     * @param levelIndex 要解锁的关卡索引
     */
    void unlockLevel(int levelIndex);
};

#endif
