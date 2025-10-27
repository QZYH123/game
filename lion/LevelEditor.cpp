/**
 * @file LevelEditor.cpp
 * @brief 关卡编辑器实现
 * @author 开发团队
 * @date 2024-12-15
 */

#include "LevelEditor.h"
#include <QApplication>

// === LevelEditorCanvas 实现 ===

LevelEditorCanvas::LevelEditorCanvas(QWidget* parent)
    : QWidget(parent)
    , level_data(nullptr)
    , current_element_type(GameElementType::SolidBlock)
    , current_arrow_direction(ArrowDirection::Right)
    , show_grid(true)
    , is_dragging(false)
    , grid_size(B0)
    , canvas_width(GRID_WIDTH * B0)
    , canvas_height(GRID_HEIGHT * B0)
{
    setFixedSize(canvas_width, canvas_height);
    setMouseTracking(true);
    setStyleSheet("background-color: lightblue;");
}

void LevelEditorCanvas::setLevelData(LevelData* levelData)
{
    level_data = levelData;
    update();
}

void LevelEditorCanvas::setCurrentElementType(GameElementType elementType)
{
    current_element_type = elementType;
}

GameElementType LevelEditorCanvas::getCurrentElementType() const
{
    return current_element_type;
}

void LevelEditorCanvas::setShowGrid(bool show)
{
    show_grid = show;
    update();
}

void LevelEditorCanvas::clearCanvas()
{
    if (!level_data) {
        return;
    }
    
    // 清空网格数据
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            level_data->setElementAt(x, y, GameElementType::Empty);
        }
    }
    
    // 清空游戏元素列表
    level_data->clearGameElements();
    
    emit levelDataChanged();
    update();
}

void LevelEditorCanvas::setCurrentArrowDirection(ArrowDirection direction)
{
    current_arrow_direction = direction;
}

QPoint LevelEditorCanvas::screenToGrid(const QPoint& screenPos) const
{
    return QPoint(screenPos.x() / grid_size, screenPos.y() / grid_size);
}

QPoint LevelEditorCanvas::gridToScreen(const QPoint& gridPos) const
{
    return QPoint(gridPos.x() * grid_size, gridPos.y() * grid_size);
}

void LevelEditorCanvas::placeElement(const QPoint& gridPos)
{
    if (!level_data || gridPos.x() < 0 || gridPos.x() >= GRID_WIDTH ||
        gridPos.y() < 0 || gridPos.y() >= GRID_HEIGHT) {
        return;
    }
    
    // 若选择空白，删除该格子中的所有组件
    if (current_element_type == GameElementType::Empty) {
        level_data->removeElementsAt(gridPos.x(), gridPos.y());
        emit levelDataChanged();
        update();
        return;
    }
    
    // 设置网格数据
    level_data->setElementAt(gridPos.x(), gridPos.y(), current_element_type);
    
    // 如果是特殊元素，添加到游戏元素列表
    if (current_element_type != GameElementType::SolidBlock) {
        GameElement element;
        element.element_type = current_element_type;
        element.position = QPointF(gridPos.x() * grid_size, gridPos.y() * grid_size);
        element.size = QPointF(grid_size, grid_size);
        
        // 设置纹理路径或属性
        switch (current_element_type) {
        case GameElementType::Vegetable:
            element.texture_path = ":/vegetables/cabbage.png";
            break;
        case GameElementType::LevelExit:
            element.texture_path = ":/ui/exit_flag.png";
            break;
        case GameElementType::PlayerStart:
            level_data->setPlayerStartPosition(element.position);
            break;
        case GameElementType::ArrowTrap:
            // 根据当前选择的方向设置属性
            switch (current_arrow_direction) {
            case ArrowDirection::Right:
                element.properties["direction"] = "right";
                break;
            case ArrowDirection::Left:
                element.properties["direction"] = "left";
                break;
            case ArrowDirection::Up:
                element.properties["direction"] = "up";
                break;
            case ArrowDirection::Down:
                element.properties["direction"] = "down";
                break;
            }
            element.properties["rate"] = 60; // 默认发射间隔tick
            break;
        case GameElementType::Water:
        case GameElementType::Lava:
            // 纹理可缺省，使用颜色渲染
            break;
        default:
            break;
        }
        
        level_data->addGameElement(element);
    }
    
    emit levelDataChanged();
    emit elementPlaced(current_element_type, QPointF(gridPos.x() * grid_size, gridPos.y() * grid_size));
    update(); // 立即刷新画布
}

void LevelEditorCanvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    painter.fillRect(rect(), QColor(173, 216, 230)); // 浅蓝色背景
    
    // 绘制网格
    if (show_grid) {
        drawGrid(painter);
    }
    
    // 绘制关卡元素
    drawLevelElements(painter);
}

void LevelEditorCanvas::drawGrid(QPainter& painter)
{
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    
    // 绘制垂直线
    for (int x = 0; x <= canvas_width; x += grid_size) {
        painter.drawLine(x, 0, x, canvas_height);
    }
    
    // 绘制水平线
    for (int y = 0; y <= canvas_height; y += grid_size) {
        painter.drawLine(0, y, canvas_width, y);
    }
}

void LevelEditorCanvas::drawLevelElements(QPainter& painter)
{
    if (!level_data) return;
    
    // 绘制网格元素
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            GameElementType elementType = level_data->getElementAt(x, y);
            if (elementType != GameElementType::Empty) {
                QColor color = getElementColor(elementType);
                painter.fillRect(x * grid_size, y * grid_size, grid_size, grid_size, color);
            }
        }
    }
    
    // 绘制特殊元素
    const auto& elements = level_data->getGameElements();
    for (const auto& element : elements) {
        QColor color = getElementColor(element.element_type);
        painter.fillRect(
            static_cast<int>(element.position.x()),
            static_cast<int>(element.position.y()),
            static_cast<int>(element.size.x()),
            static_cast<int>(element.size.y()),
            color
        );
        
        // 添加文字标识
        painter.setPen(Qt::white);
        QString text;
        switch (element.element_type) {
        case GameElementType::Vegetable:
            text = "V";
            break;
        case GameElementType::LevelExit:
            text = "E";
            break;
        case GameElementType::PlayerStart:
            text = "S";
            break;
        case GameElementType::ArrowTrap:
            // 根据方向显示不同的箭头符号
            if (element.properties.contains("direction")) {
                QString direction = element.properties.value("direction").toString();
                if (direction == "right") text = "→";
                else if (direction == "left") text = "←";
                else if (direction == "up") text = "↑";
                else if (direction == "down") text = "↓";
                else text = "A";
            } else {
                text = "A";
            }
            break;
        case GameElementType::Water:
            text = "W";
            break;
        case GameElementType::Lava:
            text = "L";
            break;
        default:
            break;
        }
        if (!text.isEmpty()) {
            painter.drawText(
                static_cast<int>(element.position.x() + element.size.x() / 4),
                static_cast<int>(element.position.y() + element.size.y() * 3 / 4),
                text
            );
        }
    }
    
    // 绘制玩家起始位置
    QPointF startPos = level_data->getPlayerStartPosition();
    painter.fillRect(
        static_cast<int>(startPos.x()),
        static_cast<int>(startPos.y()),
        grid_size, grid_size,
        QColor(255, 255, 0, 128) // 半透明黄色
    );
    painter.setPen(Qt::black);
    painter.drawText(
        static_cast<int>(startPos.x() + grid_size / 4),
        static_cast<int>(startPos.y() + grid_size * 3 / 4),
        "P"
    );
}

QColor LevelEditorCanvas::getElementColor(GameElementType elementType) const
{
    switch (elementType) {
    case GameElementType::Empty:
        return QColor(0, 0, 0, 0); // 透明
    case GameElementType::SolidBlock:
        return QColor(139, 69, 19); // 棕色
    case GameElementType::Vegetable:
        return QColor(0, 255, 0); // 绿色
    case GameElementType::LevelExit:
        return QColor(255, 0, 0); // 红色
    case GameElementType::PlayerStart:
        return QColor(255, 255, 0); // 黄色
    case GameElementType::Spike:
        return QColor(255, 0, 255); // 紫色
    case GameElementType::MovingPlatform:
        return QColor(0, 255, 255); // 青色
    case GameElementType::Checkpoint:
        return QColor(0, 0, 255); // 蓝色
    case GameElementType::ArrowTrap:
        return QColor(128, 128, 128); // 灰色
    case GameElementType::Water:
        return QColor(64, 164, 223); // 水蓝
    case GameElementType::Lava:
        return QColor(255, 85, 0); // 岩浆橙红
    default:
        return QColor(128, 128, 128);
    }
}

void LevelEditorCanvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        is_dragging = true;
        last_mouse_pos = event->pos();
        
        QPoint gridPos = screenToGrid(event->pos());
        placeElement(gridPos);
    }
}

void LevelEditorCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (is_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint gridPos = screenToGrid(event->pos());
        QPoint lastGridPos = screenToGrid(last_mouse_pos);
        
        if (gridPos != lastGridPos) {
            placeElement(gridPos);
            last_mouse_pos = event->pos();
        }
    }
}

void LevelEditorCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        is_dragging = false;
    }
}

// === LevelEditor 实现 ===

LevelEditor::LevelEditor(QWidget* parent)
    : QMainWindow(parent)
    , current_level(nullptr)
    , is_modified(false)
{
    setWindowTitle("醒狮跃境 - 关卡编辑器");
    setMinimumSize(1200, 800);
    
    // 创建新的空关卡
    current_level = new LevelData();
    current_level->setLevelName("New Level");
    current_level->setLevelDescription("Please enter level description");
    
    initializeUI();
    connectSignals();
    updateUI();
}

LevelEditor::~LevelEditor()
{
    delete current_level;
}

void LevelEditor::initializeUI()
{
    createMenuBar();
    createToolBar();
    createStatusBar();
    
    // 创建中央窗口
    central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    
    main_layout = new QHBoxLayout(central_widget);
    
    createToolPanel();
    createCanvas();
}

void LevelEditor::createMenuBar()
{
    menu_bar = menuBar();
    
    // 文件菜单
    QMenu* fileMenu = menu_bar->addMenu("文件(&F)");
    
    new_action = new QAction("新建(&N)", this);
    new_action->setShortcut(QKeySequence::New);
    fileMenu->addAction(new_action);
    
    open_action = new QAction("打开(&O)", this);
    open_action->setShortcut(QKeySequence::Open);
    fileMenu->addAction(open_action);
    
    fileMenu->addSeparator();
    
    save_action = new QAction("保存(&S)", this);
    save_action->setShortcut(QKeySequence::Save);
    fileMenu->addAction(save_action);
    
    save_as_action = new QAction("另存为(&A)", this);
    save_as_action->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(save_as_action);
    
    fileMenu->addSeparator();
    
    exit_action = new QAction("退出(&X)", this);
    exit_action->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exit_action);
    
    // 编辑菜单
    QMenu* editMenu = menu_bar->addMenu("编辑(&E)");
    
    grid_action = new QAction("显示网格(&G)", this);
    grid_action->setCheckable(true);
    grid_action->setChecked(true);
    editMenu->addAction(grid_action);
    
    // 测试菜单
    QMenu* testMenu = menu_bar->addMenu("测试(&T)");
    
    test_action = new QAction("测试关卡(&T)", this);
    test_action->setShortcut(Qt::Key_F5);
    testMenu->addAction(test_action);
}

void LevelEditor::createToolBar()
{
    tool_bar = addToolBar("主工具栏");
    
    tool_bar->addAction(new_action);
    tool_bar->addAction(open_action);
    tool_bar->addAction(save_action);
    tool_bar->addSeparator();
    tool_bar->addAction(test_action);
    tool_bar->addSeparator();
    
    // 添加返回主菜单按钮
    QAction* menu_action = new QAction("返回主菜单", this);
    menu_action->setToolTip("返回主菜单");
    menu_action->setIcon(QIcon(":/icons/home.png")); // 如果有图标的话
    connect(menu_action, &QAction::triggered, this, &LevelEditor::returnToMenu);
    tool_bar->addAction(menu_action);
}

void LevelEditor::createStatusBar()
{
    status_bar = statusBar();
    status_bar->showMessage("就绪");
}

