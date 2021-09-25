#pragma once

#include <QMainWindow>
#include "playscene.h"
#include "doubleplayer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //重写，画背景图
    void paintEvent(QPaintEvent *);

    PlayScene * playScene = NULL;
    PlayScene * playScene2 = NULL;

    DoublePlayer * doublePlayerScene = NULL;

private:
    QPushButton * startButton, * loadButton, * startButton2;

    QPixmap startPix, backgroundPix;

    void playGame(PlayScene* &, bool);
};
