/**
 * @file LevelManager.cpp
 * @brief 关卡管理器实现
 * @author 开发团队
 * @date 2024-12-15
 */

#include "LevelManager.h"
#include "Config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QApplication>

LevelManager& LevelManager::getInstance()
{
    static LevelManager instance;
    return instance;
}

LevelManager::LevelManager(QObject* parent)
    : QObject(parent)
    , current_level_index(0)
{
    // 设置关卡文件目录 - 使用智能路径检测
    QString dataDir = getDataDirectory();
    levels_directory = dataDir + "/levels";
    progress_file_path = dataDir + "/progress.json";
    qDebug() << "关卡目录路径:" << levels_directory;
}

bool LevelManager::initialize()
{
    qDebug() << "初始化关卡管理器...";
    
    // 初始化目录结构
    if (!initializeDirectories()) {
        qDebug() << "初始化目录失败";
        return false;
    }
    
    // 尝试加载现有关卡
    if (!loadAllLevels()) {
        qDebug() << "加载关卡失败，创建默认关卡";
        createDefaultLevels();
    }
    
    // 加载游戏进度
    loadProgress();
    
    // 确保至少有一个关卡解锁
    if (level_unlocked_status.isEmpty() || !level_unlocked_status[0]) {
        unlockLevel(0);
    }
    
    qDebug() << "关卡管理器初始化完成，共" << getLevelCount() << "个关卡";
    return true;
}

bool LevelManager::initializeDirectories()
{
    QDir dir;
    if (!dir.exists(levels_directory)) {
        if (!dir.mkpath(levels_directory)) {
            qDebug() << "无法创建关卡目录：" << levels_directory;
            return false;
        }
    }
    
    // 确保进度文件目录存在
    QString progressDir = QFileInfo(progress_file_path).absolutePath();
    if (!dir.exists(progressDir)) {
        if (!dir.mkpath(progressDir)) {
            qDebug() << "无法创建进度文件目录：" << progressDir;
            return false;
        }
    }
    
    return true;
}

bool LevelManager::isValidLevelIndex(int levelIndex) const
{
    return levelIndex >= 0 && levelIndex < level_data_list.size();
}

LevelData* LevelManager::getLevelData(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        qDebug() << "无效的关卡索引：" << levelIndex;
        return nullptr;
    }
    return level_data_list[levelIndex];
}

bool LevelManager::setCurrentLevel(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        qDebug() << "无法设置当前关卡，无效索引：" << levelIndex;
        return false;
    }
    
    if (current_level_index != levelIndex) {
        current_level_index = levelIndex;
        emit currentLevelChanged(levelIndex);
        qDebug() << "当前关卡设置为：" << levelIndex;
    }
    
    return true;
}

bool LevelManager::isLevelUnlocked(int levelIndex) const
{
    if (!isValidLevelIndex(levelIndex)) {
        return false;
    }
    
    if (levelIndex >= level_unlocked_status.size()) {
        return false;
    }
    
    return level_unlocked_status[levelIndex];
}

void LevelManager::unlockLevel(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        return;
    }
    
    // 确保解锁状态数组足够大
    while (level_unlocked_status.size() <= levelIndex) {
        level_unlocked_status.append(false);
    }
    
    if (!level_unlocked_status[levelIndex]) {
        level_unlocked_status[levelIndex] = true;
        emit levelUnlockStatusChanged(levelIndex, true);
        qDebug() << "关卡" << levelIndex << "已解锁";
        
        // 保存进度
        saveProgress();
    }
}

void LevelManager::markLevelCompleted(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        return;
    }
    
    // 确保完成状态数组足够大
    while (level_completed_status.size() <= levelIndex) {
        level_completed_status.append(false);
    }
    
    if (!level_completed_status[levelIndex]) {
        level_completed_status[levelIndex] = true;
        qDebug() << "关卡" << levelIndex << "已完成";
        
        // 自动解锁下一关
        if (levelIndex + 1 < getLevelCount()) {
            unlockLevel(levelIndex + 1);
        }
        
        // 保存进度
        saveProgress();
    }
}

bool LevelManager::isLevelCompleted(int levelIndex) const
{
    if (!isValidLevelIndex(levelIndex)) {
        return false;
    }
    
    if (levelIndex >= level_completed_status.size()) {
        return false;
    }
    
    return level_completed_status[levelIndex];
}

QString LevelManager::generateLevelFileName(int levelIndex) const
{
    return QString("level_%1.json").arg(levelIndex, 3, 10, QChar('0'));
}

QString LevelManager::getLevelFilePath(int levelIndex) const
{
    return levels_directory + "/" + generateLevelFileName(levelIndex);
}

LevelData* LevelManager::loadLevelFromFile(const QString& filePath)
{
    LevelData* levelData = new LevelData();
    
    if (levelData->loadFromFile(filePath)) {
        qDebug() << "从文件加载关卡成功：" << filePath;
        return levelData;
    } else {
        delete levelData;
        qDebug() << "从文件加载关卡失败：" << filePath;
        return nullptr;
    }
}

