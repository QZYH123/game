# 《醒狮跃境》命名与注释规范 - 简化版

## 1. 命名规范

### 1.1 基本原则

- **见名知意**：名称应清晰表达其用途和含义
- **保持一致**：同类型的命名风格在整个项目中保持一致
- **避免缩写**：除非是广泛认知的缩写（如UI、ID、URL等）

### 1.2 具体命名规则

#### 1.2.1 类名 - 大驼峰命名法（PascalCase）

```cpp
class GameEngine;           // 游戏引擎
class InputManager;         // 输入管理器
class PhysicsComponent;     // 物理组件
class PlayerController;     // 玩家控制器
class LevelData;           // 关卡数据
class AudioManager;        // 音频管理器

// 枚举类
enum class ExpressionType {
    Awake,
    Sleeping,
    Confused,
    Exploring
};

// 结构体
struct GameSettings;
struct CollisionInfo;
```

#### 1.2.2 函数名 - 小驼峰命名法（camelCase）

```cpp
// 动作函数
void initializeGame();      // 初始化游戏
void updatePhysics();       // 更新物理
void renderScene();         // 渲染场景
bool checkCollision();      // 检查碰撞

// 获取/设置函数
int getPlayerScore();       // 获取玩家分数
void setPlayerScore(int score);  // 设置玩家分数
bool isGameRunning();       // 是否游戏运行中
bool hasCollision();        // 是否有碰撞

// 事件处理函数
void handleKeyPress(QKeyEvent* event);  // 处理按键
void onPlayerJump();                    // 玩家跳跃事件
void processInput();                    // 处理输入
```

#### 1.2.3 成员变量 - 蛇形命名法（snake_case）

```cpp
class Player {
private:
    int player_health;          // 玩家生命值
    float movement_speed;       // 移动速度
    QPointF current_position;   // 当前位置
    QString player_name;        // 玩家名称
    bool is_grounded;          // 是否在地面
    QTimer* update_timer;      // 更新定时器
    
    // 静态成员变量
    static int total_player_count;     // 玩家总数
    static const float max_speed;      // 最大速度
};
```

#### 1.2.4 局部变量 - 小驼峰命名法（camelCase）

```cpp
void updatePlayer() {
    float deltaTime = 0.016f;           // 时间间隔
    QPointF currentPosition;            // 当前位置
    QPointF targetPosition;             // 目标位置
    bool isGrounded = false;            // 是否在地面
    int playerHealth = 100;             // 玩家生命值
    
    // 循环变量
    for (int i = 0; i < playerCount; ++i) {
        // 复杂循环使用描述性名称
        for (int levelIndex = 0; levelIndex < maxLevels; ++levelIndex) {
            // ...
        }
    }
}
```

#### 1.2.5 常量 - 全大写+下划线

```cpp
const int MAX_PLAYERS = 2;
const float GRAVITY_FORCE = 9.8f;
const QString DEFAULT_PLAYER_NAME = "Player";
const float JUMP_VELOCITY = 500.0f;

// 宏定义
#define GAME_VERSION "1.0.0"
#define DEBUG_MODE 1
```

#### 1.2.6 文件命名

```cpp
// 头文件：与类名保持一致
GameEngine.h
InputManager.h
PhysicsComponent.h

// 源文件：与头文件同名
GameEngine.cpp
InputManager.cpp
PhysicsComponent.cpp

// 特殊文件
main.cpp                    // 程序入口
GameConstants.h             // 常量定义
GameTypes.h                 // 类型定义
```

---

## 2. 注释规范

### 2.1 注释基本原则

- **解释为什么，而不是做什么**：代码本身说明做什么，注释说明为什么
- **保持同步**：代码修改时必须同步更新注释
- **简洁明了**：注释应该简洁但完整
- **使用中文**：项目内部使用中文注释，便于团队理解

### 2.2 文件头注释

```cpp
/**
 * @file GameEngine.h
 * @brief 游戏引擎核心类定义
 * @author 张三
 * @date 2024-12-15
 * @version 1.0.0
 */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H
```

### 2.3 类注释