void LevelEditor::createToolPanel()
{
    toolbar_widget = new QWidget();
    toolbar_widget->setFixedWidth(250);
    toolbar_layout = new QVBoxLayout(toolbar_widget);
    
    // 元素选择组
    element_group = new QGroupBox("游戏元素", toolbar_widget);
    QVBoxLayout* elementLayout = new QVBoxLayout(element_group);
    
    element_combo = new QComboBox();
    element_combo->addItem("空白", static_cast<int>(GameElementType::Empty));
    element_combo->addItem("实心方块", static_cast<int>(GameElementType::SolidBlock));
    element_combo->addItem("青菜", static_cast<int>(GameElementType::Vegetable));
    element_combo->addItem("关卡出口", static_cast<int>(GameElementType::LevelExit));
    element_combo->addItem("玩家起点", static_cast<int>(GameElementType::PlayerStart));
    element_combo->addItem("尖刺", static_cast<int>(GameElementType::Spike));
    element_combo->addItem("移动平台", static_cast<int>(GameElementType::MovingPlatform));
    element_combo->addItem("检查点", static_cast<int>(GameElementType::Checkpoint));
    element_combo->addItem("箭机关", static_cast<int>(GameElementType::ArrowTrap));
    element_combo->addItem("水(减速)", static_cast<int>(GameElementType::Water));
    element_combo->addItem("岩浆(死亡)", static_cast<int>(GameElementType::Lava));
    element_combo->setCurrentIndex(1); // 默认选择实心方块
    
    elementLayout->addWidget(new QLabel("选择元素类型："));
    elementLayout->addWidget(element_combo);
    
    // 箭机关方向选择
    arrow_direction_label = new QLabel("箭机关方向：");
    arrow_direction_combo = new QComboBox();
    arrow_direction_combo->addItem("向右 →", static_cast<int>(ArrowDirection::Right));
    arrow_direction_combo->addItem("向左 ←", static_cast<int>(ArrowDirection::Left));
    arrow_direction_combo->addItem("向上 ↑", static_cast<int>(ArrowDirection::Up));
    arrow_direction_combo->addItem("向下 ↓", static_cast<int>(ArrowDirection::Down));
    arrow_direction_combo->setCurrentIndex(0); // 默认向右
    
    elementLayout->addWidget(arrow_direction_label);
    elementLayout->addWidget(arrow_direction_combo);
    
    // 初始时隐藏方向选择（只有选择箭机关时才显示）
    arrow_direction_label->setVisible(false);
    arrow_direction_combo->setVisible(false);
    
    clear_button = new QPushButton("清空关卡");
    elementLayout->addWidget(clear_button);
    
    test_button = new QPushButton("测试关卡");
    elementLayout->addWidget(test_button);
    
    back_to_menu_button = new QPushButton("返回主菜单");
    back_to_menu_button->setStyleSheet("QPushButton { background-color: #ff6b6b; color: white; font-weight: bold; }");
    elementLayout->addWidget(back_to_menu_button);
    
    toolbar_layout->addWidget(element_group);
    
    // 关卡属性组
    properties_group = new QGroupBox("关卡属性", toolbar_widget);
    QVBoxLayout* propertiesLayout = new QVBoxLayout(properties_group);
    
    propertiesLayout->addWidget(new QLabel("关卡名称："));
    level_name_edit = new QLineEdit();
    propertiesLayout->addWidget(level_name_edit);
    
    propertiesLayout->addWidget(new QLabel("关卡描述："));
    level_desc_edit = new QTextEdit();
    level_desc_edit->setMaximumHeight(100);
    propertiesLayout->addWidget(level_desc_edit);
    
    toolbar_layout->addWidget(properties_group);
    
    toolbar_layout->addStretch();
    
    main_layout->addWidget(toolbar_widget);
}

void LevelEditor::createCanvas()
{
    canvas = new LevelEditorCanvas();
    canvas->setLevelData(current_level);
    
    canvas_scroll = new QScrollArea();
    canvas_scroll->setWidget(canvas);
    canvas_scroll->setWidgetResizable(false);
    
    main_layout->addWidget(canvas_scroll, 1);
}

void LevelEditor::connectSignals()
{
    // 菜单动作
    connect(new_action, &QAction::triggered, this, &LevelEditor::newLevel);
    connect(open_action, &QAction::triggered, this, &LevelEditor::openLevel);
    connect(save_action, &QAction::triggered, this, &LevelEditor::saveLevel);
    connect(save_as_action, &QAction::triggered, this, &LevelEditor::saveLevelAs);
    connect(test_action, &QAction::triggered, this, &LevelEditor::testCurrentLevel);
    connect(exit_action, &QAction::triggered, this, &LevelEditor::returnToMenu);
    connect(grid_action, &QAction::toggled, this, &LevelEditor::toggleGrid);
    
    // 工具面板
    connect(element_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LevelEditor::onElementTypeChanged);
    connect(arrow_direction_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LevelEditor::onArrowDirectionChanged);
    connect(clear_button, &QPushButton::clicked, this, &LevelEditor::clearLevel);
    connect(test_button, &QPushButton::clicked, this, &LevelEditor::testCurrentLevel);
    connect(back_to_menu_button, &QPushButton::clicked, this, &LevelEditor::returnToMenu);
    
    // 属性编辑
    connect(level_name_edit, &QLineEdit::textChanged, this, &LevelEditor::onLevelNameChanged);
    connect(level_desc_edit, &QTextEdit::textChanged, this, &LevelEditor::onLevelDescriptionChanged);
    
    // 画布
    connect(canvas, &LevelEditorCanvas::levelDataChanged, this, &LevelEditor::onCanvasDataChanged);
}

void LevelEditor::updateUI()
{
    if (current_level) {
        level_name_edit->setText(current_level->getLevelName());
        level_desc_edit->setPlainText(current_level->getLevelDescription());
        canvas->setLevelData(current_level);
    }
    
    updateWindowTitle();
}

void LevelEditor::setModified(bool modified)
{
    is_modified = modified;
    updateWindowTitle();
}

