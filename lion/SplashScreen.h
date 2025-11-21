#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();

signals:
    void finished();

private slots:
    void startFadeOut();

private:
    void setupUI();
    
    QLabel* imageLabel;
    QLabel* textLabel;
    QTimer* displayTimer;
    QPropertyAnimation* fadeAnimation;
};

#endif // SPLASHSCREEN_H
