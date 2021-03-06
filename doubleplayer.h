#pragma once

#include <QWidget>
#include "rolelabel.h"
#include "boxlabel.h"
#include <QTimer>
#include <QPushButton>
#include <QFile>

class DoublePlayer : public QWidget
{
    Q_OBJECT
public:
    explicit DoublePlayer(QWidget *parent = nullptr);

    DoublePlayer(bool);

    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPixmap backgroundPix;
    QLabel * winLabel1, * winLabel2, * deuceLabel, * pauseLabel;
    QPushButton * pauseButton;

    RoleLabel * role1, * role2;

    int boxType[8][WIDTH];
    BoxLabel * boxes[8][WIDTH];

    bool isEmpty[8+2][WIDTH+2];

    QTimer * timer;
    int countdown = 30;
    int score[3] = {0};
    bool isWin = false, isPause = false, isPaintable = false;

    void setMyLabel(QLabel *&, QPixmap, int y, int x = -1);
    void setAnimation();

    void exchange(int &, int &);

    int activePos1[3] = {0, -1, -1};
    int activeType[3] = {0, -1, -1};
    int x1, x2, y1, y2;
    int x01 = -1, x02 = -1, y01 = -1, y02 = -1;

    void roleAction(RoleLabel *&, int);
    void chooseBox(int, int, int);

    bool isRemovable();
    bool horizontalPath(int);
    bool verticalPath(int, int);
    bool checkRemainder();

    void pauseGame();
    void saveGameInf();
    void loadGameInf();

    QFile * archiveFile;
    char archiveData[33+SUM] = {'\0'};

signals:
    void doublePlayerSceneBack();
    void loadGame();

};

