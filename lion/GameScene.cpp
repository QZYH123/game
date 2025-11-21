/**
 * @file GameScene.cpp
 * @brief 游戏场景类实现（含动画控制测试）
 * @author sqbsayori
 * @date 2025-10-19
 */

#include "GameScene.h"
#include "LionAnimation.h"
#include <QPushButton>
#include "qpainter.h"
#include "QKeyEvent"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QDebug>
#include "Config.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QDateTime>
extern int map[GRID_WIDTH][GRID_HEIGHT];
int map[GRID_WIDTH][GRID_HEIGHT];
int map1[GRID_WIDTH][GRID_HEIGHT];
GameScene::GameScene(QWidget* parent): QWidget(parent){

    // 设置场景基本属性
    setWindowTitle(TITLE);
    setFixedSize(XSIZE, YSIZE);  // 匹配项目分辨率
    memset(map,0,sizeof(map));
    background=BackGround(1);
    
    // === 新增：初始化关卡系统 ===
    current_level_data = nullptr;
    
    // === 初始化暂停功能 ===
    is_paused = false;
    pause_menu = nullptr;
    resume_button = nullptr;
    restart_button = nullptr;
    main_menu_button = nullptr;
    
    // 初始化UI标签
    objective_label = new QLabel(this);
    objective_label->setGeometry(10, 10, 300, 30);
    objective_label->setStyleSheet("color: white; font-size: 16px; font-weight: bold; background-color: rgba(0,0,0,100);");
    
    tutorial_label = new QLabel(this);
    tutorial_label->setGeometry(10, 50, 400, 60);
    tutorial_label->setStyleSheet("color: yellow; font-size: 14px; background-color: rgba(0,0,0,100);");
    tutorial_label->setWordWrap(true);
    
    // 初始化返回按钮
    back_button = new QPushButton("返回", this);
    back_button->setGeometry(10, 10, 90, 32);
    back_button->setText("返回菜单");
    back_button->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 0, 0, 180);"
        "    color: white;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    border: 2px solid white;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(255, 50, 50, 200);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(200, 0, 0, 220);"
        "}"
    );
    
    // 连接返回按钮信号
    connect(back_button, &QPushButton::clicked, [this]() {
        emit backToMainMenu();
    });
    
    // 加载纹理资源（使用Qt资源路径，避免工作目录影响）
    vegetable_texture.load(":/images/vegetable.png");
    if (vegetable_texture.isNull()) {
        // 如果没有青菜图片，创建一个简单的绿色方块
        vegetable_texture = QPixmap(B0, B0);
        vegetable_texture.fill(Qt::green);
    }
    
    exit_texture.load(":/images/door.png");
    if (exit_texture.isNull()) {
        // 如果没有出口图片，创建一个简单的红色方块
        exit_texture = QPixmap(B0, B0);
        exit_texture.fill(Qt::red);
    }
    
    // 加载箭机关纹理（统一使用 archery.png）
    arrow_trap_right_texture.load(":/images/archery.png");
    arrow_trap_left_texture.load(":/images/archery.png");
    arrow_trap_up_texture.load(":/images/archery.png");
    arrow_trap_down_texture.load(":/images/archery.png");
    // 修正箭机关默认朝向：顺时针旋转90°
    {
        QTransform archeryRotate;
        archeryRotate.rotate(90);
        if (!arrow_trap_right_texture.isNull())
            arrow_trap_right_texture = arrow_trap_right_texture.transformed(archeryRotate, Qt::SmoothTransformation);
        if (!arrow_trap_left_texture.isNull())
            arrow_trap_left_texture = arrow_trap_left_texture.transformed(archeryRotate, Qt::SmoothTransformation);
        if (!arrow_trap_up_texture.isNull())
            arrow_trap_up_texture = arrow_trap_up_texture.transformed(archeryRotate, Qt::SmoothTransformation);
        if (!arrow_trap_down_texture.isNull())
            arrow_trap_down_texture = arrow_trap_down_texture.transformed(archeryRotate, Qt::SmoothTransformation);
    }
    
    // 加载移动平台和开关门纹理（如果没有图片，创建简单的颜色方块）
    horizontal_platform_texture.load(":/images/moving_platform.png");
    if (horizontal_platform_texture.isNull()) {
        horizontal_platform_texture = QPixmap(B0 * 2, B0);
        horizontal_platform_texture.fill(QColor(139, 69, 19)); // 棕色
    }

    vertical_platform_texture.load(":/images/moving_platform.png");
    if (vertical_platform_texture.isNull()) {
        vertical_platform_texture = QPixmap(B0, B0 * 2);
        vertical_platform_texture.fill(QColor(139, 69, 19)); // 棕色
    }
    
    switch_texture.load(":/images/switch.svg");
    if (switch_texture.isNull()) {
        switch_texture = QPixmap(B0, B0);
        switch_texture.fill(QColor(255, 255, 0)); // 黄色
    }
    
    door_texture.load(":/images/door.png");
    if (door_texture.isNull()) {
        door_texture = QPixmap(B0, B0);
        door_texture.fill(QColor(101, 67, 33)); // 深棕色
    }

    // 加载箭矢纹理（保持原始方向）
    arrow_texture.load(":/images/arrow.png");
    if (arrow_texture.isNull()) {
        arrow_texture = QPixmap(B0, B0 / 4);
        arrow_texture.fill(Qt::red);
    }
    
    init();

    // 初始化关卡管理器并加载第一关
    LevelManager::getInstance().initialize();
    loadLevelInternal(0);  // 加载教学关卡

    gameStart();
    updatenum=0;

    // +++ 新增：初始化残影计时器
    lastAfterimageTime = 0;
}
GameScene::~GameScene() {
    // 若有动态分配的资源，在此释放
    if (pause_menu) {
        delete pause_menu;
        pause_menu = nullptr;
    }
}
void GameScene::init()
{
    setFixedSize(XSIZE, YSIZE);
    setWindowTitle(TITLE);
    Timer.setInterval(GAME_TICK);
    mapInit();
}
void GameScene::mapInit(){
    for(int i=0;i<GRID_WIDTH;i++)
        for(int j=23;j>20;j--)
            map[i][j]=1;
    for(int i=0;i<17;i++)
        map[i][19]=1;
    for(int i=23;i>17;i--)
        map[i][18]=1;
    for(int i=7;i<22;i++)
        map[i][16]=1;
    for(int i=0;i<14;i++)
        map[i][13]=1;
    for(int i=4;i<9;i++)
        map[i][5]=1;
    for(int i=0;i<3;i++)
        map[i][2]=1;
        map[4][18]=map[20][15]=map[13][14]=map[16][14]=
        map[17][14]=map[18][14]=map[21][14]=map[22][13]=
        map[22][12]=map[20][12]=map[19][12]=map[18][12]=
        map[2][11]=map[3][11]=map[22][11]=map[13][10]=
        map[16][10]=map[17][10]=map[21][10]=map[0][9]=
        map[1][9]=map[6][9]=map[7][9]=map[5][9]=
        map[12][9]=map[19][9]=map[20][9]=map[21][9]=
        map[11][8]=map[15][8]=map[16][8]=map[2][7]=
        map[3][7]=map[10][7]=map[9][6]=map[17][6]=
        map[18][6]=map[19][6]=map[23][6]=map[15][5]=
        map[14][5]=map[20][5]=map[22][5]=map[12][4]=
        map[13][4]=map[22][4]=map[4][3]=map[5][3]=
        map[11][3]=map[10][2]=map[9][1]=map[9][0]=1;
    for(int i=0;i<GRID_WIDTH;i++)
        for(int j=0;j<GRID_HEIGHT;j++)
            map1[i][j]=map[i][j];
}
void GameScene::gameStart()
{
    // 先断开之前的连接，避免重复连接
    Timer.disconnect();
    Timer.start();
    level_timer.restart();
    
    // 初始化移动平台和开关门
    initializeMovingPlatforms();
    initializeSwitchDoors();
    
    // pl.setMoveState(leftpress,rightpress);
    connect(&Timer,&QTimer::timeout,[=](){
        // 如果游戏暂停，不更新游戏逻辑
        if (is_paused) {
            return;
        }
        
        tick_counter++;
        
        // === 新增：更新移动平台 ===
        updateMovingPlatforms();
        
        // 保存玩家移动前的位置
        int prev_x = pl.x;
        int prev_y = pl.y;
        
        pl.update();

        // +++ 新增：更新残影逻辑
        updateAfterimages();

        // 只有当玩家不在移动平台上，或者在移动平台上但有主动输入时，才处理左右移动和动画
        if (!pl.onMovingPlatform || (pl.onMovingPlatform && (leftpress || rightpress))) {
            if(leftpress) pl.left();
            if(rightpress) pl.right();
        }

        // 动画状态由player.updateAnimationState()统一处理，此处不再重复处理
        
        // === 新增：检查移动平台碰撞（在玩家更新后） ===
        checkMovingPlatformCollisions();
        
        // === 新增：处理玩家跟随移动平台（在碰撞检测后，独立处理） ===
        handlePlatformFollowing();
        
        // 检查门碰撞，如果与关闭的门碰撞则恢复到之前的位置
        QRectF playerRect(pl.x, pl.y, pl.w, pl.h);
        if (checkDoorCollision(playerRect)) {
            pl.x = prev_x;
            pl.y = prev_y;
        }
        
        // 箭机关：周期性发射箭矢
        if (current_level_data && tick_counter % 60 == 0) { // 每1秒发射一次
            const auto& elements = current_level_data->getGameElements();
            for (const auto& e : elements) {
                if (e.element_type == GameElementType::ArrowTrap) {
                    Projectile p;
                    p.pos = e.position + QPointF(e.size.x()/2, e.size.y()/2);
                    p.size = QPointF(20.0, 6.0);
                    p.active = true;
                    
                    // 根据方向设置速度和大小
                    QString direction = "right"; // 默认方向
                    if (e.properties.contains("direction")) {
                        direction = e.properties.value("direction").toString();
                    }
                    
                    float speed = 6.0f; // 约3格/秒的速度
                    if (direction == "right") {
                        p.vel = QPointF(speed, 0.0);
                        p.size = QPointF(2 * B0, 8.0); // 调整箭大小：长度2格，厚度8像素
                    } else if (direction == "left") {
                        p.vel = QPointF(-speed, 0.0);
                        p.size = QPointF(2 * B0, 8.0);
                    } else if (direction == "up") {
                        p.vel = QPointF(0.0, -speed);
                        p.size = QPointF(8.0, 2 * B0);
                    } else if (direction == "down") {
                        p.vel = QPointF(0.0, speed);
                        p.size = QPointF(8.0, 2 * B0);
                    }
                    
                    projectiles.push_back(p);
                }
            }
        }
        // 更新箭矢位置并检测碰撞/出界
        if (!projectiles.isEmpty()) {
            QRectF playerRect(pl.x, pl.y, pl.w, pl.h);
            for (auto &p : projectiles) {
                if (!p.active) continue;
                p.pos += p.vel;
                
                // 检查出界
                if (p.pos.x() < -50 || p.pos.x() > XSIZE + 50 || p.pos.y() < -50 || p.pos.y() > YSIZE + 50) {
                    p.active = false;
                    continue;
                }
                
                QRectF arrowRect(p.pos.x(), p.pos.y(), p.size.x(), p.size.y());
                
                // 检查与玩家的碰撞
                if (arrowRect.intersects(playerRect)) {
                    is_dead = true;
                    Timer.stop();
                    gameover();
                    return;
                }
                
                // 检查与方块的碰撞
                bool hitBlock = false;
                
                // 检查箭矢四个角是否与实心方块碰撞
                int leftCol = static_cast<int>(p.pos.x()) / B0;
                int rightCol = static_cast<int>(p.pos.x() + p.size.x()) / B0;
                int topRow = static_cast<int>(p.pos.y()) / B0;
                int bottomRow = static_cast<int>(p.pos.y() + p.size.y()) / B0;
                
                // 确保坐标在地图范围内
                leftCol = qMax(0, qMin(leftCol, GRID_WIDTH - 1));
                rightCol = qMax(0, qMin(rightCol, GRID_WIDTH - 1));
                topRow = qMax(0, qMin(topRow, GRID_HEIGHT - 1));
                bottomRow = qMax(0, qMin(bottomRow, GRID_HEIGHT - 1));
                
                // 检查箭矢覆盖的所有网格
                for (int col = leftCol; col <= rightCol && !hitBlock; ++col) {
                    for (int row = topRow; row <= bottomRow && !hitBlock; ++row) {
                        if (map[col][row] == 1) { // 实心方块
                            hitBlock = true;
                        }
                    }
                }
                
                // 如果与方块碰撞，标记箭矢为无效
                if (hitBlock) {
                    p.active = false;
                }
            }
            // 清理无效箭矢
            projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& pr){return !pr.active;}), projectiles.end());
        }
        
        // === 新增：检查开关碰撞 ===
        checkSwitchCollisions();
        
        // === 新增：检查游戏元素碰撞 ===
        checkGameElementCollisions();
        
        // === 新增：更新UI显示 ===
        updateObjectiveDisplay();
        updateTutorialHints();
        
        // 设置游戏开始状态
        if(pl.getLeftPressed() || pl.getRightPressed()){
            if(begin==false)
                begin=true;
        }
        update();

    });
}
void GameScene::keyPressEvent(QKeyEvent *event) //按键事件
{
    // ESC键暂停/恢复游戏
    if (event->key() == Qt::Key_Escape)
    {
        if (is_paused)
        {
            resumeGame();
        }
        else
        {
            pauseGame();
        }
        return;
    }
    
    // 如果游戏暂停，不处理其他按键
    if (is_paused)
    {
        return;
    }
    
    if (event->key() == Qt::Key_A && event->type())
    {
        // leftpress = 1;
        // pl.setMoveState(leftpress, rightpress);
        pl.setLeftPressed(true);
    }
    else if (event->key() == Qt::Key_D)
    {
        // rightpress = 1;
        // pl.setMoveState(leftpress, rightpress);
        pl.setRightPressed(true);
    }
    else if (event->key() == Qt::Key_K && !pl.isJump && (pl.onGround || pl.onMovingPlatform))
    {
        pl.jump();
    }
    // +++ 新增：Shift键触发冲刺
    else if (event->key() == Qt::Key_Shift && !event->isAutoRepeat())
    {
        pl.startDash();
    }
    update();
}
void GameScene::keyReleaseEvent(QKeyEvent *event)//松开按键事件
{
    // 如果游戏暂停，不处理按键释放
    if (is_paused)
    {
        return;
    }
    
    if (event->key() == Qt::Key_A && event->type())
    {
        // leftpress = 0;
        pl.setLeftPressed(false);
    }
    if (event->key() == Qt::Key_D)
    {
        // rightpress = 0;
        pl.setRightPressed(false);
    }
    update();
}

