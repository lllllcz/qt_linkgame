#include "boxlabel.h"
#include <QPixmap>
#include <QString>
#include <QDebug>

BoxLabel::BoxLabel(QWidget *parent) : QLabel(parent)
{

}

BoxLabel::BoxLabel(int type)
{
    this->setFixedSize(SIZE, SIZE);
    //qDebug() << SIZE;

    boxType = type;

    QPixmap boxPix;
    QString str = QString(":/res/box%1.png").arg(boxType + 1);
    boxPix.load(str);
    this->setPixmap(boxPix.scaled(SIZE, SIZE));
}

void BoxLabel::changePix(bool isActive, int num)
/***用于改变显示箱子状态的图片***/
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
    this->setPixmap(boxPix.scaled(SIZE, SIZE));
}

void BoxLabel::changeType(int type)
/***用于重排***/
{
    boxType = type;

    QPixmap boxPix;
    QString str = QString(":/res/box%1.png").arg(boxType + 1);
    boxPix.load(str);
    this->setPixmap(boxPix.scaled(SIZE, SIZE));
}
