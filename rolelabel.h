#pragma once

#include <QLabel>

const int SIZE = 60;
const int WIDTH = 10;
const int HEIGHT = 8;//注意数组
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