void GameScene::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // 如果暂停菜单存在且可见，调整其大小
    if (pause_menu && pause_menu->isVisible()) {
        pause_menu->resize(this->size());
    }
}

void GameScene::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    block5.load(BLOCK5);
    if (block5.isNull()) {
        block5 = QPixmap(B0, B0);
        block5.fill(Qt::gray);
    }
    painter.drawPixmap(background.map1_x, 0,XSIZE+5,YSIZE, background.map1);
    painter.drawPixmap(background.map2_x, 0,XSIZE+5,YSIZE, background.map2);
    painter.drawPixmap(background.map3_x, 0,XSIZE+5,YSIZE, background.map3);
    
    for(int i=0;i<GRID_WIDTH;i++)
        for(int j=0;j<GRID_HEIGHT;j++)
        {
            switch(map1[i][j]){
            case 1:
                painter.drawPixmap(i*B0, j*B0,W,W, block5);
                break;
            }
        }
    
    // 绘制游戏元素
    drawGameElements(painter);
    
    // 绘制箭矢（使用贴图并根据方向旋转/镜像）
    for (const auto &p : projectiles) {
        if (!p.active) continue;
        QRectF arrowRect(p.pos.x(), p.pos.y(), p.size.x(), p.size.y());
        QPixmap pix = arrow_texture;
        // 根据速度方向旋转或镜像
        if (std::abs(p.vel.y()) > std::abs(p.vel.x())) {
            // 垂直方向
            QTransform tf;
            if (p.vel.y() < 0) {
                tf.rotate(-90);
            } else {
                tf.rotate(90);
            }
            pix = pix.transformed(tf, Qt::SmoothTransformation);
        } else {
            // 水平方向：向左时镜像
            if (p.vel.x() < 0) {
                pix = QPixmap::fromImage(pix.toImage().mirrored(true, false));
            }
        }
        painter.drawPixmap(arrowRect.toRect(), pix);
    }
    
    if (!afterimages.isEmpty()) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        // 遍历所有残影
        for (const auto& img : afterimages) {
            qint64 age = now - img.spawnTime;
            // 计算透明度 (残影越老越透明)
            double opacity = 1.0 - (double)age / AFTERIMAGE_LIFETIME;

            if (opacity > 0.1) { // 只绘制还未完全消失的
                painter.setOpacity(opacity * 0.5); // 设置最大 50% 的透明度
                painter.drawPixmap(img.rect.toRect(), img.frame);
            }
        }
        painter.setOpacity(1.0); // 恢复不透明度，准备绘制玩家
    }

    // 绘制玩家角色
    QPixmap currentFrame = pl.getCurrentAnimationFrame();
    if (!currentFrame.isNull()) {
        painter.drawPixmap(pl.x, pl.y, pl.w, pl.h,
                           currentFrame.scaled(pl.w, pl.h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

BackGround::BackGround()
{
    map1_x = XSIZE;
    map2_x = 0;
    map3_x = -XSIZE;
    m_scroll_speed = BG_SPEED;
}
BackGround::BackGround(int num)
{
    if(num==1)
    {
        map1.load(BACK_GROUND1); //设置背景
        map2.load(BACK_GROUND1);
        map3.load(BACK_GROUND1);
    }
    if(num==2)
    {
        map1.load(BACK_GROUND2); //设置背景
        map2.load(BACK_GROUND2);
        map3.load(BACK_GROUND2);
    }
    map1_x = XSIZE;
    map2_x = 0;
    map3_x = -XSIZE;
    m_scroll_speed = BG_SPEED;
}
void BackGround::mappositionr()
{
    map1_x -= BG_SPEED, map2_x -= BG_SPEED, map3_x -= BG_SPEED;
    if (map1_x <= 0)
        map1_x = XSIZE, map2_x = 0, map3_x = -XSIZE;
}
void BackGround::mappositionl()
{
    map1_x += BG_SPEED, map2_x += BG_SPEED, map3_x += BG_SPEED;
    if (map2_x >= XSIZE)
        map1_x = XSIZE, map2_x = 0, map3_x = -XSIZE;
}

// === 新增：关卡系统方法实现 ===

bool GameScene::loadLevel(int levelIndex)
{
    return loadLevelInternal(levelIndex);
}

bool GameScene::loadLevelInternal(int levelIndex)
{
    qDebug() << "加载关卡：" << levelIndex;
    
    // 从关卡管理器获取关卡数据
    current_level_data = LevelManager::getInstance().getLevelData(levelIndex);
    if (!current_level_data) {
        qDebug() << "无法加载关卡" << levelIndex;
        return false;
    }
    current_level_data->setCustomLevel(false);
    
    // 设置当前关卡
    LevelManager::getInstance().setCurrentLevel(levelIndex);
    
    // 重置关卡状态
    resetLevel();
    
    // 从关卡数据更新传统地图数组（兼容现有代码）
    // 手动填充地图数组以适应新的网格大小
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            GameElementType type = current_level_data->getElementAt(x, y);
            if (type == GameElementType::SolidBlock) {
                map[x][y] = 1;
                map1[x][y] = 1;
            } else {
                map[x][y] = 0;
                map1[x][y] = 0;
            }
        }
    }
    
    // 调试：检查地图数据
    int blockCount = 0;
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (map1[i][j] == 1) {
                blockCount++;
            }
        }
    }
    qDebug() << "地图中方块数量：" << blockCount;
    
    // 设置玩家起始位置
    QPointF startPos = current_level_data->getPlayerStartPosition();
    pl.x = static_cast<int>(startPos.x());
    pl.y = static_cast<int>(startPos.y());
    qDebug() << "玩家起始位置：" << pl.x << "," << pl.y;
    
    // 保存游戏进度
    saveGameProgress(levelIndex);
    
    // 启动游戏定时器
    gameStart();
    
    qDebug() << "关卡加载完成：" << current_level_data->getLevelName();
    return true;
}

