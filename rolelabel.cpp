#include "rolelabel.h"

RoleLabel::RoleLabel(QWidget *parent, int num) : QLabel(parent)
{
    rolePixName = QString(":/res/role%1R.jpg").arg(num);
    number = num;

    this->setFixedSize(100, 100);

    this->setPixmap(QPixmap(rolePixName));

    this->direction = 1;

    if (num == 2) {
        posX = 5;
        posY = 4;
    }
}

void RoleLabel::moveInGame()
{
    this->move(posX * 100 + 100, posY * 100);
}

void RoleLabel::changeDirection()
{
    switch (this->direction) {
    case 1:
        rolePixName = QString(":/res/role%1R.jpg").arg(number);
        break;
    case -1:
        rolePixName = QString(":/res/role%1L.jpg").arg(number);
        break;
    case 4:
        rolePixName = QString(":/res/role%1D.jpg").arg(number);
        break;
    case -4:
        rolePixName = QString(":/res/role%1U.jpg").arg(number);
        break;
    }
    this->setPixmap(QPixmap(rolePixName));
}
