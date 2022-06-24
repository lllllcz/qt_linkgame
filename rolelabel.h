#pragma once

#include <QLabel>

const int SIZE = 75;
const int WIDTH = 8;
const int HEIGHT = 6;//注意数组
//注意存档的位置
const int SUM = WIDTH*HEIGHT + (WIDTH+2)*(HEIGHT+2);

class RoleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit RoleLabel(QWidget *parent = nullptr, int num = 1);

    int direction = 0;
    int posX = 0, posY = 0;

    int number;

    QString rolePixName;

    void moveInGame();
    void changeDirection();

signals:

};