bool GameScene::loadLevelFromFile(const QString& filePath)
{
    // 获取关卡管理器
    LevelManager& levelManager = LevelManager::getInstance();
    
    // 从文件加载关卡数据
    LevelData* levelData = levelManager.loadLevelFromFile(filePath);
    if (!levelData) {
        qDebug() << "从文件加载关卡失败：" << filePath;
        return false;
    }
    
    // 保存当前关卡数据
    current_level_data = levelData;
    current_level_data->setCustomLevel(true);
    
    // 重置关卡状态
    resetLevel();
    
    // 从关卡数据更新传统地图数组
    // 手动填充地图数组以适应新的网格大小
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            GameElementType type = current_level_data->getElementAt(x, y);
            if (type == GameElementType::SolidBlock) {
                map[x][y] = 1;
                map1[x][y] = 1;
            } else {
                map[x][y] = 0;
                map1[x][y] = 0;
            }
        }
    }
    
    // 设置玩家位置
    QPointF playerPos = current_level_data->getPlayerStartPosition();
    pl.x = static_cast<int>(playerPos.x());
    pl.y = static_cast<int>(playerPos.y());
    
    // 更新UI显示
    updateObjectiveDisplay();
    updateTutorialHints();
    
    // 启动游戏定时器
    gameStart();
    
    qDebug() << "从文件加载关卡成功：" << filePath;
    return true;
}

void GameScene::checkGameElementCollisions()
{
    if (!current_level_data) return;
    
    QRectF playerRect(pl.x, pl.y, pl.w, pl.h);
    bool touchedWater = false;
    
    const auto& elements = current_level_data->getGameElements();
    for (const auto& element : elements) {
        QRectF elementRect(element.position.x(), element.position.y(), 
                          element.size.x(), element.size.y());
        
        if (playerRect.intersects(elementRect)) {
            bool alreadyCollected = false;
            for (const auto& collected : collected_items) {
                if (collected.position == element.position && 
                    collected.element_type == element.element_type) {
                    alreadyCollected = true;
                    break;
                }
            }
            
            switch (element.element_type) {
            case GameElementType::Vegetable:
                if (!alreadyCollected) collectItem(element);
                break;
            case GameElementType::LevelExit: {
                // 检查关卡完成条件
                bool canComplete = false;
                const auto& objectives = current_level_data->getObjectives();
                
                if (objectives.isEmpty()) {
                    // 没有设置目标，直接允许通关（向后兼容）
                    canComplete = true;
                } else {
                    // 有目标设置，检查是否有青菜收集目标
                    bool hasVegetableObjective = false;
                    bool allVegetablesCollected = true;
                    
                    for (const auto& obj : objectives) {
                        if (obj.objective_type == "collect_vegetables") {
                            hasVegetableObjective = true;
                            if (!obj.isCompleted()) {
                                allVegetablesCollected = false;
                                break;
                            }
                        }
                    }
                    
                    if (hasVegetableObjective) {
                        // 有青菜收集目标，必须完成才能通关
                        canComplete = allVegetablesCollected;
                    } else {
                        // 没有青菜收集目标，检查所有其他目标是否完成
                        canComplete = current_level_data->areAllObjectivesCompleted();
                    }
                }
                
                if (canComplete) {
                    // 可以通关
                    if (!alreadyCollected) {
                        collectItem(element);
                        Timer.stop();
                        gamewin();
                        return;
                    }
                } else {
                    // 未满足通关条件，显示提示
                    qDebug() << "还有目标未完成，无法通关！";
                    showGameMessage("还有目标未完成，无法通关！", 3000);
                }
                break;
            }
            case GameElementType::Water: {
                touchedWater = true;
                if (!is_in_water) {
                    is_in_water = true;
                    // 进入水域后：移动速度减半
                    pl.setMoveSpeedScale(0.5);
                }
                break;
            }
            case GameElementType::Lava: {
                is_dead = true;
                Timer.stop();
                gameover();
                return;
            }
            default:
                break;
            }
        }
    }
    // 离开水域时恢复
    if (!touchedWater && is_in_water) {
        is_in_water = false;
        pl.setMoveSpeedScale(1.0);
    }
}


void GameScene::collectItem(const GameElement& element)
{
    // 添加到已收集列表
    collected_items.append(element);
    
    // 更新关卡目标进度
    switch (element.element_type) {
    case GameElementType::Vegetable:
        AudioController::getInstance().playSound(SoundType::Collect);
        current_level_data->updateObjectiveProgress("collect_vegetables", 1);
        qDebug() << "收集到青菜！";
        break;
    case GameElementType::LevelExit:
        AudioController::getInstance().playSound(SoundType::Win);
        current_level_data->updateObjectiveProgress("reach_exit", 1);
        qDebug() << "到达终点！";
        break;
    default:
        break;
    }
}

void GameScene::updateObjectiveDisplay()
{
    if (!current_level_data || !objective_label) return;
    
    QString objectiveText = "目标：";
    const auto& objectives = current_level_data->getObjectives();
    
    for (const auto& objective : objectives) {
        objectiveText += QString("%1 (%2/%3) ")
                        .arg(objective.description)
                        .arg(objective.current_count)
                        .arg(objective.target_count);
    }
    
    objective_label->setText(objectiveText);
}

void GameScene::updateTutorialHints()
{
    if (!tutorial_label) return;
    
    // 根据游戏状态显示不同的教学提示
    QString hintText;
    
    if (!begin) {
        hintText = "按 A/D 键移动，按 K 键跳跃\n收集所有青菜后到达红色终点！";
    } else {
        // 检查玩家进度给出提示
        const auto& objectives = current_level_data->getObjectives();
        bool hasVegetableObjective = false;
        bool vegetableCompleted = false;
        
        for (const auto& objective : objectives) {
            if (objective.objective_type == "collect_vegetables") {
                hasVegetableObjective = true;
                if (objective.isCompleted()) {
                    vegetableCompleted = true;
                }
                break;
            }
        }
        
        if (hasVegetableObjective && !vegetableCompleted) {
            hintText = "继续收集青菜！";
        } else if (vegetableCompleted) {
            hintText = "很好！现在前往红色终点完成关卡！";
        } else {
            hintText = "";
        }
    }
    
    tutorial_label->setText(hintText);
}

bool GameScene::checkLevelCompletion()
{
    if (!current_level_data) return false;
    
    return current_level_data->areAllObjectivesCompleted();
}

void GameScene::drawGameElements(QPainter& painter)
{
    if (!current_level_data) return;
    
    const auto& elements = current_level_data->getGameElements();
    for (const auto& element : elements) {
        bool isCollected = false;
        for (const auto& collected : collected_items) {
            if (collected.position == element.position && 
                collected.element_type == element.element_type) {
                isCollected = true;
                break;
            }
        }
        if (isCollected) continue;
        
        QPixmap texture;
        bool drawRect = false;
        QColor rectColor;
        switch (element.element_type) {
        case GameElementType::Vegetable:
            texture = vegetable_texture;
            break;
        case GameElementType::LevelExit:
            texture = exit_texture;
            break;
        case GameElementType::Water:
            drawRect = true;
            rectColor = QColor(0, 120, 255, 180);
            if (!water_texture.isNull()) texture = water_texture;
            break;
        case GameElementType::Lava:
            drawRect = true;
            rectColor = QColor(255, 60, 0, 200);
            if (!lava_texture.isNull()) texture = lava_texture;
            break;
        case GameElementType::ArrowTrap:
            {
                // 根据箭机关方向选择对应纹理
                QString direction = "right"; // 默认方向
                if (element.properties.contains("direction")) {
                    direction = element.properties["direction"].toString();
                }
                
                if (direction == "right" && !arrow_trap_right_texture.isNull()) {
                    texture = arrow_trap_right_texture;
                } else if (direction == "left" && !arrow_trap_left_texture.isNull()) {
                    texture = arrow_trap_left_texture;
                } else if (direction == "up" && !arrow_trap_up_texture.isNull()) {
                    texture = arrow_trap_up_texture;
                } else if (direction == "down" && !arrow_trap_down_texture.isNull()) {
                    texture = arrow_trap_down_texture;
                } else {
                    // 如果没有对应方向的纹理，使用默认颜色
                    drawRect = true;
                    rectColor = QColor(180, 180, 180, 200);
                }
            }
            break;
        case GameElementType::HorizontalPlatform:
            texture = horizontal_platform_texture;
            break;
        case GameElementType::VerticalPlatform:
            texture = vertical_platform_texture;
            break;
        case GameElementType::Switch:
            texture = switch_texture;
            break;
        case GameElementType::Door:
            texture = door_texture;
            break;
        default:
            continue;
        }
        
        int x = static_cast<int>(element.position.x());
        int y = static_cast<int>(element.position.y());
        const int w = static_cast<int>(element.size.x());
        const int h = static_cast<int>(element.size.y());
        
        // 对于移动平台，使用动态位置
        if (element.element_type == GameElementType::HorizontalPlatform || 
            element.element_type == GameElementType::VerticalPlatform) {
            for (const auto& platform : moving_platforms) {
                if (platform.element_index == &element - &elements[0]) {
                    x = static_cast<int>(platform.current_pos.x());
                    y = static_cast<int>(platform.current_pos.y());
                    break;
                }
            }
        }
        
        // 对于门，检查是否应该显示（门关闭时显示）
        bool shouldDraw = true;
        if (element.element_type == GameElementType::Door) {
            for (const auto& switchDoor : switch_doors) {
                if (switchDoor.door_element_index == &element - &elements[0]) {
                    shouldDraw = !switchDoor.door_is_open;
                    break;
                }
            }
        }
        
        if (!shouldDraw) continue;
        
        if (!texture.isNull() && element.element_type == GameElementType::Vegetable) {
            painter.drawPixmap(x, y, w, h, texture);
        } else if (!texture.isNull() && element.element_type == GameElementType::LevelExit) {
            // 检查是否收集了所有青菜
            bool allVegetablesCollected = true;
            const auto& objectives = current_level_data->getObjectives();
            for (const auto& obj : objectives) {
                if (obj.objective_type == "collect_vegetables" && !obj.isCompleted()) {
                    allVegetablesCollected = false;
                    break;
                }
            }
            
            if (allVegetablesCollected) {
                // 青菜收集完毕，正常显示终点
                painter.drawPixmap(x, y, w, h, texture);
            } else {
                // 青菜未收集完毕，半透明显示终点，表示无法通关
                painter.setOpacity(0.3);
                painter.drawPixmap(x, y, w, h, texture);
                painter.setOpacity(1.0); // 恢复透明度
            }
        } else if (!texture.isNull() && (element.element_type == GameElementType::Water || element.element_type == GameElementType::Lava || element.element_type == GameElementType::ArrowTrap)) {
            painter.drawPixmap(x, y, w, h, texture);
        } else if (!texture.isNull() && (element.element_type == GameElementType::HorizontalPlatform || 
                                        element.element_type == GameElementType::VerticalPlatform ||
                                        element.element_type == GameElementType::Switch ||
                                        element.element_type == GameElementType::Door)) {
            painter.drawPixmap(x, y, w, h, texture);
        } else if (drawRect) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(rectColor));
            painter.drawRect(QRect(x, y, w, h));
        }
    }
}

