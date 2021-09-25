#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QTimer>
#include "boxlabel.h"
#include "rolelabel.h"

class PlayScene : public QMainWindow
{
    Q_OBJECT
public:
    explicit PlayScene(QWidget *parent = nullptr);

    PlayScene(bool);

    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    RoleLabel * role = NULL;

    QPushButton * pauseButton;

    QPixmap playBackgroundPix;

    QLabel * winLabel, * failLabel, *pauseLabel;
    //QLabel * clearLabel;

    bool isEmpty[5][6];

    int boxType[3][4];
    BoxLabel * boxes[3][4];

    int activeType = -1;
    int activePos = -1;
    int secondPos = -1;
    int x1, x2, y1, y2;

    int x01=-1, x02=-1, y01=-1, y02=-1;

    int toolType;//0代表+1s；1代表跳转
    bool isFlying = false;
    int toolPos = -1;
    QLabel * toolLabel;

    int countdown = 30;
    int score = 0;

    QTimer * timer, * toolTimer;

    void pauseGame();
    void saveGameInf();
    void loadGameInf();

    void shuffle();

    void chooseBox(int x = -1);
    void clearBox(int);

    bool isRemovable();
    bool checkRemainder();
    bool horizontalPath(int);
    bool verticalPath(int, int);

    void setAnimation(QLabel *);
    void setMyLabel(QLabel *&, QPixmap, int, int x = -1);

    bool isWin = false;
    bool isPause = false;
    bool isPaintable = false;
    bool isFirst = true;

    QFile * archiveFile;
    char archiveData[60] = {'\0'};

signals:
    //返回信号
    void playSceneBack();

    void loadGame();

};
