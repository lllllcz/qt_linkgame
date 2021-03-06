#include "playscene.h"

#include <QPainter>
#include <QTime>
#include <QString>
#include <QInputEvent>
#include <QPropertyAnimation>

#include <stdlib.h>
#include <sstream>
//#include <string>

#include <QDebug>

PlayScene::PlayScene(QWidget *parent) : QMainWindow(parent)
{

}

PlayScene::PlayScene(bool isNew)
{
    //配置主场景
    setFixedSize(840, 700);
    setWindowIcon(QIcon(":/res/link.png"));
    setWindowTitle("连连看");

    //实现返回按钮
    QPixmap backIconPix(":/res/back.png");
    QPushButton * backButton = new QPushButton(this);
    backButton->setFixedSize(backIconPix.size());
    backButton->setIcon(QIcon(backIconPix));
    backButton->setIconSize(backIconPix.size());
    backButton->move(this->width() - backButton->width(),
                     this->height() - backButton->height());
    connect(backButton, &QPushButton::clicked, this, &PlayScene::playSceneBack);

    //实现暂停按钮
    QPixmap pauseIconPix(":/res/pause.png");
    pauseButton = new QPushButton(this);
    pauseButton->setFixedSize(50, 50);
    pauseButton->setIcon(QIcon(pauseIconPix));
    pauseButton->setIconSize(QSize(50, 50));
    pauseButton->move(this->width()-pauseButton->width(), 0);
    connect(pauseButton, &QPushButton::clicked, this, &PlayScene::pauseGame);

    //加载背景
    playBackgroundPix.load(":/res/playBack2.jpg");

    //加载存档文件
    archiveFile = new QFile("./archive.txt");

    //设置人物
    role = new RoleLabel(this);

    if (isNew) {
        //新游戏
        //随机箱子类型
        QTime time = QTime::currentTime();
        unsigned seed = time.msec() + time.second();
        srand(seed);
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++)
                boxType[i][j] = rand() % 3;
        }

//        qDebug() << "箱子类型：";
//        qDebug() << boxType[0][0] << boxType[0][1] << boxType[0][2] << boxType [0][3];
//        qDebug() << boxType[1][0] << boxType[1][1] << boxType[1][2] << boxType [1][3];
//        qDebug() << boxType[2][0] << boxType[2][1] << boxType[2][2] << boxType [2][3] << '\n';

        //初始化地图空间
        for (int i = 0; i < HEIGHT+2; i++) {
            for (int j = 0; j < WIDTH+2; j++)
                isEmpty[i][j] = true;
        }
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++)
                isEmpty[i+1][j+1] = false;
        }
    }
    else {
        //加载游戏
        loadGameInf();
        role->changeDirection();
    }
    //加载箱子
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (!isEmpty[i+1][j+1]) {
                boxes[i][j] = new BoxLabel(boxType[i][j]);
                boxes[i][j]->setParent(this);
                boxes[i][j]->move(2*SIZE + j*SIZE, SIZE + i*SIZE);
            }
        }
    }

    if (!isNew && activePos != -1) {
        if(activePos != -1){
            boxes[activePos/WIDTH][activePos%WIDTH]->changePix(false);
            //activeType = boxType[activePos/4][activePos%4];
        }
    }

    //设置道具
    toolType = rand() % 3;
//    toolType = 2;

    toolPos = rand() % ((WIDTH+2)*(HEIGHT+2));
    while(!isEmpty[toolPos/(WIDTH+2)][toolPos%(WIDTH+2)])
        toolPos = (toolPos + 1) % ((HEIGHT+2)*(WIDTH+2));
    if (toolPos == 0) toolPos++;

    toolLabel = new QLabel(this);
    toolLabel->setGeometry(0, 0, SIZE, SIZE);
    QPixmap pix;
    switch (toolType) {
    case 0: pix = QPixmap(":/res/tool1.png");break;
    case 1: pix = QPixmap(":/res/tool2.png");break;
    case 2: pix = QPixmap(":/res/tool3.png");break;
    }
    toolLabel->setPixmap(pix.scaled(SIZE, SIZE));
    toolLabel->move(SIZE + toolPos%(WIDTH+2) * SIZE, toolPos/(WIDTH+2) * SIZE);

    //胜利标志设置
    QPixmap winPix(":/res/succeed.png");
    setMyLabel(winLabel, winPix, -winPix.height());

    //失败标志设置
    QPixmap failPix(":/res/fail.png");
    setMyLabel(failLabel, failPix, -failPix.height());

//    //设置消除标志
//    clearLabel = new QLabel(this);
//    pix.load(":/res/clear.png");
//    clearLabel->setGeometry(0, 0, pix.width(), pix.height());
//    clearLabel->setPixmap(pix);
//    clearLabel->move((this->width() - pix.width()) * 0.5,
//                     this->height() - pix.height() - 20);
//    clearLabel->hide();

    //设置倒计时
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        countdown--;
        if (countdown == 0) {
            isWin = true;
            disconnect(pauseButton, &QPushButton::clicked,
                       this, &PlayScene::pauseGame);
            timer->stop();
            setAnimation(failLabel);
        }
        this->update();
    });
    timer->start(1000);

    //设置暂停标志
    QPixmap pausingPix(":/res/pausing.png");
    setMyLabel(pauseLabel, pausingPix, 50);
    pauseLabel->hide();

    //保存按钮
    QPushButton * saveButton = new QPushButton("保存", pauseLabel);
    saveButton->setFixedSize(150, 50);
    saveButton->move((pauseLabel->width() - saveButton->width()) * 0.5, 200);
    connect(saveButton, &QPushButton::clicked, this, &PlayScene::saveGameInf);

    QPushButton * loadButton = new QPushButton("加载单人游戏", pauseLabel);
    loadButton->setFixedSize(150, 50);
    loadButton->move((pauseLabel->width() - loadButton->width()) * 0.5, 300);
    connect(loadButton, &QPushButton::clicked, this, &PlayScene::loadGame);
}

void PlayScene::pauseGame()
/***暂停游戏***/
{
    if (isPause) {
        isPause = false;
        pauseLabel->hide();
        timer->start();
    }
    else {
        isPause = true;
        pauseLabel->show();
        timer->stop();
    }
}

void PlayScene::saveGameInf()
/***储存游戏数据***/
{
    //储存数据
    //d+4,x,y,boxType,isEmpty,coundown,i,score,i,activePos,i,activeType,e
    //1 + 1 + 1 + 12 + 30 + 2 + 1 + 2 + 1 + 2 + 1 + 1 + 1 = 56
    int i = 0;
    //std::string str;
    std::stringstream ss;

    ss << role->direction + 4 << role->posY;
    for (i = 0; i < WIDTH*HEIGHT; i++)
        ss << boxType[i/WIDTH][i%WIDTH];
    for (i = 0; i < (WIDTH+2)*(HEIGHT+2); i++)
        ss << isEmpty[i/(WIDTH+2)][i%(WIDTH+2)];
    ss << countdown << 'i' << score << 'i' << role->posX << 'i'
       << activePos << 'i' << activeType << 'e';

    ss >> archiveData;
    qDebug() << archiveData[0];

//    for (i = 0; str.at(i) != 'e'; i++)
//        archiveData[i] = str.at(i);

    archiveFile->open(QIODevice::WriteOnly);
    if (archiveFile->write(archiveData) == -1)
        qDebug() << "读取文件错误";
    archiveFile->close();
}

void PlayScene::loadGameInf()
/***加载游戏数据***/
{
    int i = 0, begin, end, tmp;
    int array[5] = {0};

    archiveFile->open(QIODevice::ReadOnly);
    if (-1 == archiveFile->read(archiveData, SUM+18))
        qDebug() << "读取文件错误";
    archiveFile->close();

    role->direction = archiveData[0] - 52;
    role->posY = archiveData[1] - 48;

    for (; i < WIDTH*HEIGHT; i++)
        boxType[i/WIDTH][i%WIDTH] = archiveData[i+2] - 48;

    for (i = 0; i < (WIDTH+2)*(HEIGHT+2); i++)
        isEmpty[i/(WIDTH+2)][i%(WIDTH+2)] = archiveData[i+2+WIDTH*HEIGHT] - 48;

    begin = SUM + 1;
    i = 0;
    for (end = begin+1; i < 5; end++) {
        if (archiveData[end] == 'i' || archiveData[end] == 'e') {
            if (end - begin == 2)
                array[i] = archiveData[begin+1] - 48;
            else if (archiveData[begin+1] == '-')
                array[i] = -1;
            else {
                tmp = (archiveData[begin+1] - 48) * 10;
                array[i] = tmp + archiveData[begin+2] - 48;
            }
            begin = end;
            i++;
        }
    }

    countdown = array[0];
    score = array[1];
    role->posX = array[2];
    activePos = array[3];
    activeType = array[4];
}

void PlayScene::shuffle()
/***重新排列***/
{
    //重置箱子
    QTime time = QTime::currentTime();
    unsigned seed = time.msec() + time.second();
    srand(seed);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++)
            boxType[i][j] = rand() % 3;
    }

    //加载箱子
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (!isEmpty[i+1][j+1])
                boxes[i][j]->changeType(boxType[i][j]);
        }
    }
    if(activePos != -1){
        boxes[activePos/WIDTH][activePos%WIDTH]->changePix(false);
        //activeType = boxType[activePos/4][activePos%4];
    }
    this->update();
}

void PlayScene::paintEvent(QPaintEvent *)
{
    //画背景
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), playBackgroundPix);

    //移动人物
    role->moveInGame();

    //画路线
    if (isPaintable) {
        QPainterPath path;
        painter.setPen(QPen(Qt::black, 8));
        path.moveTo(x1*SIZE+1.5*SIZE, y1*SIZE+0.5*SIZE);
        path.lineTo(x01*SIZE+1.5*SIZE, y01*SIZE+0.5*SIZE);
        if (x02 != -1 && y02 != -1)
            path.lineTo(x02*SIZE+1.5*SIZE, y02*SIZE+0.5*SIZE);
        path.lineTo(x2*SIZE+1.5*SIZE, y2*SIZE+0.5*SIZE);
        painter.drawPath(path);
    }

    QString str;

    //画倒计时
    QRectF clock(0, this->height()-100, 195, 90);
    painter.setPen(QPen(QColor(255, 0, 0), 5));
    painter.setFont(QFont("Arial", 18));
    if (countdown > 9)
        str = QString("倒计时 00:%1").arg(countdown);
    else
        str = QString("倒计时 00:0%1").arg(countdown);
    painter.drawRoundedRect(clock, 20, 15);
    painter.drawText(clock, Qt::AlignCenter, str);

    //画分数
    QRectF scoreBoard(200, this->height()-100, 195, 90);
    painter.setPen(QPen(QColor(0, 0, 255), 5));
    str = QString("得分 %1").arg(score);
    painter.drawRoundedRect(scoreBoard, 20, 15);
    painter.drawText(scoreBoard, Qt::AlignCenter, str);
}

void PlayScene::keyPressEvent(QKeyEvent *event)
{
    if (isWin) return;

    if (isPaintable) {
        clearBox(secondPos);
        int tmp1 = activePos, tmp2 = secondPos;
        isWin = checkRemainder();
        activePos = tmp1;
        secondPos = tmp2;
        isPaintable = false;
        activePos = -1;
        secondPos = -1;
    }

    if (event->key() == Qt::Key_P)
        pauseGame();

    if (isPause) return;

    switch (event->key()) {
    case Qt::Key_S:
        role->direction = 4;
        if (role->posY < HEIGHT+1 && isEmpty[role->posY+1][role->posX])
            role->posY += 1;
        else chooseBox();
        break;
    case Qt::Key_W:
        role->direction = -4;
        if (role->posY > 0 && isEmpty[role->posY-1][role->posX])
            role->posY -= 1;
        else chooseBox();
        break;
    case Qt::Key_D:
        role->direction = 1;
        if (role->posX < WIDTH+1 && isEmpty[role->posY][role->posX+1])
            role->posX += 1;
        else chooseBox();
        break;
    case Qt::Key_A:
        role->direction = -1;
        if (role->posX > 0 && isEmpty[role->posY][role->posX-1])
            role->posX -= 1;
        else chooseBox();
        break;
    case Qt::Key_E:
        chooseBox();
        break;
    }
    role->changeDirection();

    if (isFirst && role->posX + role->posY*(WIDTH+2) == toolPos) {
        isFirst = false;
        toolLabel->clear();
        switch (toolType) {
        case 0: countdown++;break;
        case 1:
            isFlying = true;
            toolTimer = new QTimer(this);
            toolTimer->start(5000);
            connect(toolTimer, &QTimer::timeout, [=](){isFlying = false;});
            break;
        case 2: shuffle();break;
        }
    }

    this->update();

    if (isWin) {
        qDebug() << "WIN!!!";
        setAnimation(winLabel);
        disconnect(pauseButton, &QPushButton::clicked,
                   this, &PlayScene::pauseGame);
        timer->stop();
        //emit playSceneBack();
    }
    //qDebug() << x1 << y1 << "->" << x01 << y01 << "->" << x02 << y02 << "->" << x2 << y2;
}

void PlayScene::mousePressEvent(QMouseEvent *ev)
{
    if (!isFlying) return;
    int x, y;
    x = ev->x()/SIZE - 1;
    y = ev->y()/SIZE;
    qDebug() << "点击" << x << y;

    if (x < 0 || x > (WIDTH+1) || y < 0 || y > (HEIGHT+1)) return;

    if (isEmpty[y][x]) {
        role->posX = x;
        role->posY = y;
        this->update();
        isFlying = false;
    }
    else {
        //点击箱子
        int dir[4] = {1, -1, WIDTH+2, -WIDTH-2};
        int i;
        int pos = x + y * (WIDTH+2);
        for (i = 0; i < 4; i++) {
            pos += dir[i];
            if (isEmpty[pos/(WIDTH+2)][pos%(WIDTH+2)])
                break;
            pos -= dir[i];
        }
        if ( i == 4 ) {
            isFlying = false;
            return;
        }
        role->posX = pos % (WIDTH+2);
        role->posY = pos / (WIDTH+2);
        this->update();
        isFlying = false;

        int p = (x - 1) + (y - 1) * (WIDTH);
        chooseBox(p);
    }
}

void PlayScene::chooseBox(int x)
/***选中当前指向的箱子，若无箱子则?***/
{
    if (x == -1) {
        switch (role->direction) {
        case 4:
            if (role->posY >= HEIGHT+1) x = -1;
            if (!isEmpty[role->posY+1][role->posX])
                x = (role->posY * WIDTH + role->posX -1);
            break;
        case -4:
            if (role->posY <= 0) x = -1;
            if (!isEmpty[role->posY-1][role->posX])
                x = ((role->posY - 2) * WIDTH + role->posX - 1);
            break;
        case 1:
            if (role->posX >= WIDTH+1) x = -1;
            if (!isEmpty[role->posY][role->posX+1])
                x = ((role->posY - 1) * WIDTH + role->posX);
            break;
        case -1:
            if (role->posY <= 0) x = -1;
            if (!isEmpty[role->posY][role->posX-1])
                x = ((role->posY - 1) * WIDTH + role->posX - 2);
            break;
        }

        if (x == -1) return;//没有指向箱子
    }

    //第一次激活
    if (activePos == -1) {
        activePos = x;
        boxes[activePos/WIDTH][activePos%WIDTH]->changePix(false);
        activeType = boxType[activePos/WIDTH][activePos%WIDTH];
        return;
    }

    //取消激活
    if (activePos == x) {
        boxes[activePos/WIDTH][activePos%WIDTH]->changePix(true);
        activePos = -1;
        activeType = -1;
    }

    //第二次激活
    if (activeType == boxType[x/WIDTH][x%WIDTH]) {
        secondPos = x;
        if (isRemovable()) {
            //clearBox(secondPos);
            score += 2;
            //isWin = checkRemainder();//注释掉，以免影响isPaintable，或许应该改改isRemovable
        }
        else return;
        //activePos = -1;
        //secondPos = -1;
    }

    return;
}

bool PlayScene::horizontalPath(int dir)
/***判断有无水平方向路径***/
{
    int i, j, tmp;

    for (i = x1 + dir; (i >= 0 && i < WIDTH+2); i += dir) {
        if (isEmpty[y1][i]) {
            tmp = i;
            y01 = y1;

            for (j = y1 - 1; j >= y2; j--) {
                if (!isEmpty[j][i]) break;
            }
            if (i == x2 && j == y2) {
                x01 = x2;
                return true;
            }
            if (j == y2 - 1) {
                x01 = x02 =i;
                if (i < x2) {
                    for (j++; i < x2; i++) {
                        if (!isEmpty[j][i]) break;
                    }
                }
                else {
                    for (j++; i > x2; i--) {
                        if (!isEmpty[j][i]) break;
                    }
                }
                if (i == x2) {
                    y02 = y2;
                    return true;
                }
            }
            i = tmp;
        }
        else break;
    }
    return false;
}

bool PlayScene::verticalPath(int dirY, int dirX)
/***判断有无垂直方向路径***/
{
    int i, j, tmp;

    for (j = y1 + dirY; (j >= 0 && j < HEIGHT+2); j += dirY) {
        if (isEmpty[j][x1]) {
            tmp = j;
            x01 = x1;
            if (dirX == 1) {
                for (i = x1 + dirX; i <= x2; i += dirX) {
                    if (!isEmpty[j][i]) break;
                }
            }
            else {
                for (i = x1 - 1; i >= x2; i--) {
                    if (!isEmpty[j][i]) break;
                }
            }
            if (i == x2 && j == y2) {
                y01 = y2;
                return true;
            }
            if (i == x2 + dirX) {
                i -= dirX;
                y01 = y02 = j;
                if (j < y2) {
                    for (; j < y2; j++) {
                        if (!isEmpty[j][i]) break;
                    }
                }
                else {
                    for (; j > y2; j--) {
                        if (!isEmpty[j][i]) break;
                    }
                }
                if (j == y2) {
                    x02 = x2;
                    return true;
                }
            }
            j = tmp;
        }
        else break;
    }
    return false;
}

bool PlayScene::isRemovable()
/***判断能否消除***/
{
    //表示刚才进行了消除操作，需要显示路线，并在下一次移动后消除路线
    isPaintable = true;

    y01 = x01 = x02 = y02 = -1;

    x1 = activePos%WIDTH + 1;
    x2 = secondPos%WIDTH + 1;
    y1 = activePos/WIDTH + 1;
    y2 = secondPos/WIDTH + 1;

    //若1号在2号上方，转换
    if (y1 < y2) {
        x2 = activePos%WIDTH + 1;
        x1 = secondPos%WIDTH + 1;
        y2 = activePos/WIDTH + 1;
        y1 = secondPos/WIDTH + 1;
    }
    //向左找
    if (horizontalPath(-1)) return true;
    //向右找
    if (horizontalPath(1)) return true;

    //1号在2号左方
    if (x1 <= x2){
        //向下找
        if (verticalPath(1, 1)) return true;
        //向上找
        if (verticalPath(-1, 1)) return true;
    }

    //1号在2号右方
    if (x1 >= x2 && y1 >= y2){
        //向下找
        if (verticalPath(1, -1)) return true;
        //向上找
        if (verticalPath(-1, -1)) return true;
    }
    isPaintable = false;
    x01 = x02 = y01 = y02 = -1;

    if (abs(activePos - secondPos) == 1 || abs(activePos - secondPos) == WIDTH)
        return true;

    return false;
}

void PlayScene::clearBox(int activePos2)
/***消除选中的箱子，并重置激活项***/
{
    boxes[activePos2/WIDTH][activePos2%WIDTH]->clear();
    boxes[activePos/WIDTH][activePos%WIDTH]->clear();
    qDebug() << "消除";

    isEmpty[activePos2/WIDTH+1][activePos2%WIDTH+1] = true;
    isEmpty[activePos/WIDTH+1][activePos%WIDTH+1] = true;
    activeType = -1;

//    //显示 消除 字样
//    clearLabel->show();
//    QTimer::singleShot(666, this, [=](){
//        clearLabel->hide();
//    });
}

bool PlayScene::checkRemainder()
/***检查剩余箱子，判断游戏是否结束***/
{
    int type[3]={0};

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (!isEmpty[i+1][j+1])
                type[boxType[i][j]]++;
        }
    }

    if (type[0] < 2 && type[1] < 2 && type[2] < 2)
        return true;

    if (type[0] <= 2 && type[1] <= 2 && type[2] <= 2) {
        int re[3][2];
        for (int i = 0; i < 6; i++)
            re[i/2][i%2] = -1;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (!isEmpty[i+1][j+1]) {
                    if (re[boxType[i][j]][0] == -1) re[boxType[i][j]][0] = i * WIDTH + j;
                    else re[boxType[i][j]][1] = i * WIDTH + j;
                }
            }
        }
        if (re[0][1] != -1) {
            qDebug() << re[0][0] << re[0][1];
            activePos = re[0][0];
            secondPos = re[0][1];
            if (isRemovable())
                return false;
        }
        if (re[1][1] != -1) {
            qDebug() << re[1][0] << re[1][1];
            activePos = re[1][0];
            secondPos = re[1][1];
            if (isRemovable())
                return false;
        }
        if (re[2][1] != -1) {
            qDebug() << re[2][0] << re[2][1];
            activePos = re[2][0];
            secondPos = re[2][1];
            if (isRemovable())
                return false;
        }
        return true;
    }
    return false;
}

void PlayScene::setAnimation(QLabel * label)
/***播放动画***/
{
    //将图片移动下来
    QPropertyAnimation * animation = new QPropertyAnimation(label, "geometry");

    //设置时间间隔
    animation->setDuration(1000);

    //设置开始位置
    animation->setStartValue(QRect(label->x(), label->y(),
                                   label->width(), label->height()));

    //设置结束位置
    animation->setEndValue(QRect(label->x(), label->y() + 120,
                                 label->width(), label->height()));

    //设置缓和曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);

    //设置动画
    animation->start();
    //emit playSceneBack();
}

void PlayScene::setMyLabel(QLabel *& label, QPixmap pix, int y, int x)
/***对标签的处理***/
{
    label = new QLabel(this);
    label->setGeometry(0, 0, pix.width(), pix.height());
    label->setPixmap(pix);
    if (x == -1) x = (this->width() - label->width()) * 0.5;
    label->move(x, y);
}