// === 暂停功能实现 ===

void GameScene::pauseGame()
{
    if (is_paused) return;
    
    is_paused = true;
    
    // 创建暂停菜单（如果还没有创建）
    if (!pause_menu) {
        createPauseMenu();
    }
    
    showPauseMenu();
    qDebug() << "Game paused";
}

void GameScene::resumeGame()
{
    if (!is_paused) return;
    
    is_paused = false;
    hidePauseMenu();
    qDebug() << "Game resumed";
}

void GameScene::createPauseMenu()
{
    // 创建暂停菜单容器
    pause_menu = new QWidget(this);
    pause_menu->setStyleSheet("background-color: rgba(0, 0, 0, 180);"); // 半透明黑色背景
    
    // 创建菜单面板
    QWidget* menu_panel = new QWidget(pause_menu);
    menu_panel->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(50, 50, 50, 220);"
        "    border: 2px solid #888;"
        "    border-radius: 10px;"
        "}"
    );
    menu_panel->setFixedSize(300, 250);
    
    // 创建标题
    QLabel* title_label = new QLabel("游戏暂停", menu_panel);
    title_label->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "    border: none;"
        "}"
    );
    title_label->setAlignment(Qt::AlignCenter);
    
    // 创建按钮
    resume_button = new QPushButton("继续游戏", menu_panel);
    restart_button = new QPushButton("重新开始", menu_panel);
    main_menu_button = new QPushButton("返回主菜单", menu_panel);
    
    // 设置按钮样式
    QString button_style = 
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px;"
        "    font-size: 16px;"
        "    border-radius: 5px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}";
    
    resume_button->setStyleSheet(button_style);
    restart_button->setStyleSheet(button_style.replace("#4CAF50", "#2196F3").replace("#45a049", "#1976D2").replace("#3d8b40", "#1565C0"));
    main_menu_button->setStyleSheet(button_style.replace("#2196F3", "#f44336").replace("#1976D2", "#d32f2f").replace("#1565C0", "#c62828"));
    
    // 连接按钮信号
    connect(resume_button, &QPushButton::clicked, this, &GameScene::resumeGame);
    connect(restart_button, &QPushButton::clicked, this, &GameScene::restartLevel);
    connect(main_menu_button, &QPushButton::clicked, [this]() {
        resumeGame();
        emit backToMainMenu();
    });
    
    // 布局
    QVBoxLayout* panel_layout = new QVBoxLayout(menu_panel);
    panel_layout->addWidget(title_label);
    panel_layout->addSpacing(20);
    panel_layout->addWidget(resume_button);
    panel_layout->addSpacing(10);
    panel_layout->addWidget(restart_button);
    panel_layout->addSpacing(10);
    panel_layout->addWidget(main_menu_button);
    panel_layout->setAlignment(Qt::AlignCenter);
    
    // 将菜单面板居中放置
    QVBoxLayout* main_layout = new QVBoxLayout(pause_menu);
    main_layout->addWidget(menu_panel, 0, Qt::AlignCenter);
    
    pause_menu->hide();
}

void GameScene::showPauseMenu()
{
    if (pause_menu) {
        // 调整暂停菜单大小以覆盖整个游戏窗口
        pause_menu->resize(this->size());
        pause_menu->show();
        pause_menu->raise(); // 确保在最上层
    }
}

void GameScene::hidePauseMenu()
{
    if (pause_menu) {
        pause_menu->hide();
    }
}

void GameScene::resetLevel()
{
    // 停止定时器
    Timer.stop();
    
    // 清理所有胜利界面相关的子控件
    QList<QWidget*> childWidgets = this->findChildren<QWidget*>();
    for (QWidget* child : childWidgets) {
        if (child == pause_menu || (pause_menu && pause_menu->isAncestorOf(child))) {
            continue;
        }
        if (child != this && child->parent() == this) {
            // 检查是否是胜利界面相关的控件
            if (child->styleSheet().contains("rgba(0, 0, 0, 180)") || 
                child->styleSheet().contains("rgba(50, 50, 50, 220)")) {
                child->deleteLater();
            }
        }
    }
    
    // 清空已收集物品
    collected_items.clear();
    
    // 清空箭矢
    projectiles.clear();

    // +++ 新增：清空残影
    afterimages.clear();
    lastAfterimageTime = 0;

    // 重置移动平台和开关门状态
    moving_platforms.clear();
    switch_doors.clear();
    
    // 重置游戏状态
    begin = false;
    leftpress = false;
    rightpress = false;
    is_dead = false;
    tick_counter = 0;
    
    // 重置水域状态和移动速度
    is_in_water = false;
    pl.resetMoveSpeed();
    
    // 重置玩家按键状态
    pl.resetKeyStates();
    
    // 重置玩家位置和目标进度（如果有当前关卡数据）
    if (current_level_data) {
        QPointF startPos = current_level_data->getPlayerStartPosition();
        pl.x = startPos.x();
        pl.y = startPos.y();
        pl.vx = 0;
        pl.vy = 0;
        
        // 重置关卡目标进度
        current_level_data->resetObjectiveProgress();
    }
    qDebug() << "关卡状态已重置，包括胜利界面";
}

void GameScene::restartLevel()
{
    resumeGame();
    resetLevel();
    gameStart();
    if (current_level_data) {
        if (current_level_data->isCustomLevel()) {
            loadLevelFromFile(current_level_data->getFilePath());
        } else {
            loadLevelInternal(LevelManager::getInstance().getCurrentLevelIndex());
        }
    }
}

