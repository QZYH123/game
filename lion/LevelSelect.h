#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class GameScene;

class LevelSelect : public QMainWindow
{
    Q_OBJECT

public:
    explicit LevelSelect(QWidget *parent = nullptr);
    ~LevelSelect();
    void ui_load();
    void setLevelUnlocked(int levelIndex, bool unlocked);
    bool isLevelUnlocked(int levelIndex) const;
    void loadGameProgress();

signals:
    void backToMenu();
    void levelSelected(int levelIndex);

private slots:
    void onLevelButtonClicked();
    void onBackButtonClicked();

private:
    static const int MAX_LEVELS = 6;
    QPushButton** level_buttons;
    QPushButton* back_button;
    QLabel* back_button_label;
    bool level_unlocked[MAX_LEVELS];
    void updateLevelButtonStyle(int levelIndex);
};

#endif // LEVELSELECT_H