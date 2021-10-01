#include "rolelabel.h"

RoleLabel::RoleLabel(QWidget *parent, int num) : QLabel(parent)
{
    rolePixName = QString(":/res/role%1R.jpg").arg(num);
    number = num;

    this->setFixedSize(SIZE, SIZE);

    QPixmap pix = QPixmap(rolePixName);
    this->setPixmap(pix.scaled(SIZE, SIZE));

    this->direction = 1;

    if (num == 2) {
        posX = WIDTH+1;
        posY = HEIGHT+1;
    }
}

void RoleLabel::moveInGame()
{
    this->move(posX * SIZE + SIZE, posY * SIZE);
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
    QPixmap pix = QPixmap(rolePixName);
    this->setPixmap(pix.scaled(SIZE, SIZE));
}