void GameScene::gamewin()
{
    // 停止游戏定时器
    Timer.stop();
    
    // 标记当前关卡为已完成
    int currentLevelIndex = LevelManager::getInstance().getCurrentLevelIndex();
    LevelManager::getInstance().markLevelCompleted(currentLevelIndex);
    
    // 保存游戏进度并解锁下一关
    saveGameProgress(currentLevelIndex + 1);  // 传入当前通关的关卡编号（从0基索引转换为1基编号）
    
    // 创建胜利界面背景
    QWidget* winWidget = new QWidget(this);
    winWidget->setGeometry(0, 0, XSIZE, YSIZE);
    winWidget->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    
    // 创建胜利信息容器
    QWidget* winContainer = new QWidget(winWidget);
    winContainer->setGeometry(XSIZE/2 - 200, YSIZE/2 - 150, 400, 300);
    winContainer->setStyleSheet("background-color: rgba(50, 50, 50, 220); border: 3px solid gold; border-radius: 15px;");
    
    QVBoxLayout* layout = new QVBoxLayout(winContainer);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // 胜利标题
    QLabel* titleLabel = new QLabel("🎉 恭喜通关！ 🎉", winContainer);
    titleLabel->setStyleSheet("color: gold; font-size: 28px; font-weight: bold; border: none;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // 关卡信息
    QString levelInfo = QString("关卡 %1 完成").arg(currentLevelIndex + 1);
    QLabel* levelLabel = new QLabel(levelInfo, winContainer);
    levelLabel->setStyleSheet("color: white; font-size: 18px; border: none;");
    levelLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(levelLabel);
    
    // 用时显示
    qint64 elapsedMs = level_timer.elapsed();
    double elapsedSeconds = elapsedMs / 1000.0;
    QString timeInfo = QString("用时：%1 秒").arg(QString::number(elapsedSeconds, 'f', 2));
    QLabel* timeLabel = new QLabel(timeInfo, winContainer);
    timeLabel->setStyleSheet("color: #FFD700; font-size: 16px; border: none;");
    timeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(timeLabel);
    
    // 按钮容器
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 返回主菜单按钮
    QPushButton* menuButton = new QPushButton("返回主菜单", winContainer);
    menuButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 24px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
    );
    
    // 下一关按钮（如果有下一关）
    QPushButton* nextButton = nullptr;
    if (currentLevelIndex + 1 < LevelManager::getInstance().getLevelCount()) {
        nextButton = new QPushButton("下一关", winContainer);
        nextButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #2196F3;"
            "    color: white;"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "    border: none;"
            "    border-radius: 8px;"
            "    padding: 12px 24px;"
            "    min-width: 120px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #1976D2;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #1565C0;"
            "}"
        );
        
        // 连接下一关按钮
         connect(nextButton, &QPushButton::clicked, [this, currentLevelIndex, winWidget]() {
             winWidget->deleteLater(); // 删除胜利界面
             this->loadLevelInternal(currentLevelIndex + 1);
             this->gameStart(); // 重新开始游戏
         });
        
        buttonLayout->addWidget(nextButton);
    }
    
    buttonLayout->addWidget(menuButton);
    layout->addLayout(buttonLayout);
    
    // 连接返回主菜单按钮
    connect(menuButton, &QPushButton::clicked, [this, winWidget]() {
        winWidget->deleteLater();
        emit gameFinished();
        emit backToMainMenu();
    });
    
    winWidget->show();
    winContainer->show();
    
    qDebug() << "关卡" << currentLevelIndex << "通关成功！";
}

