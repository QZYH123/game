/**
 * @file LevelData.cpp
 * @brief 关卡数据管理类实现
 * @author 开发团队
 * @date 2025-10-15
 */

#include "LevelData.h"
#include "Config.h"

LevelData::LevelData(int width, int height)
    : level_width(width)
    , level_height(height)
    , level_name("未命名关卡")
    , level_description("暂无描述")
    , player_start_position(X, Y)  // 使用Config.h中的默认值
{
    initializeGrid();
}

void LevelData::initializeGrid()
{
    // 初始化网格为空
    level_grid.resize(level_height);
    for (int i = 0; i < level_height; ++i) {
        level_grid[i].resize(level_width);
        for (int j = 0; j < level_width; ++j) {
            level_grid[i][j] = GameElementType::Empty;
        }
    }
}

bool LevelData::isValidCoordinate(int x, int y) const
{
    return x >= 0 && x < level_width && y >= 0 && y < level_height;
}

GameElementType LevelData::getElementAt(int x, int y) const
{
    if (!isValidCoordinate(x, y)) {
        return GameElementType::Empty;
    }
    return level_grid[y][x];
}

void LevelData::setElementAt(int x, int y, GameElementType type)
{
    if (!isValidCoordinate(x, y)) {
        qDebug() << "警告：尝试设置无效坐标的元素：" << x << "," << y;
        return;
    }
    level_grid[y][x] = type;
}

void LevelData::addGameElement(const GameElement& element)
{
    game_elements.append(element);
    
    // 同时更新网格数据
    int gridX = static_cast<int>(element.position.x() / B0);
    int gridY = static_cast<int>(element.position.y() / B0);
    setElementAt(gridX, gridY, element.element_type);
}

void LevelData::addObjective(const LevelObjective& objective)
{
    level_objectives.append(objective);
}

void LevelData::updateObjectiveProgress(const QString& objectiveType, int increment)
{
    for (auto& objective : level_objectives) {
        if (objective.objective_type == objectiveType) {
            objective.current_count += increment;
            qDebug() << "目标进度更新：" << objectiveType 
                     << objective.current_count << "/" << objective.target_count;
            break;
        }
    }
}

bool LevelData::areAllObjectivesCompleted() const
{
    for (const auto& objective : level_objectives) {
        if (!objective.isCompleted()) {
            return false;
        }
    }
    return !level_objectives.isEmpty(); // 至少要有一个目标
}

void LevelData::resetObjectiveProgress()
{
    for (auto& objective : level_objectives) {
        objective.current_count = 0;
        qDebug() << "重置目标进度：" << objective.objective_type;
    }
}

bool LevelData::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开关卡文件：" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "关卡文件格式错误：" << filePath;
        return false;
    }
    
    return loadFromJson(doc.object());
}

bool LevelData::saveToFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建关卡文件：" << filePath;
        return false;
    }
    
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    return true;
}

bool LevelData::loadFromJson(const QJsonObject& jsonObj)
{
    // 加载基本信息
    level_name = jsonObj["name"].toString("未命名关卡");
    level_description = jsonObj["description"].toString("暂无描述");
    level_width = jsonObj["width"].toInt(GRID_WIDTH);
    level_height = jsonObj["height"].toInt(GRID_HEIGHT);
    
    // 重新初始化网格
    initializeGrid();
    
    // 加载玩家起始位置
    QJsonObject startPos = jsonObj["playerStart"].toObject();
    player_start_position = QPointF(
        startPos["x"].toDouble(X),
        startPos["y"].toDouble(Y)
    );
    
    // 先尝试从grid数组加载实心方块网格（推荐格式）
    QJsonArray gridArray = jsonObj["grid"].toArray();
    if (!gridArray.isEmpty()) {
        for (int y = 0; y < gridArray.size() && y < level_height; ++y) {
            QJsonArray row = gridArray[y].toArray();
            for (int x = 0; x < row.size() && x < level_width; ++x) {
                int v = row[x].toInt(0);
                if (v == 1) {
                    setElementAt(x, y, GameElementType::SolidBlock);
                }
            }
        }
    }
    
    // 加载游戏元素
    game_elements.clear();
    QJsonArray elementsArray = jsonObj["elements"].toArray();
    for (const auto& value : elementsArray) {
        QJsonObject elemObj = value.toObject();
        
        GameElement element;
        element.element_type = static_cast<GameElementType>(elemObj["type"].toInt());
        element.position = QPointF(
            elemObj["x"].toDouble(),
            elemObj["y"].toDouble()
        );
        element.size = QPointF(
            elemObj["width"].toDouble(B0),
            elemObj["height"].toDouble(B0)
        );
        element.texture_path = elemObj["texture"].toString();
        element.properties = elemObj["properties"].toObject();
        
        addGameElement(element);
        
        // 兼容旧格式：若元素类型是实心方块，则同时更新网格
        if (element.element_type == GameElementType::SolidBlock) {
            int gridX = static_cast<int>(element.position.x() / B0);
            int gridY = static_cast<int>(element.position.y() / B0);
            if (isValidCoordinate(gridX, gridY)) {
                setElementAt(gridX, gridY, GameElementType::SolidBlock);
            }
        }
    }
    
    // 加载关卡目标
    level_objectives.clear();
    QJsonArray objectivesArray = jsonObj["objectives"].toArray();
    for (const auto& value : objectivesArray) {
        QJsonObject objObj = value.toObject();
        
        LevelObjective objective;
        objective.objective_type = objObj["type"].toString();
        objective.target_count = objObj["targetCount"].toInt();
        objective.current_count = objObj["currentCount"].toInt(0);
        objective.description = objObj["description"].toString();
        
        addObjective(objective);
    }
    
    return true;
}

