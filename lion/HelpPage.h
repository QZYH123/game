#ifndef HELPPAGE_H
#define HELPPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

class HelpPage : public QWidget
{
    Q_OBJECT

public:
    explicit HelpPage(QWidget *parent = nullptr);
    ~HelpPage();

signals:
    void backToMenu();

private:
    void setupUI();
    void createContent();
    
    QPushButton* backButton;
    QScrollArea* scrollArea;
    QWidget* contentWidget;
};

#endif // HELPPAGE_H