bool LevelManager::loadAllLevels()
{
    // 清空现有数据
    qDeleteAll(level_data_list);
    level_data_list.clear();
    
    QDir levelsDir(levels_directory);
    QStringList levelFiles = levelsDir.entryList(QStringList() << "level_*.json" << "tutorial_level.json", QDir::Files, QDir::Name);
    
    if (levelFiles.isEmpty()) {
        qDebug() << "未找到关卡文件";
        return false;
    }
    
    // 按文件名排序加载
    for (const QString& fileName : levelFiles) {
        QString filePath = levelsDir.absoluteFilePath(fileName);
        LevelData* levelData = loadLevelFromFile(filePath);
        
        if (levelData) {
            level_data_list.append(levelData);
            qDebug() << "加载关卡：" << levelData->getLevelName();
        }
    }
    
    return !level_data_list.isEmpty();
}

bool LevelManager::saveLevelData(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        return false;
    }
    
    QString filePath = getLevelFilePath(levelIndex);
    bool success = level_data_list[levelIndex]->saveToFile(filePath);
    
    if (success) {
        emit levelDataChanged(levelIndex);
        qDebug() << "保存关卡" << levelIndex << "成功";
    } else {
        qDebug() << "保存关卡" << levelIndex << "失败";
    }
    
    return success;
}