```cpp
/**
 * @class GameEngine
 * @brief 游戏引擎核心管理类
 * 
 * 负责游戏的整体运行流程，包括：
 * - 游戏循环的管理和控制
 * - 各个子系统的初始化和协调
 * - 全局资源的管理和分配
 * 
 * 使用示例：
 * @code
 * GameEngine engine;
 * if (engine.initialize()) {
 *     engine.run();
 * }
 * @endcode
 * 
 * @note 这是一个单例类，全局只能有一个实例
 * @warning 必须在主线程中使用
 */
class GameEngine : public QObject
{
    Q_OBJECT
    
public:
    // ...
};
```

### 2.4 函数注释

```cpp
/**
 * @brief 检查两个游戏对象之间的碰撞
 * 
 * 使用AABB算法检测两个矩形对象是否发生碰撞
 * 
 * @param obj1 第一个游戏对象，不能为nullptr
 * @param obj2 第二个游戏对象，不能为nullptr
 * @param collisionPoint 碰撞点坐标（可选参数）
 * 
 * @return bool 是否发生碰撞
 * @retval true 两个对象发生碰撞
 * @retval false 两个对象未发生碰撞
 * 
 * @note 时间复杂度为O(1)
 * @warning 对于旋转对象，精度可能会降低
 */
bool checkCollision(GameObject* obj1, GameObject* obj2, QPointF* collisionPoint = nullptr);

/**
 * @brief 获取玩家当前生命值
 * @return int 生命值（0-100）
 */
int getHealth() const { return player_health; }

/**
 * @brief 设置玩家生命值
 * @param health 新的生命值，范围[0, 100]
 * @note 超出范围的值会被自动限制
 */
void setHealth(int health);
```

### 2.5 成员变量注释

```cpp
class Player {
private:
    int player_health;              ///< 玩家生命值 [0-100]
    float movement_speed;           ///< 移动速度（像素/秒）
    QPointF current_position;       ///< 当前位置（世界坐标）
    QString player_name;            ///< 玩家名称
    bool is_grounded;              ///< 是否在地面上
    QTimer* update_timer;          ///< 更新定时器
    
    static int total_player_count;  ///< 玩家总数
};
```

### 2.6 行内注释

```cpp
void updatePlayer(float deltaTime) {
    // 应用重力效果
    velocity.setY(velocity.y() + GRAVITY_FORCE * deltaTime);
    
    // 限制最大下落速度，防止穿透地面
    if (velocity.y() > MAX_FALL_SPEED) {
        velocity.setY(MAX_FALL_SPEED);
    }
    
    // 更新位置
    current_position += velocity * deltaTime;
    
    /* 
     * 复杂的碰撞检测逻辑
     * 使用分离轴定理进行精确碰撞检测
     */
    for (const auto& platform : platforms) {
        if (checkSATCollision(this, platform)) {
            resolveCollision(platform);
        }
    }
}
```

### 2.7 特殊标记注释

```cpp
/**
 * @todo 实现更精确的碰撞检测算法
 * @fixme 修复在高速移动时的碰撞穿透问题
 * @bug 已知问题：在某些情况下会出现内存泄漏
 * @note 注意：此方法会修改全局状态
 * @warning 警告：不要在多线程环境中调用
 * @deprecated 此方法已过时，请使用newMethod()替代
 */
void someMethod();
```

### 2.8 代码段注释

```cpp
void processInput() {
    // === 处理玩家1输入 ===
    if (isKeyPressed(Qt::Key_W)) {
        player1->jump();
    }
    if (isKeyPressed(Qt::Key_A)) {
        player1->moveLeft();
    }
    
    // === 处理玩家2输入 ===
    if (isKeyPressed(Qt::Key_Up)) {
        player2->jump();
    }
    if (isKeyPressed(Qt::Key_Left)) {
        player2->moveLeft();
    }
    
    // === 处理合作动作 ===
    if (isKeyPressed(Qt::Key_W) && isKeyPressed(Qt::Key_Up)) {
        performCooperativeJump();  // 双人同时跳跃
    }
}
```

---

## 3. 完整示例

### 3.1 头文件示例