QJsonObject LevelData::toJson() const
{
    QJsonObject jsonObj;
    
    // 基本信息
    jsonObj["name"] = level_name;
    jsonObj["description"] = level_description;
    jsonObj["width"] = level_width;
    jsonObj["height"] = level_height;
    
    // 玩家起始位置
    QJsonObject startPos;
    startPos["x"] = player_start_position.x();
    startPos["y"] = player_start_position.y();
    jsonObj["playerStart"] = startPos;
    
    // 序列化网格（24x24或关卡宽高）
    QJsonArray gridArray;
    for (int y = 0; y < level_height; ++y) {
        QJsonArray row;
        for (int x = 0; x < level_width; ++x) {
            int v = (level_grid[y][x] == GameElementType::SolidBlock) ? 1 : 0;
            row.append(v);
        }
        gridArray.append(row);
    }
    jsonObj["grid"] = gridArray;
    
    // 游戏元素
    QJsonArray elementsArray;
    for (const auto& element : game_elements) {
        QJsonObject elemObj;
        elemObj["type"] = static_cast<int>(element.element_type);
        elemObj["x"] = element.position.x();
        elemObj["y"] = element.position.y();
        elemObj["width"] = element.size.x();
        elemObj["height"] = element.size.y();
        elemObj["texture"] = element.texture_path;
        elemObj["properties"] = element.properties;
        
        elementsArray.append(elemObj);
    }
    jsonObj["elements"] = elementsArray;
    
    // 关卡目标
    QJsonArray objectivesArray;
    for (const auto& objective : level_objectives) {
        QJsonObject objObj;
        objObj["type"] = objective.objective_type;
        objObj["targetCount"] = objective.target_count;
        objObj["currentCount"] = objective.current_count;
        objObj["description"] = objective.description;
        
        objectivesArray.append(objObj);
    }
    jsonObj["objectives"] = objectivesArray;
    
    return jsonObj;
}

void LevelData::getLegacyMapArray(int mapArray[24][24]) const
{
    // 清空数组
    for (int i = 0; i < 24; ++i) {
        for (int j = 0; j < 24; ++j) {
            mapArray[i][j] = 0;
        }
    }
    
    // 填充数据（注意坐标转换：level_grid[y][x] -> mapArray[x][y]）
    for (int y = 0; y < qMin(level_height, 24); ++y) {
        for (int x = 0; x < qMin(level_width, 24); ++x) {
            GameElementType type = level_grid[y][x];
            if (type == GameElementType::SolidBlock) {
                mapArray[x][y] = 1;
            } else {
                mapArray[x][y] = 0;
            }
        }
    }
}

void LevelData::setFromLegacyMapArray(const int mapArray[24][24])
{
    // 清空现有数据
    initializeGrid();
    
    // 从数组填充数据（注意坐标转换：mapArray[x][y] -> level_grid[y][x]）
    for (int x = 0; x < 24; ++x) {
        for (int y = 0; y < 24; ++y) {
            if (x < level_width && y < level_height) {
                if (mapArray[x][y] == 1) {
                    level_grid[y][x] = GameElementType::SolidBlock;
                } else {
                    level_grid[y][x] = GameElementType::Empty;
                }
            }
        }
    }
}

void LevelData::getMapArray(int** mapArray, int width, int height) const
{
    // 填充数据（注意坐标转换：level_grid[y][x] -> mapArray[x][y]）
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (x < level_width && y < level_height) {
                GameElementType type = level_grid[y][x];
                if (type == GameElementType::SolidBlock) {
                    mapArray[x][y] = 1;
                } else {
                    mapArray[x][y] = 0;
                }
            } else {
                mapArray[x][y] = 0;
            }
        }
    }
}

void LevelData::removeElementsAt(int grid_x, int grid_y)
{
    if (!isValidCoordinate(grid_x, grid_y)) {
        qDebug() << "警告：尝试删除无效坐标的元素：" << grid_x << "," << grid_y;
        return;
    }
    // 清空网格对应类型
    level_grid[grid_y][grid_x] = GameElementType::Empty;
    // 过滤掉位于该格子的所有元素
    QVector<GameElement> remaining;
    remaining.reserve(game_elements.size());
    for (const auto& el : game_elements) {
        int ex = static_cast<int>(el.position.x() / B0);
        int ey = static_cast<int>(el.position.y() / B0);
        if (!(ex == grid_x && ey == grid_y)) {
            remaining.append(el);
        }
    }
    game_elements = remaining;
}