bool LevelManager::saveAllLevels()
{
    bool allSuccess = true;
    
    for (int i = 0; i < level_data_list.size(); ++i) {
        if (!saveLevelData(i)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool LevelManager::saveProgress()
{
    QJsonObject progressObj;
    progressObj["currentLevel"] = current_level_index;
    
    // 保存解锁状态
    QJsonArray unlockedArray;
    for (bool unlocked : level_unlocked_status) {
        unlockedArray.append(unlocked);
    }
    progressObj["unlockedLevels"] = unlockedArray;
    
    // 保存完成状态
    QJsonArray completedArray;
    for (bool completed : level_completed_status) {
        completedArray.append(completed);
    }
    progressObj["completedLevels"] = completedArray;
    
    QFile file(progress_file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法保存进度文件：" << progress_file_path;
        return false;
    }
    
    QJsonDocument doc(progressObj);
    file.write(doc.toJson());
    return true;
}

bool LevelManager::loadProgress()
{
    QFile file(progress_file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "进度文件不存在，使用默认进度";
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        qDebug() << "进度文件格式错误";
        return false;
    }
    
    QJsonObject progressObj = doc.object();
    current_level_index = progressObj["currentLevel"].toInt(0);
    
    // 加载解锁状态
    level_unlocked_status.clear();
    QJsonArray unlockedArray = progressObj["unlockedLevels"].toArray();
    for (const auto& value : unlockedArray) {
        level_unlocked_status.append(value.toBool());
    }
    
    // 加载完成状态
    level_completed_status.clear();
    QJsonArray completedArray = progressObj["completedLevels"].toArray();
    for (const auto& value : completedArray) {
        level_completed_status.append(value.toBool());
    }
    
    qDebug() << "加载游戏进度成功";
    return true;
}

int LevelManager::createNewLevel(const QString& levelName, const QString& description)
{
    LevelData* newLevel = new LevelData();
    newLevel->setLevelName(levelName);
    newLevel->setLevelDescription(description);
    
    int newIndex = level_data_list.size();
    level_data_list.append(newLevel);
    
    // 保存新关卡
    if (saveLevelData(newIndex)) {
        qDebug() << "创建新关卡：" << levelName << "索引：" << newIndex;
        return newIndex;
    } else {
        // 保存失败，移除关卡
        level_data_list.removeLast();
        delete newLevel;
        return -1;
    }
}

bool LevelManager::deleteLevel(int levelIndex)
{
    if (!isValidLevelIndex(levelIndex)) {
        return false;
    }
    
    // 删除文件
    QString filePath = getLevelFilePath(levelIndex);
    QFile::remove(filePath);
    
    // 删除内存中的数据
    delete level_data_list[levelIndex];
    level_data_list.removeAt(levelIndex);
    
    // 调整解锁和完成状态
    if (levelIndex < level_unlocked_status.size()) {
        level_unlocked_status.removeAt(levelIndex);
    }
    if (levelIndex < level_completed_status.size()) {
        level_completed_status.removeAt(levelIndex);
    }
    
    // 调整当前关卡索引
    if (current_level_index >= levelIndex && current_level_index > 0) {
        current_level_index--;
    }
    
    qDebug() << "删除关卡" << levelIndex;
    return true;
}

int LevelManager::duplicateLevel(int sourceIndex, const QString& newName)
{
    if (!isValidLevelIndex(sourceIndex)) {
        return -1;
    }
    
    // 创建新关卡
    LevelData* sourceLevel = level_data_list[sourceIndex];
    LevelData* newLevel = new LevelData();
    
    // 复制数据
    QJsonObject sourceJson = sourceLevel->toJson();
    newLevel->loadFromJson(sourceJson);
    newLevel->setLevelName(newName);
    
    int newIndex = level_data_list.size();
    level_data_list.append(newLevel);
    
    // 保存新关卡
    if (saveLevelData(newIndex)) {
        qDebug() << "复制关卡：" << newName << "索引：" << newIndex;
        return newIndex;
    } else {
        // 保存失败，移除关卡
        level_data_list.removeLast();
        delete newLevel;
        return -1;
    }
}

void LevelManager::createDefaultTutorialLevel()
{
    LevelData* tutorialLevel = new LevelData();
    tutorialLevel->setLevelName("教学关卡：基本移动");
    tutorialLevel->setLevelDescription("学习基本的移动和跳跃操作，收集青菜到达终点！");
    
    // 设置玩家起始位置（在地面上）
    tutorialLevel->setPlayerStartPosition(QPointF(100, 21 * B0 - H));
    
    // 创建简单的平台布局
    // 地面平台（底部两层）
    for (int x = 0; x < GRID_WIDTH; x++) {
        tutorialLevel->setElementAt(x, GRID_HEIGHT-1, GameElementType::SolidBlock);
        tutorialLevel->setElementAt(x, GRID_HEIGHT-2, GameElementType::SolidBlock);
    }
    
    // 起始平台（扩展到更大范围）
    for (int x = 0; x < 8; x++) {
        tutorialLevel->setElementAt(x, 21, GameElementType::SolidBlock);
    }
    
    // 中间跳跃平台
    for (int x = 8; x < 12; x++) {
        tutorialLevel->setElementAt(x, 19, GameElementType::SolidBlock);
    }
    
    // 高台平台
    for (int x = 15; x < 20; x++) {
        tutorialLevel->setElementAt(x, 16, GameElementType::SolidBlock);
    }
    
    // 保存教学关卡到文件
    QString tutorialFilePath = levels_directory + "/tutorial_level.json";
    tutorialLevel->saveToFile(tutorialFilePath);
    
    // 终点平台
    for (int x = GRID_WIDTH-7; x < GRID_WIDTH; x++) {
        tutorialLevel->setElementAt(x, 18, GameElementType::SolidBlock);
    }
    
    // 添加青菜收集目标
    GameElement vegetable1;
    vegetable1.element_type = GameElementType::Vegetable;
    vegetable1.position = QPointF(9 * B0, 18 * B0);
    vegetable1.size = QPointF(B0, B0);
    vegetable1.texture_path = ":/images/vegetable.png"; // 使用资源中的青菜图片
    tutorialLevel->addGameElement(vegetable1);
    
    GameElement vegetable2;
    vegetable2.element_type = GameElementType::Vegetable;
    vegetable2.position = QPointF(17 * B0, 15 * B0);
    vegetable2.size = QPointF(B0, B0);
    vegetable2.texture_path = ":/images/vegetable.png";
    tutorialLevel->addGameElement(vegetable2);
    
    // 添加终点
    GameElement levelExit;
    levelExit.element_type = GameElementType::LevelExit;
    levelExit.position = QPointF(22 * B0, 17 * B0);
    levelExit.size = QPointF(B0, B0);
    levelExit.texture_path = ":/images/door.png"; // 使用门图片作为关卡出口
    tutorialLevel->addGameElement(levelExit);
    
    // 设置关卡目标
    LevelObjective collectVegetables;
    collectVegetables.objective_type = "collect_vegetables";
    collectVegetables.target_count = 2;
    collectVegetables.current_count = 0;
    collectVegetables.description = "收集2个青菜";
    tutorialLevel->addObjective(collectVegetables);
    
    LevelObjective reachExit;
    reachExit.objective_type = "reach_exit";
    reachExit.target_count = 1;
    reachExit.current_count = 0;
    reachExit.description = "到达终点";
    tutorialLevel->addObjective(reachExit);
    
    // 添加到关卡列表
    level_data_list.append(tutorialLevel);
    
    qDebug() << "创建默认教学关卡";
}

void LevelManager::createDefaultLevels()
{
    qDebug() << "创建默认关卡集合";
    
    // 创建教学关卡
    createDefaultTutorialLevel();
    
    // 可以在这里添加更多默认关卡...
    
    // 保存所有关卡
    saveAllLevels();
    
    // 初始化进度状态
    level_unlocked_status.clear();
    level_completed_status.clear();
    
    for (int i = 0; i < level_data_list.size(); ++i) {
        level_unlocked_status.append(i == 0); // 只解锁第一关
        level_completed_status.append(false);
    }
    
    // 保存进度
    saveProgress();
}