```cpp
/**
 * @file Player.h
 * @brief 玩家角色类定义
 * @author 开发团队
 * @date 2024-12-15
 * @version 1.0.0
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPointF>
#include <QString>

/**
 * @class Player
 * @brief 玩家角色管理类
 * 
 * 负责管理玩家的状态、行为和属性。支持双人合作模式。
 */
class Player : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @enum PlayerRole
     * @brief 玩家角色类型
     */
    enum class PlayerRole {
        LionHead,    ///< 狮头控制者
        LionTail     ///< 狮尾控制者
    };
    
    /**
     * @brief 构造函数
     * @param name 玩家名称
     * @param role 玩家角色
     * @param parent 父对象指针
     */
    explicit Player(const QString& name, PlayerRole role, QObject* parent = nullptr);
    
    /**
     * @brief 获取玩家名称
     * @return QString 玩家名称
     */
    QString getName() const { return player_name; }
    
    /**
     * @brief 玩家跳跃动作
     * @param force 跳跃力度，范围[0.0, 1.0]
     * @return bool 是否成功执行跳跃
     */
    bool jump(float force = 1.0f);
    
signals:
    /**
     * @brief 玩家状态改变信号
     * @param player 发生变化的玩家指针
     */
    void stateChanged(Player* player);
    
private:
    QString player_name;            ///< 玩家名称
    PlayerRole player_role;         ///< 玩家角色
    QPointF current_position;       ///< 当前位置
    QPointF current_velocity;       ///< 当前速度
    bool is_grounded;              ///< 是否在地面上
    float health_points;           ///< 生命值 [0.0, 100.0]
    
    static int total_player_count; ///< 玩家总数
    
    /**
     * @brief 验证跳跃条件
     * @return bool 是否可以跳跃
     */
    bool canJump() const;
};

#endif // PLAYER_H
```

### 3.2 源文件示例

```cpp
/**
 * @file Player.cpp
 * @brief 玩家角色类实现
 * @author 开发团队
 * @date 2024-12-15
 */

#include "Player.h"
#include "GameConstants.h"
#include <QDebug>

// 静态成员初始化
int Player::total_player_count = 0;

Player::Player(const QString& name, PlayerRole role, QObject* parent)
    : QObject(parent)
    , player_name(name)
    , player_role(role)
    , current_position(0.0f, 0.0f)
    , current_velocity(0.0f, 0.0f)
    , is_grounded(false)
    , health_points(100.0f)
{
    // 增加玩家计数
    ++total_player_count;
    
    qDebug() << "创建玩家:" << name << "角色:" << static_cast<int>(role);
}

bool Player::jump(float force)
{
    // 验证跳跃条件
    if (!canJump()) {
        qDebug() << "玩家" << player_name << "无法跳跃：不在地面上";
        return false;
    }
    
    // 限制力度范围
    force = qBound(0.0f, force, 1.0f);
    
    // 计算跳跃速度
    float jumpVelocity = JUMP_BASE_VELOCITY * force;
    
    // 根据角色调整跳跃效果
    if (player_role == PlayerRole::LionTail) {
        jumpVelocity *= LION_TAIL_JUMP_MODIFIER;  // 狮尾跳跃力度调整
    }
    
    // 应用跳跃
    current_velocity.setY(-jumpVelocity);  // Y轴向上为负
    is_grounded = false;
    
    qDebug() << "玩家" << player_name << "跳跃，力度:" << force;
    return true;
}

bool Player::canJump() const
{
    // 必须在地面上且生命值大于0
    return is_grounded && health_points > 0.0f;
}
```

---

## 4. 代码审查检查清单

### 4.1 命名检查

- [ ] 类名使用大驼峰命名法（PascalCase）
- [ ] 函数名使用小驼峰命名法（camelCase），动词开头
- [ ] 成员变量使用蛇形命名法（snake_case）
- [ ] 局部变量使用小驼峰命名法（camelCase）
- [ ] 常量使用全大写+下划线
- [ ] 变量名具有描述性，避免无意义缩写
- [ ] 文件名与主要类名保持一致

### 4.2 注释检查

- [ ] 所有公共接口都有完整的文档注释
- [ ] 复杂算法有详细的实现说明
- [ ] 注释与代码保持同步
- [ ] 使用标准的Doxygen格式
- [ ] 成员变量有简洁的行内注释
- [ ] 标记了已知问题和限制

---

**本规范适用于《醒狮跃境》项目的所有代码，请团队成员严格遵循。**