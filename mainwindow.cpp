#include "mainwindow.h"
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //配置主场景
    setFixedSize(800, 600);
    setWindowIcon(QIcon(":/res/link.png"));
    setWindowTitle("连连看");

    //创建菜单栏
    QMenuBar * bar = menuBar();
    setMenuBar(bar);
    QMenu * menu = bar->addMenu("菜单");//创建菜单
    QAction * quitAction = menu->addAction("退出");//创建菜单项
    connect(quitAction, &QAction::triggered, [=](){
        //点击退出，实现退出游戏
        this->close();
    });

    //加载开始按钮图片
    startPix.load(":/res/startButton.png");

    //设置开始按钮
    startButton = new QPushButton(this);
    startButton->setFixedSize(startPix.size());
    startButton->setIcon(QIcon(startPix));
    startButton->setIconSize(startPix.size());
    startButton->move((this->width() - startButton->width()) * 0.5,
                      this->height() * 0.5);
    connect(startButton, &QPushButton::clicked, [=](){
        playGame(playScene, true);
    });

    //设置加载按钮
    loadButton = new QPushButton(this);
    loadButton->setFixedSize(150, 60);
    loadButton->setText("加载单人游戏");
    loadButton->move(this->width() * 0.5 - loadButton->width() - 10,
                     this->height() * 0.5 + 200);
    connect(loadButton, &QPushButton::clicked, [=](){
        playGame(playScene2, false);
    });

    //设置双人游戏按钮
    startButton2 = new QPushButton(this);
    startButton2->setFixedSize(150, 60);
    startButton2->setText("双人游戏");
    startButton2->move((this->width() - loadButton->width()) * 0.5,
                       this->height() * 0.5 + 100);
    connect(startButton2, &QPushButton::clicked, [=](){
        playDoublePlayerGame(doublePlayerScene, true);
    });

    //设置双人游戏加载按钮
    loadButton = new QPushButton(this);
    loadButton->setFixedSize(150, 60);
    loadButton->setText("加载双人游戏");
    loadButton->move(this->width() * 0.5 + 10,
                     this->height() * 0.5 + 200);
    connect(loadButton, &QPushButton::clicked, [=](){
        playDoublePlayerGame(doublePlayerScene2, false);
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //画背景
    backgroundPix.load(":/res/startBack.png");
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), backgroundPix);

}

void MainWindow::playGame(PlayScene *& ps, bool isNew)
{
    ps = new PlayScene(isNew);

    ps->setGeometry(this->geometry());
    this->hide();
    ps->show();

    connect(ps, &PlayScene::playSceneBack, [=](){
        this->setGeometry(ps->geometry());
        this->show();
        delete ps;
        playScene = NULL;
        playScene2 = NULL;
    });

    connect(ps, &PlayScene::loadGame, [=](){
        playGame(playScene2, false);
        ps->close();
    });
}

void MainWindow::playDoublePlayerGame(DoublePlayer *& dp, bool isNew)
{
    dp = new DoublePlayer(isNew);

    dp->setGeometry(this->geometry());
    this->hide();
    dp->show();

    connect(dp, &DoublePlayer::doublePlayerSceneBack, [=](){
        this->setGeometry(dp->geometry());
        this->show();
        delete dp;
        doublePlayerScene = NULL;
        doublePlayerScene2 = NULL;
    });

    connect(dp, &DoublePlayer::loadGame, [=](){
        playDoublePlayerGame(doublePlayerScene2, false);
        dp->close();
    });
}