void GameScene::gameover()
{
    Timer.stop();
    
    QWidget* overWidget = new QWidget(this);
    overWidget->setGeometry(0, 0, XSIZE, YSIZE);
    overWidget->setStyleSheet("background-color: rgba(0, 0, 0, 180);");
    overWidget->show();
    
    QWidget* container = new QWidget(overWidget);
    container->setGeometry(XSIZE/2 - 200, YSIZE/2 - 140, 400, 280);
    container->setStyleSheet("background-color: rgba(30, 30, 30, 220); border: 3px solid #ff5555; border-radius: 15px;");
    container->show(); // 确保container显示
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setSpacing(16);
    layout->setContentsMargins(24, 24, 24, 24);
    
    QLabel* titleLabel = new QLabel("💀 你死了", container);
    titleLabel->setStyleSheet("color: #ff7777; font-size: 26px; font-weight: bold; border: none;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    QLabel* tipLabel = new QLabel("小心岩浆和飞箭！", container);
    tipLabel->setStyleSheet("color: white; font-size: 16px; border: none;");
    tipLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tipLabel);
    
    QHBoxLayout* btns = new QHBoxLayout();
    QPushButton* retry = new QPushButton("重试", container);
    retry->setStyleSheet("QPushButton {background-color: #FFA000; color: white; font-size: 16px; font-weight: bold; border: none; border-radius: 8px; padding: 10px 22px;} QPushButton:hover {background-color: #FB8C00;} QPushButton:pressed {background-color: #F57C00;}");
    connect(retry, &QPushButton::clicked, [this, overWidget]() {
        overWidget->deleteLater();
        resetLevel();
        gameStart();
        // 重新加载当前关卡，检查是否为自定义关卡
        if (current_level_data && current_level_data->isCustomLevel()) {
            loadLevelFromFile(current_level_data->getFilePath());
        } else {
            int idx = LevelManager::getInstance().getCurrentLevelIndex();
            loadLevelInternal(idx);
        }
        is_dead = false;
        projectiles.clear();
    });
    btns->addWidget(retry);
    
    QPushButton* menu = new QPushButton("返回主菜单", container);
    menu->setStyleSheet("QPushButton {background-color: #4CAF50; color: white; font-size: 16px; font-weight: bold; border: none; border-radius: 8px; padding: 10px 22px;} QPushButton:hover {background-color: #45a049;} QPushButton:pressed {background-color: #3d8b40;}");
    connect(menu, &QPushButton::clicked, [this, overWidget]() {
        overWidget->deleteLater();
        emit gameFinished();
        emit backToMainMenu();
    });
    btns->addWidget(menu);
    layout->addLayout(btns);
    
    // 死亡动画：标题淡入
    auto* effect = new QGraphicsOpacityEffect(titleLabel);
    titleLabel->setGraphicsEffect(effect);
    auto* opacityAnim = new QPropertyAnimation(effect, "opacity", container);
    opacityAnim->setDuration(500);
    opacityAnim->setStartValue(0.0);
    opacityAnim->setEndValue(1.0);
    opacityAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameScene::showGameMessage(const QString& message, int duration)
{
    // 如果tutorial_label存在，使用它来显示消息
    if (tutorial_label) {
        tutorial_label->setText(message);
        tutorial_label->setStyleSheet(
            "QLabel {"
            "background-color: rgba(255, 255, 0, 200);"
            "color: black;"
            "font-size: 18px;"
            "font-weight: bold;"
            "padding: 10px;"
            "border-radius: 8px;"
            "border: 2px solid orange;"
            "}"
        );
        tutorial_label->show();
        
        // 设置定时器自动隐藏消息
        QTimer::singleShot(duration, [this]() {
            if (tutorial_label) {
                tutorial_label->hide();
            }
        });
    }
}

void GameScene::saveGameProgress(int levelIndex)
{
    // 获取游戏进度保存路径 - 使用智能路径检测
    QString saveDir = getDataDirectory();
    QDir dir;
    if (!dir.exists(saveDir)) {
        dir.mkpath(saveDir);
    }
    
    QString saveFilePath = saveDir + "/game_progress.json";
    
    // 先加载现有进度
    QJsonObject saveObject = loadGameProgress();
    
    // 更新通关关卡列表
    QJsonArray unlockedLevels = saveObject["unlockedLevels"].toArray();
    
    // 确保当前关卡在已解锁列表中
    bool levelExists = false;
    for (const QJsonValue& value : unlockedLevels) {
        if (value.toInt() == levelIndex) {
            levelExists = true;
            break;
        }
    }
    
    if (!levelExists) {
        unlockedLevels.append(levelIndex);
    }
    
    // 解锁下一关（如果存在）
    int nextLevel = levelIndex + 1;
    bool nextLevelExists = false;
    for (const QJsonValue& value : unlockedLevels) {
        if (value.toInt() == nextLevel) {
            nextLevelExists = true;
            break;
        }
    }
    
    if (!nextLevelExists) {
        unlockedLevels.append(nextLevel);
        qDebug() << "解锁下一关：" << nextLevel;
    }
    
    // 更新保存对象
    saveObject["unlockedLevels"] = unlockedLevels;
    saveObject["lastLevel"] = levelIndex;
    saveObject["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // 写入文件
    QJsonDocument saveDoc(saveObject);
    QFile saveFile(saveFilePath);
    
    if (saveFile.open(QIODevice::WriteOnly)) {
        saveFile.write(saveDoc.toJson());
        saveFile.close();
        qDebug() << "游戏进度已保存，关卡：" << levelIndex;
    } else {
        qDebug() << "无法保存游戏进度文件：" << saveFilePath;
    }
}

QJsonObject GameScene::loadGameProgress()
{
    QString saveDir = getDataDirectory();
    QString saveFilePath = saveDir + "/game_progress.json";
    
    QFile saveFile(saveFilePath);
    if (!saveFile.exists()) {
        // 如果文件不存在，创建默认进度（第1关解锁）
        QJsonObject defaultProgress;
        QJsonArray unlockedLevels;
        unlockedLevels.append(1);  // 第1关默认解锁
        defaultProgress["unlockedLevels"] = unlockedLevels;
        defaultProgress["lastLevel"] = 0;
        defaultProgress["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        
        qDebug() << "创建默认游戏进度，第1关解锁";
        return defaultProgress;
    }
    
    if (saveFile.open(QIODevice::ReadOnly)) {
        QByteArray saveData = saveFile.readAll();
        saveFile.close();
        
        QJsonDocument loadDoc = QJsonDocument::fromJson(saveData);
        QJsonObject progress = loadDoc.object();
        
        // 确保有unlockedLevels字段
        if (!progress.contains("unlockedLevels")) {
            QJsonArray unlockedLevels;
            unlockedLevels.append(1);  // 第1关默认解锁
            progress["unlockedLevels"] = unlockedLevels;
        }
        
        return progress;
    } else {
        qDebug() << "无法读取游戏进度文件：" << saveFilePath;
        // 返回默认进度
        QJsonObject defaultProgress;
        QJsonArray unlockedLevels;
        unlockedLevels.append(1);
        defaultProgress["unlockedLevels"] = unlockedLevels;
        defaultProgress["lastLevel"] = 0;
        return defaultProgress;
    }
}

bool GameScene::isLevelUnlocked(int levelIndex)
{
    QJsonObject progress = loadGameProgress();
    QJsonArray unlockedLevels = progress["unlockedLevels"].toArray();
    
    for (const QJsonValue& value : unlockedLevels) {
        if (value.toInt() == levelIndex) {
            return true;
        }
    }
    
    return false;
}

void GameScene::unlockLevel(int levelIndex)
{
    QJsonObject progress = loadGameProgress();
    QJsonArray unlockedLevels = progress["unlockedLevels"].toArray();
    
    // 检查关卡是否已经解锁
    bool alreadyUnlocked = false;
    for (const QJsonValue& value : unlockedLevels) {
        if (value.toInt() == levelIndex) {
            alreadyUnlocked = true;
            break;
        }
    }
    
    if (!alreadyUnlocked) {
        unlockedLevels.append(levelIndex);
        progress["unlockedLevels"] = unlockedLevels;
        
        // 保存更新后的进度
        QString saveDir = getDataDirectory();
        QDir dir;
        if (!dir.exists(saveDir)) {
            dir.mkpath(saveDir);
        }
        
        QString saveFilePath = saveDir + "/game_progress.json";
        QJsonDocument saveDoc(progress);
        QFile saveFile(saveFilePath);
        
        if (saveFile.open(QIODevice::WriteOnly)) {
            saveFile.write(saveDoc.toJson());
            saveFile.close();
            qDebug() << "关卡" << levelIndex << "已解锁";
        } else {
            qDebug() << "无法保存关卡解锁进度";
        }
    }
}

// === 移动平台和开关门方法实现 ===

void GameScene::initializeMovingPlatforms()
{
    moving_platforms.clear();
    
    if (!current_level_data) return;
    
    const auto& elements = current_level_data->getGameElements();
    for (int i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];
        
        if (element.element_type == GameElementType::HorizontalPlatform ||
            element.element_type == GameElementType::VerticalPlatform) {
            
            MovingPlatformState platform;
            platform.element_index = i;
            platform.current_pos = element.position;
            platform.start_pos = element.position;
            platform.moving_to_end = true;
            
            // 从属性中获取终点位置
            if (element.properties.contains("end_x") && element.properties.contains("end_y")) {
                platform.end_pos = QPointF(
                    element.properties["end_x"].toDouble(),
                    element.properties["end_y"].toDouble()
                );
            } else {
                // 默认终点位置（如果没有指定）
                if (element.element_type == GameElementType::HorizontalPlatform) {
                    platform.end_pos = element.position + QPointF(B0 * 3, 0); // 向右移动3格
                } else {
                    platform.end_pos = element.position + QPointF(0, B0 * 3); // 向下移动3格
                }
            }
            
            // 计算移动速度（每秒移动1格）
            QPointF direction = platform.end_pos - platform.start_pos;
            float distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
            if (distance > 0) {
                float speed = B0 / 60.0f; // 每帧移动的距离（60FPS）
                platform.velocity = direction * (speed / distance);
            } else {
                platform.velocity = QPointF(0, 0);
            }
            
            moving_platforms.append(platform);
        }
    }
}

void GameScene::updateMovingPlatforms()
{
    for (auto& platform : moving_platforms) {
        if (platform.velocity.x() == 0 && platform.velocity.y() == 0) continue;
        
        // 更新位置
        platform.current_pos += platform.velocity;
        
        // 检查是否到达终点
        QPointF target = platform.moving_to_end ? platform.end_pos : platform.start_pos;
        QPointF diff = target - platform.current_pos;
        
        // 如果接近目标点，切换方向
        if (abs(diff.x()) < 2.0f && abs(diff.y()) < 2.0f) {
            platform.current_pos = target;
            platform.moving_to_end = !platform.moving_to_end;
            platform.velocity = -platform.velocity;
        }
    }
}

void GameScene::initializeSwitchDoors()
{
    switch_doors.clear();
    
    if (!current_level_data) return;
    
    const auto& elements = current_level_data->getGameElements();
    
    // 查找所有开关和门的配对
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i].element_type == GameElementType::Switch) {
            // 查找对应的门
            for (int j = 0; j < elements.size(); ++j) {
                if (elements[j].element_type == GameElementType::Door) {
                    // 检查是否有配对信息
                    QString switchId = elements[i].properties.value("switch_id").toString();
                    if (switchId.isEmpty()) switchId = QString::number(i);
                    QString doorId = elements[j].properties.value("door_id").toString();
                    if (doorId.isEmpty()) doorId = QString::number(j);
                    
                    if (switchId == doorId || 
                        (elements[i].properties.contains("paired_door") && 
                         elements[i].properties["paired_door"].toInt() == j)) {
                        
                        SwitchDoorState switchDoor;
                        switchDoor.switch_element_index = i;
                        switchDoor.door_element_index = j;
                        switchDoor.is_activated = false;
                        switchDoor.door_is_open = false;
                        
                        switch_doors.append(switchDoor);
                        break;
                    }
                }
            }
        }
    }
}

void GameScene::checkSwitchCollisions()
{
    if (!current_level_data) return;
    
    const auto& elements = current_level_data->getGameElements();
    QRectF playerRect(pl.x, pl.y, pl.w, pl.h);
    
    for (auto& switchDoor : switch_doors) {
        if (switchDoor.is_activated) continue;
        
        const auto& switchElement = elements[switchDoor.switch_element_index];
        QRectF switchRect(switchElement.position.x(), switchElement.position.y(),
                         switchElement.size.x(), switchElement.size.y());
        
        if (playerRect.intersects(switchRect)) {
            activateSwitch(switch_doors.indexOf(switchDoor));
        }
    }
}

