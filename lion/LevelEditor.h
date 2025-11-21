/**
 * @file LevelEditor.h
 * @brief 关卡编辑器类声明，支持可视化关卡编辑
 * @author 开发团队
 * @date 2025-10-22
 * @version 1.0.0
 */

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QGroupBox>
#include <QSpinBox>
#include <QMouseEvent>
#include <QPainter>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include "LevelData.h"
#include "LevelManager.h"
#include "Config.h"

/**
 * @class LevelEditorCanvas
 * @brief 关卡编辑画布，支持鼠标操作编辑关卡
 */
class LevelEditorCanvas : public QWidget
{
    Q_OBJECT
    
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit LevelEditorCanvas(QWidget* parent = nullptr);
    
    /**
     * @brief 设置当前编辑的关卡数据
     * @param levelData 关卡数据指针
     */
    void setLevelData(LevelData* levelData);
    
    /**
     * @brief 设置当前选中的元素类型
     * @param elementType 元素类型
     */
    void setCurrentElementType(GameElementType elementType);
    
    /**
     * @brief 获取当前选中的元素类型
     * @return GameElementType 当前元素类型
     */
    GameElementType getCurrentElementType() const;
    
    /**
     * @brief 设置网格显示状态
     * @param show 是否显示网格
     */
    void setShowGrid(bool show);
    
    /**
     * @brief 清空画布
     */
    void clearCanvas();
    
    /**
     * @brief 设置当前箭机关方向
     * @param direction 箭机关方向
     */
    void setCurrentArrowDirection(ArrowDirection direction);
    
    /**
     * @brief 设置当前移动平台距离
     * @param distance 移动距离（正数向上/右，负数向下/左）
     */
    void setCurrentPlatformDistance(int distance);

signals:
    /**
     * @brief 关卡数据改变信号
     */
    void levelDataChanged();
    
    /**
     * @brief 元素放置信号
     * @param elementType 元素类型
     * @param position 位置
     */
    void elementPlaced(GameElementType elementType, QPointF position);

protected:
    /**
     * @brief 绘制事件
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent* event) override;
    
    /**
     * @brief 鼠标按下事件
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent* event) override;
    
    /**
     * @brief 鼠标移动事件
     * @param event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent* event) override;
    
    /**
     * @brief 鼠标释放事件
     * @param event 鼠标事件
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    LevelData* level_data;                  ///< 当前编辑的关卡数据
    GameElementType current_element_type;   ///< 当前选中的元素类型
    ArrowDirection current_arrow_direction; ///< 当前箭机关方向
    int current_platform_distance;         ///< 当前移动平台距离
    bool show_grid;                         ///< 是否显示网格
    bool is_dragging;                       ///< 是否正在拖拽
    QPoint last_mouse_pos;                  ///< 上次鼠标位置
    
    int grid_size;                          ///< 网格大小
    int canvas_width;                       ///< 画布宽度
    int canvas_height;                      ///< 画布高度
    
    /**
     * @brief 将屏幕坐标转换为网格坐标
     * @param screenPos 屏幕坐标
     * @return QPoint 网格坐标
     */
    QPoint screenToGrid(const QPoint& screenPos) const;
    
    /**
     * @brief 将网格坐标转换为屏幕坐标
     * @param gridPos 网格坐标
     * @return QPoint 屏幕坐标
     */
    QPoint gridToScreen(const QPoint& gridPos) const;
    
    /**
     * @brief 在指定位置放置元素
     * @param gridPos 网格位置
     */
    void placeElement(const QPoint& gridPos);
    
    /**
     * @brief 绘制网格
     * @param painter 绘制器
     */
    void drawGrid(QPainter& painter);
    
    /**
     * @brief 绘制关卡元素
     * @param painter 绘制器
     */
    void drawLevelElements(QPainter& painter);
    
    /**
     * @brief 获取元素颜色
     * @param elementType 元素类型
     * @return QColor 颜色
     */
    QColor getElementColor(GameElementType elementType) const;
};

/**
 * @class LevelEditor
 * @brief 关卡编辑器主窗口
 * 
 * 提供完整的关卡编辑功能：
 * - 可视化编辑界面
 * - 元素工具栏
 * - 关卡属性设置
 * - 文件操作（新建、打开、保存）
 * - 测试功能
 */
class LevelEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit LevelEditor(QWidget* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LevelEditor();

signals:
    /**
     * @brief 返回主菜单信号
     */
    void backToMenu();
    
    /**
     * @brief 测试关卡信号
     * @param levelData 要测试的关卡数据
     */
    void testLevel(LevelData* levelData);

private slots:
    /**
     * @brief 新建关卡
     */
    void newLevel();
    
    /**
     * @brief 打开关卡文件
     */
    void openLevel();
    
    /**
     * @brief 保存当前关卡
     */
    void saveLevel();
    
    /**
     * @brief 另存为关卡文件
     */
    void saveLevelAs();
    
    /**
     * @brief 测试当前关卡
     */
    void testCurrentLevel();
    
    /**
     * @brief 返回主菜单
     */
    void returnToMenu();
    
    /**
     * @brief 元素类型改变槽函数
     * @param index 选择的索引
     */
    void onElementTypeChanged(int index);
    
    /**
     * @brief 箭机关方向改变槽函数
     * @param index 选择的索引
     */
    void onArrowDirectionChanged(int index);
    
    /**
     * @brief 移动平台距离改变
     */
    void onPlatformDistanceChanged(int distance);
    
    /**
     * @brief 关卡名称改变
     */
    void onLevelNameChanged();
    
    /**
     * @brief 关卡描述改变
     */
    void onLevelDescriptionChanged();
    
    /**
     * @brief 清空关卡
     */
    void clearLevel();
    
    /**
     * @brief 切换网格显示
     * @param show 是否显示
     */
    void toggleGrid(bool show);
    
    /**
     * @brief 画布数据改变处理
     */
    void onCanvasDataChanged();

private:
    // === UI组件 ===
    QWidget* central_widget;                ///< 中央窗口
    QHBoxLayout* main_layout;               ///< 主布局
    
    // 左侧工具栏
    QWidget* toolbar_widget;                ///< 工具栏窗口
    QVBoxLayout* toolbar_layout;            ///< 工具栏布局
    QGroupBox* element_group;               ///< 元素选择组
    QComboBox* element_combo;               ///< 元素类型选择框
    QComboBox* arrow_direction_combo;       ///< 箭机关方向选择框
    QLabel* arrow_direction_label;          ///< 箭机关方向标签
    QSpinBox* platform_distance_spinbox;    ///< 移动平台距离设置框
    QLabel* platform_distance_label;       ///< 移动平台距离标签
    QPushButton* clear_button;              ///< 清空按钮
    QPushButton* test_button;               ///< 测试按钮
    QPushButton* back_to_menu_button;       ///< 返回主菜单按钮
    
    // 关卡属性设置
    QGroupBox* properties_group;            ///< 属性设置组
    QLineEdit* level_name_edit;             ///< 关卡名称编辑框
    QTextEdit* level_desc_edit;             ///< 关卡描述编辑框
    
    // 画布区域
    QScrollArea* canvas_scroll;             ///< 画布滚动区域
    LevelEditorCanvas* canvas;              ///< 编辑画布
    
    // === 菜单和工具栏 ===
    QMenuBar* menu_bar;                     ///< 菜单栏
    QToolBar* tool_bar;                     ///< 工具栏
    QStatusBar* status_bar;                 ///< 状态栏
    
    // 菜单动作
    QAction* new_action;                    ///< 新建动作
    QAction* open_action;                   ///< 打开动作
    QAction* save_action;                   ///< 保存动作
    QAction* save_as_action;                ///< 另存为动作
    QAction* test_action;                   ///< 测试动作
    QAction* exit_action;                   ///< 退出动作
    QAction* grid_action;                   ///< 网格显示动作
    
    // === 数据 ===
    LevelData* current_level;               ///< 当前编辑的关卡
    QString current_file_path;              ///< 当前文件路径
    bool is_modified;                       ///< 是否已修改
    
    /**
     * @brief 初始化UI界面
     */
    void initializeUI();
    
    /**
     * @brief 创建菜单栏
     */
    void createMenuBar();
    
    /**
     * @brief 创建工具栏
     */
    void createToolBar();
    
    /**
     * @brief 创建状态栏
     */
    void createStatusBar();
    
    /**
     * @brief 创建左侧工具面板
     */
    void createToolPanel();
    
    /**
     * @brief 创建编辑画布
     */
    void createCanvas();
    
    /**
     * @brief 连接信号槽
     */
    void connectSignals();
    
    /**
     * @brief 更新UI状态
     */
    void updateUI();
    
    /**
     * @brief 设置修改状态
     * @param modified 是否已修改
     */
    void setModified(bool modified);
    
    /**
     * @brief 检查是否需要保存
     * @return bool 是否继续操作
     */
    bool checkSaveBeforeClose();
    
    /**
     * @brief 更新窗口标题
     */
    void updateWindowTitle();
};

#endif // LEVELEDITOR_H
