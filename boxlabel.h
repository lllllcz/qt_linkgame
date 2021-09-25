#pragma once

#include <QLabel>

class BoxLabel : public QLabel
{
    Q_OBJECT
public:
    explicit BoxLabel(QWidget *parent = nullptr);

    BoxLabel(int type);

    int boxType;

    void changePix(bool isActive, int num = 1);
    void changeType(int);

signals:

};