void GameScene::checkMovingPlatformCollisions()
{
    if (!current_level_data) return;

    QRectF playerRect(pl.x, pl.y, pl.w, pl.h);
    bool isSupported = false; // 标记玩家本帧是否被任何平面支撑

    // 默认玩家不在移动平台上，除非检测到
    pl.onMovingPlatform = false;

    for (int i = 0; i < moving_platforms.size(); ++i) {
        const auto& platform = moving_platforms[i];
        const auto& element = current_level_data->getGameElements()[platform.element_index];
        QRectF platformRect(platform.current_pos.x(), platform.current_pos.y(), element.size.x(), element.size.y());

        // 优化检测条件：只关心玩家是否在平台上方，并且即将或正在接触
        bool isHorizontallyAligned = playerRect.right() > platformRect.left() && playerRect.left() < platformRect.right();
        qreal vertical_tolerance = 5.0;
        bool isVerticallyClose = (pl.y + pl.h) >= platformRect.top() && (pl.y + pl.h) <= (platformRect.top() + vertical_tolerance);

        if (isHorizontallyAligned && isVerticallyClose) {
            // 玩家在平台上方
            pl.y = platformRect.top() - pl.h; // 精确地将玩家放在平台表面
            
            // 关键修复：只有当玩家向下运动或静止时才重置跳跃状态
            // 这样可以保证玩家的跳跃意图不被打断
            if (pl.v0 >= 0) {  // 向下运动或静止
                pl.v0 = 0;
                pl.isJump = false;
            }
            // 如果玩家正在向上跳跃（v0 < 0），不重置跳跃状态，让玩家继续跳跃
            
            isSupported = true; // 玩家被支撑
            pl.onMovingPlatform = true; // 确认在移动平台上
            pl.onGround = true; // 强制设置onGround为true，确保可以跳跃

            pl.resetAirDash(); // +++ 新增：在移动平台落地时，重置空中冲刺

            // 如果这是玩家新接触的平台，或者平台索引变了，则更新相对位置
            if (pl.currentPlatformIndex != i) {
                pl.currentPlatformIndex = i;
                pl.platformRelativeX = pl.x - platform.current_pos.x();
                pl.platformRelativeY = pl.y - (platform.current_pos.y() - pl.h);
            }
            
            // 既然已经找到了支撑平台，就没必要再检查其他移动平台了
            break; 
        }
        // （可选）在这里可以补充处理水平碰撞和头部碰撞的逻辑
    }

    // 如果没有被任何移动平台支撑，则检查静态地面
    if (!isSupported) {
        pl.currentPlatformIndex = -1; // 不在任何移动平台上
        if (pl.is_ground()) {
            isSupported = true;
        }
    }

    // 最终根据isSupported状态更新onGround
    pl.onGround = isSupported;
}

void GameScene::activateSwitch(int switch_index)
{
    if (switch_index < 0 || switch_index >= switch_doors.size()) return;
    
    auto& switchDoor = switch_doors[switch_index];
    if (switchDoor.is_activated) return;
    
    switchDoor.is_activated = true;
    switchDoor.door_is_open = true;
    
    // 可以在这里添加音效或视觉效果
    qDebug() << "Switch activated! Door opened.";
}

bool GameScene::checkDoorCollision(const QRectF& playerRect)
{
    if (!current_level_data) return false;
    
    const auto& elements = current_level_data->getGameElements();
    
    for (const auto& switchDoor : switch_doors) {
        // 只检查关闭的门
        if (switchDoor.door_is_open) continue;
        
        const auto& doorElement = elements[switchDoor.door_element_index];
        QRectF doorRect(doorElement.position.x(), doorElement.position.y(),
                       doorElement.size.x(), doorElement.size.y());
        
        if (playerRect.intersects(doorRect)) {
            return true; // 与关闭的门发生碰撞
        }
    }
    
    return false; // 没有碰撞
}

void GameScene::handlePlatformFollowing()
{
    // 如果玩家不在移动平台上，重置相关状态
    if (!pl.onMovingPlatform) {
        pl.currentPlatformIndex = -1;
        pl.platformRelativeX = 0.0;
        pl.platformRelativeY = 0.0;
        return;
    }
    
    // 如果玩家在移动平台上，处理相对位置逻辑
    if (pl.currentPlatformIndex >= 0 && pl.currentPlatformIndex < moving_platforms.size()) {
        const auto& platform = moving_platforms[pl.currentPlatformIndex];
        const auto& element = current_level_data->getGameElements()[platform.element_index];

        // 检查玩家是否在主动移动
        bool isActivelyMoving = pl.getLeftPressed() || pl.getRightPressed();
        bool isActivelyJumping = pl.isJump && pl.v0 < 0; // 正在向上跳跃

        // 处理水平跟随
        if (isActivelyMoving) {
            // 玩家主动移动时，更新相对位置
            pl.platformRelativeX = pl.x - platform.current_pos.x();
        } else {
            // 玩家没有主动移动时，保持相对位置不变
            // 根据平台的当前位置和相对位置计算玩家的绝对位置
            double newPlayerX = platform.current_pos.x() + pl.platformRelativeX;

            // 检查边界，确保玩家不会超出平台范围太远
            double platformLeft = platform.current_pos.x();
            double platformRight = platform.current_pos.x() + element.size.x();
            double playerLeft = newPlayerX;
            double playerRight = newPlayerX + pl.w;

            // 如果玩家完全在平台范围内或者有合理的重叠，则跟随平台
            if (playerRight > platformLeft && playerLeft < platformRight) {
                pl.x = newPlayerX;
            } else {
                // 玩家离开平台范围，停止跟随
                pl.onMovingPlatform = false;
                pl.currentPlatformIndex = -1;
                pl.platformRelativeX = 0.0;
                pl.platformRelativeY = 0.0;
                return;
            }
        }

        // 处理垂直跟随：与水平跟随逻辑保持一致
        if (isActivelyJumping) {
            // 玩家主动跳跃时，更新相对位置
            pl.platformRelativeY = pl.y - (platform.current_pos.y() - pl.h);
        } else {
            // 玩家没有主动跳跃时，保持相对位置不变
            // 根据平台的当前位置和相对位置计算玩家的绝对位置
            double newPlayerY = (platform.current_pos.y() - pl.h) + pl.platformRelativeY;

            // 检查边界，确保玩家不会超出平台范围太远
            double platformTop = platform.current_pos.y();
            double platformBottom = platform.current_pos.y() + element.size.y();
            double playerTop = newPlayerY;
            double playerBottom = newPlayerY + pl.h;

            // 如果玩家完全在平台范围内或者有合理的重叠，则跟随平台
            if (playerBottom > platformTop && playerTop < platformBottom) {
                pl.y = newPlayerY;
            } else {
                // 玩家离开平台范围，停止跟随
                pl.onMovingPlatform = false;
                pl.currentPlatformIndex = -1;
                pl.platformRelativeX = 0.0;
                pl.platformRelativeY = 0.0;
                return;
            }
        }
    }
}

// +++ 新增：实现更新残影的函数 (放在 GameScene.cpp 的末尾)
void GameScene::updateAfterimages()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    // 1. 移除过期的残影
    afterimages.erase(std::remove_if(afterimages.begin(), afterimages.end(),
                                     [now, this](const Afterimage& img){
                                         // 如果残影存在时间超过了设定的LIFETIME，则移除
                                         return (now - img.spawnTime) > AFTERIMAGE_LIFETIME;
                                     }), afterimages.end());

    // 2. 如果玩家正在冲刺，则按间隔添加新的残影
    if (pl.getIsDashing()) {
        if (now - lastAfterimageTime > AFTERIMAGE_INTERVAL) {
            Afterimage newImg;
            newImg.frame = pl.getCurrentAnimationFrame(); // 捕捉当前动画帧
            newImg.rect = QRectF(pl.x, pl.y, pl.w, pl.h); // 捕捉当前位置
            newImg.spawnTime = now;

            afterimages.append(newImg); // 添加到列表中
            lastAfterimageTime = now;   // 更新上次生成时间
        }
    }
}