void LevelEditor::updateWindowTitle()
{
    QString title = "醒狮跃境 - 关卡编辑器";
    if (current_level) {
        title += " - " + current_level->getLevelName();
    }
    if (is_modified) {
        title += " *";
    }
    setWindowTitle(title);
}

bool LevelEditor::checkSaveBeforeClose()
{
    if (!is_modified) {
        return true;
    }
    
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        "保存确认",
        "关卡已修改，是否保存？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );
    
    switch (result) {
    case QMessageBox::Save:
        saveLevel();
        return true;
    case QMessageBox::Discard:
        return true;
    case QMessageBox::Cancel:
    default:
        return false;
    }
}

// === 槽函数实现 ===

void LevelEditor::newLevel()
{
    if (!checkSaveBeforeClose()) {
        return;
    }
    
    delete current_level;
    current_level = new LevelData();
    current_level->setLevelName("新关卡");
    current_level->setLevelDescription("请输入关卡描述");
    
    current_file_path.clear();
    setModified(false);
    updateUI();
    
    status_bar->showMessage("新建关卡", 2000);
}

void LevelEditor::openLevel()
{
    if (!checkSaveBeforeClose()) {
        return;
    }
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "打开关卡文件",
        LevelManager::getInstance().getLevelsDirectory(),
        "关卡文件 (*.json)"
    );
    
    if (!fileName.isEmpty()) {
        LevelData* newLevel = new LevelData();
        if (newLevel->loadFromFile(fileName)) {
            delete current_level;
            current_level = newLevel;
            current_file_path = fileName;
            setModified(false);
            updateUI();
            
            status_bar->showMessage("关卡加载成功", 2000);
        } else {
            delete newLevel;
            QMessageBox::warning(this, "错误", "无法加载关卡文件");
        }
    }
}

void LevelEditor::saveLevel()
{
    if (current_file_path.isEmpty()) {
        saveLevelAs();
        return;
    }
    
    if (current_level->saveToFile(current_file_path)) {
        setModified(false);
        status_bar->showMessage("关卡保存成功", 2000);
    } else {
        QMessageBox::warning(this, "错误", "无法保存关卡文件");
    }
}

void LevelEditor::saveLevelAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "保存关卡文件",
        LevelManager::getInstance().getLevelsDirectory() + "/" + current_level->getLevelName() + ".json",
        "关卡文件 (*.json)"
    );
    
    if (!fileName.isEmpty()) {
        current_file_path = fileName;
        saveLevel();
    }
}

void LevelEditor::testCurrentLevel()
{
    // 保存当前关卡到临时文件
    QString tempPath = QApplication::applicationDirPath() + "/temp_level.json";
    if (current_level->saveToFile(tempPath)) {
        emit testLevel(current_level);
        status_bar->showMessage("启动关卡测试", 2000);
    } else {
        QMessageBox::warning(this, "错误", "无法创建测试关卡");
    }
}

void LevelEditor::returnToMenu()
{
    if (checkSaveBeforeClose()) {
        emit backToMenu();
    }
}

void LevelEditor::onElementTypeChanged(int index)
{
    GameElementType elementType = static_cast<GameElementType>(
        element_combo->itemData(index).toInt()
    );
    canvas->setCurrentElementType(elementType);
    
    // 只有选择箭机关时才显示方向选择
    bool isArrowTrap = (elementType == GameElementType::ArrowTrap);
    arrow_direction_label->setVisible(isArrowTrap);
    arrow_direction_combo->setVisible(isArrowTrap);
}

void LevelEditor::onArrowDirectionChanged(int index)
{
    ArrowDirection direction = static_cast<ArrowDirection>(
        arrow_direction_combo->itemData(index).toInt()
    );
    canvas->setCurrentArrowDirection(direction);
}

void LevelEditor::onLevelNameChanged()
{
    if (current_level) {
        current_level->setLevelName(level_name_edit->text());
        setModified(true);
    }
}

void LevelEditor::onLevelDescriptionChanged()
{
    if (current_level) {
        current_level->setLevelDescription(level_desc_edit->toPlainText());
        setModified(true);
    }
}

void LevelEditor::clearLevel()
{
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        "确认清空",
        "确定要清空当前关卡吗？此操作无法撤销。",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (result == QMessageBox::Yes) {
        canvas->clearCanvas();
        setModified(true);
        status_bar->showMessage("关卡已清空", 2000);
    }
}

void LevelEditor::toggleGrid(bool show)
{
    canvas->setShowGrid(show);
}

void LevelEditor::onCanvasDataChanged()
{
    setModified(true);
}