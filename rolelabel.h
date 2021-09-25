#pragma once

#include <QLabel>

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

