#include "boxlabel.h"
#include <QPixmap>
#include <QString>

BoxLabel::BoxLabel(QWidget *parent) : QLabel(parent)
{

}

BoxLabel::BoxLabel(int type)
{
    this->setFixedSize(100, 100);

    boxType = type;

    QPixmap boxPix;
    QString str = QString(":/res/box%1.png").arg(boxType + 1);
    boxPix.load(str);
    this->setPixmap(boxPix);
}

void BoxLabel::changePix(bool isActive, int num)
{
    QPixmap boxPix;
    QString str;
    if (isActive) {
        str = QString(":/res/box%1.png").arg(boxType + 1);
    }
    else {
        str = QString(":/res/activeBox%1%2.png").arg(num).arg(boxType + 1);
    }
    boxPix.load(str);
    this->setPixmap(boxPix);
}

void BoxLabel::changeType(int type)
{
    boxType = type;

    QPixmap boxPix;
    QString str = QString(":/res/box%1.png").arg(boxType + 1);
    boxPix.load(str);
    this->setPixmap(boxPix);
}
