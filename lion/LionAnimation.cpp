#include "LionAnimation.h"
#include <QPainter>
#include <QDebug>
#include "Config.h"
#include <QDir>
#include <QRegularExpression>
#include <QFileInfo>

LionAnimation::LionAnimation(QWidget *parent) : QWidget(parent)
    , currentFrame(0)
    , currentType(None)
{
    // 初始化定时器（与物理刷新解耦合）
    frameTimer = new QTimer(this);
    frameTimer->setInterval(FRAME_INTERVAL_MS);
    connect(frameTimer, &QTimer::timeout, this, &LionAnimation::onFrameTimerTimeout);

    // 加载动画帧
    loadAnimationFrames();
}

// 加载帧
void LionAnimation::loadAnimationFrames() {
    // 清空原有帧
    left_frames.clear();
    right_frames.clear();
    jump_frames.clear();

    // 加载向左帧：自动枚举 left_*.png/jpg，优先 png，并按数值序排序
    {
        QDir dir(":/images");
        QStringList files = dir.entryList({"left_*.png", "left_*.jpg"}, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        QRegularExpression re("^left_(\\d+)\\.(png|jpg)$");
        QMap<int, QString> chosen;
        for (const QString &name : files) {
            QRegularExpressionMatch m = re.match(name);
            if (!m.hasMatch()) continue;
            int idx = m.captured(1).toInt();
            QString ext = m.captured(2).toLower();
            if (!chosen.contains(idx) || ext == "png") {
                chosen[idx] = name;
            }
        }
        QList<int> keys = chosen.keys();
        std::sort(keys.begin(), keys.end());
        for (int idx : keys) {
            QString path = QString(":/images/%1").arg(chosen[idx]);
            QPixmap img(path);
            if (img.isNull()) {
                qDebug() << "向左帧加载失败：" << path;
                continue;
            }
            left_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        if (left_frames.isEmpty()) {
            qDebug() << "没有可用的向左帧资源（left_*.png/jpg）";
        }
    }

    // 加载向右帧：自动枚举 right_*.png/jpg，优先 png，并按数值序排序
    {
        QDir dir(":/images");
        // 收集匹配的文件名
        QStringList files = dir.entryList({"right_*.png", "right_*.jpg"}, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        QRegularExpression re("^right_(\\d+)\\.(png|jpg)$");
        // 使用数字索引映射，png 优先
        QMap<int, QString> chosen;
        for (const QString &name : files) {
            QRegularExpressionMatch m = re.match(name);
            if (!m.hasMatch()) continue;
            int idx = m.captured(1).toInt();
            QString ext = m.captured(2).toLower();
            if (!chosen.contains(idx) || ext == "png") {
                chosen[idx] = name; // 若存在同编号png覆盖jpg
            }
        }
        QList<int> keys = chosen.keys();
        std::sort(keys.begin(), keys.end());
        for (int idx : keys) {
            QString path = QString(":/images/%1").arg(chosen[idx]);
            QPixmap img(path);
            if (img.isNull()) {
                qDebug() << "向右帧加载失败：" << path;
                continue;
            }
            right_frames.append(img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        if (right_frames.isEmpty()) {
            qDebug() << "没有可用的向右帧资源（right_*.png/jpg）";
        }
    }

    // 加载跳跃帧：选择数量更多的一组（PNG或JPG），按数值序排序，避免混用造成朝向交替
    {
        QDir dir(":/images");
        QStringList pngFiles = dir.entryList({"jump_*.png"}, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        QStringList jpgFiles = dir.entryList({"jump_*.jpg"}, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        QStringList filesToUse = (jpgFiles.size() > pngFiles.size()) ? jpgFiles : pngFiles;
        QRegularExpression re("^jump_(\\d+)\\.(png|jpg)$");
        QMap<int, QString> chosen;
        for (const QString &name : filesToUse) {
            QRegularExpressionMatch m = re.match(name);
            if (!m.hasMatch()) continue;
            int idx = m.captured(1).toInt();
            chosen[idx] = name; // 不混用：同一组文件名
        }
        QList<int> keys = chosen.keys();
        std::sort(keys.begin(), keys.end());
        for (int idx : keys) {
            QString path = QString(":/images/%1").arg(chosen[idx]);
            QPixmap img(path);
            if (img.isNull()) {
                qDebug() << "跳跃帧加载失败：" << path;
                continue;
            }
            QPixmap scaled = img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // 注意：根据你的资源说明，jump_1、jump_3 是向左的 => 编号为奇数的帧需要镜像为右向
            if (idx % 2 == 1) {
                scaled = QPixmap::fromImage(scaled.toImage().mirrored(true, false));
            }
            jump_frames.append(scaled);
        }
        if (jump_frames.isEmpty()) {
            qDebug() << "没有可用的跳跃帧资源（jump_*.png/jpg）";
        }
}
}

// 启动向左循环动画
void LionAnimation::startLeftLoop() {
    if (left_frames.isEmpty()) {
        qDebug() << "没有向左帧，无法播放动画";
        return;
    }
    currentType = Left;
    currentFrame = 0;  // 从第0帧开始
    facingRight = false; // 更新朝向
    frameTimer->start();  // 启动定时器（开始循环）
    update();  // 触发重绘
}

// 启动向右循环动画
void LionAnimation::startRightLoop() {
    if (right_frames.isEmpty()) {
        qDebug() << "没有向右帧，无法播放动画";
        return;
    }
    currentType = Right;
    currentFrame = 0;
    facingRight = true; // 更新朝向
    frameTimer->start();
    update();
}

// 启动跳跃循环动画（保留上次朝向）
void LionAnimation::startJumpLoop() {
    if (jump_frames.isEmpty()) {
        qDebug() << "没有跳跃帧，无法播放动画";
        return;
    }
    currentType = Jump;
    currentFrame = 0;
    // 不修改 facingRight，沿用上一次的左右朝向
    frameTimer->start();
    update();
}

// 新增：空闲静态帧
void LionAnimation::startIdleLeft() {
    if (left_frames.isEmpty()) return;
    currentType = IdleLeft;
    currentFrame = 0; // 首帧
    facingRight = false; // 更新朝向
    frameTimer->stop(); // 空闲不切换帧
    update();
}

void LionAnimation::startIdleRight() {
    if (right_frames.isEmpty()) return;
    currentType = IdleRight;
    currentFrame = 0; // 首帧
    facingRight = true; // 更新朝向
    frameTimer->stop(); // 空闲不切换帧
    update();
}

// 定时器触发：切换到下一帧
void LionAnimation::onFrameTimerTimeout() {
    switch (currentType) {
    case Left:
        currentFrame = (currentFrame + 1) % left_frames.size();  // 循环切换
        break;
    case Right:
        currentFrame = (currentFrame + 1) % right_frames.size();
        break;
    case Jump:
        currentFrame = (currentFrame + 1) % jump_frames.size();
        break;
    case IdleLeft:
    case IdleRight:
        // 空闲不切帧
        return;
    default:
        return;
    }
    update();  // 触发重绘，显示新帧
}

// 重绘事件：显示当前帧
void LionAnimation::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // 绘制当前帧（居中显示）
    QPixmap currentImg;
    switch (currentType) {
    case Left:
        if (!left_frames.isEmpty()) {
            currentImg = (frameTimer && !frameTimer->isActive()) ? left_frames.first() : left_frames[ currentFrame < left_frames.size() ? currentFrame : 0 ];
        }
        break;
    case Right:
        if (!right_frames.isEmpty()) {
            currentImg = (frameTimer && !frameTimer->isActive()) ? right_frames.first() : right_frames[ currentFrame < right_frames.size() ? currentFrame : 0 ];
        }
        break;
    case Jump:
        if (!jump_frames.isEmpty()) {
            currentImg = (frameTimer && !frameTimer->isActive()) ? jump_frames.first() : jump_frames[ currentFrame < jump_frames.size() ? currentFrame : 0 ];
            if (!facingRight && !currentImg.isNull()) {
                currentImg = QPixmap::fromImage(currentImg.toImage().mirrored(true, false));
            }
        }
        break;
    case IdleLeft:
        if (!left_frames.isEmpty()) currentImg = left_frames.first();
        break;
    case IdleRight:
        if (!right_frames.isEmpty()) currentImg = right_frames.first();
        break;
    default:
        // 未播放动画时，显示提示文字
        painter.drawText(rect(), Qt::AlignCenter, "点击按钮播放动画");
        return;
    }

    // 绘制图片（居中）
    if (!currentImg.isNull()) {
        int x = (width() - currentImg.width()) / 2;
        int y = (height() - currentImg.height()) / 2;
        painter.drawPixmap(x, y, currentImg);
    } else {
        painter.drawText(rect(), Qt::AlignCenter, "缺少动画帧资源");
    }
}
