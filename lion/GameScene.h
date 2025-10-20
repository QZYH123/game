#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include "qtimer.h"
#include "QKeyEvent"
#include <QLabel>
#include <QFont>
#include "player.h"
#include "qdebug.h"
#include "qpainter.h"
#include <QString>
#include "Config.h"
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
    QPixmap block1,block2,block3,block4,block5;
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
    void init();
    void mapInit();
    void gameStart();
    void gamewin();
    void updatePosition();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif
