#include "doubleplayer.h"

#include <QIcon>
#include <QTime>
#include <QPainter>
#include <QInputEvent>
#include <QPropertyAnimation>

#include <sstream>

#include <QDebug>

DoublePlayer::DoublePlayer(QWidget *parent) : QWidget(parent)
{

}

DoublePlayer::DoublePlayer(bool isNew)
{
    //配置主场景
    setFixedSize(800, 600);
    setWindowIcon(QIcon(":/res/link.png"));
    setWindowTitle("双人连连看");

    //实现返回按钮
    QPixmap backIconPix(":/res/back.png");
    QPushButton * backButton = new QPushButton(this);
    backButton->setFixedSize(backIconPix.size());
    backButton->setIcon(QIcon(backIconPix));
    backButton->setIconSize(backIconPix.size());
    backButton->move(this->width() - backButton->width(), 100);
    connect(backButton, &QPushButton::clicked,
            this, &DoublePlayer::doublePlayerSceneBack);

    //实现暂停按钮
    QPixmap pauseIconPix(":/res/pause.png");
    pauseButton = new QPushButton(this);
    pauseButton->setFixedSize(50, 50);
    pauseButton->setIcon(QIcon(pauseIconPix));
    pauseButton->setIconSize(QSize(50, 50));
    pauseButton->move(this->width()-pauseButton->width(), 0);
    connect(pauseButton, &QPushButton::clicked, this, &DoublePlayer::pauseGame);

    //加载背景
    backgroundPix.load(":/res/playBack2.jpg");

    //加载存档文件
    archiveFile = new QFile("./archive2.txt");

    //加载人物
    role1 = new RoleLabel(this);
    role2 = new RoleLabel(this, 2);

    if (isNew) {
        //新游戏
        //随机箱子类型
        QTime time = QTime::currentTime();
        unsigned seed = time.msec() + time.second();
        srand(seed);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++)
                boxType[i][j] = rand() % 3;
        }

        //初始化地图空间
        for (int k = 0; k < 30; k++)
            isEmpty[k/6][k%6] = true;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++)
                isEmpty[i+1][j+1] = false;
        }
    }
    //加载游戏
    else {
        loadGameInf();
        role1->changeDirection();
        role2->changeDirection();
    }
    //加载箱子
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (!isEmpty[i+1][j+1]) {
                boxes[i][j] = new BoxLabel(boxType[i][j]);
                boxes[i][j]->setParent(this);
                boxes[i][j]->move(200 + j*100, 100 + i*100);
            }
        }
    }

    if (!isNew) {
        if (activePos1[1] != -1)
            boxes[activePos1[1]/4][activePos1[1]%4]->changePix(false, 1);
        if (activePos1[2] != -1)
            boxes[activePos1[2]/4][activePos1[2]%4]->changePix(false, 2);
    }

    //设置1号胜利标志
    QPixmap winPix1(":/res/succeed1.png");
    setMyLabel(winLabel1, winPix1, -winPix1.height());

    //设置2号胜利标志
    QPixmap winPix2(":/res/succeed2.png");
    setMyLabel(winLabel2, winPix2, -winPix2.height());

    //设置平局标志
    QPixmap deucePix(":/res/deuce.png");
    setMyLabel(deuceLabel, deucePix, -deucePix.height());

    //设置倒计时
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        countdown--;
        if (countdown == 0) {
            isWin = true;
            disconnect(pauseButton, &QPushButton::clicked,
                       this, &DoublePlayer::pauseGame);
            timer->stop();
            //加载结束标志
            setAnimation();
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
    connect(saveButton, &QPushButton::clicked, this, &DoublePlayer::saveGameInf);

    //加载按钮
    QPushButton * loadButton = new QPushButton("加载双人游戏", pauseLabel);
    loadButton->setFixedSize(150, 50);
    loadButton->move((pauseLabel->width() - loadButton->width()) * 0.5, 300);
    connect(loadButton, &QPushButton::clicked, this, &DoublePlayer::loadGame);
}

void DoublePlayer::roleAction(RoleLabel *& role, int dir)
{
    role->direction = dir;

    switch (dir) {
    case -4:
        if (role->posY <= 0) return;
        if (isEmpty[role->posY-1][role->posX])
            role->posY -= 1;
        else
            chooseBox(role->number, role->posX-1, role->posY-2);
        break;
    case 4:
        if (role->posY >= 4) return;
        if (isEmpty[role->posY+1][role->posX])
            role->posY += 1;
        else
            chooseBox(role->number, role->posX-1, role->posY);
        break;
    case -1:
        if (role->posX <= 0) return;
        if (isEmpty[role->posY][role->posX-1])
            role->posX -= 1;
        else
            chooseBox(role->number, role->posX-2, role->posY-1);
        break;
    case 1:
        if (role->posX >= 5) return;
        if (isEmpty[role->posY][role->posX+1])
            role->posX += 1;
        else
            chooseBox(role->number, role->posX, role->posY-1);
        break;
    }
}

void DoublePlayer::chooseBox(int num, int x, int y)
{
    //第一次激活
    if (activeType[num] == -1) {
        activeType[num] = boxType[y][x];
        activePos1[num] = x + y * 4;
        boxes[y][x]->changePix(false, num);
        return;
    }

    //取消激活
    if (activePos1[num] == x + y*4) {
        activeType[num] = -1;
        activePos1[num] = -1;
        boxes[y][x]->changePix(true);
        return;
    }

    //第二次激活
    if (activeType[num] == boxType[y][x]) {
        x1 = activePos1[num] % 4 + 1;
        y1 = activePos1[num] / 4 + 1;
        x2 = x + 1;
        y2 = y + 1;
        //判断能否消除
        if (!isRemovable()) return;

        //消除两个箱子
        boxes[y1-1][x1-1]->clear();
        boxes[y2-1][x2-1]->clear();
        isEmpty[y1][x1] = true;
        isEmpty[y2][x2] = true;
        activeType[num] = -1;
        activePos1[num] = -1;

        //增加分数
        score[num] += 20;

        //判断是否结束
        int a1 = x1, a2 = x2, b1 = y1, b2 = y2;
        isWin = checkRemainder();
        x1 = a1; x2 = a2; y1 = b1; y2 = b2;
    }
}

bool DoublePlayer::isRemovable()
{
    if (x1 == x2 && abs(y1 - y2) == 1) return true;
    if (y1 == y2 && abs(x1 - x2) == 1) return true;

    isPaintable = true;

    //保证1号在2号下方
    if (y1 < y2) {
        exchange(x1, x2);
        exchange(y1, y2);
    }
    x01 = x02 = y01 = y02 = -1;
    //向左找
    if (horizontalPath(-1)) return true;
    //向右找
    if (horizontalPath(1)) return true;

    //若1号在2号左侧
    if (x1 <= x2) {
        //向上找
        if (verticalPath(-1, 1)) return true;
        //向下找
        if (verticalPath(1, 1)) return true;
    }
    //右侧
    else {
        //向上找
        if (verticalPath(-1, -1)) return true;
        //向下找
        if (verticalPath(1, -1)) return true;
    }
    isPaintable = false;
    x01 = x02 = y01 = y02 = -1;

    return false;
}

bool DoublePlayer::checkRemainder()
/***检查剩余箱子，判断游戏是否结束***/
{
    int type[3]={0};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (!isEmpty[i+1][j+1])
                type[boxType[i][j]]++;
        }
    }

    if (type[0] < 2 && type[1] < 2 && type[2] < 2)
        return true;

    if (type[0] <= 2 && type[1] <= 2 && type[2] <= 2) {
        int re[3][4];
        for (int i = 0; i < 12; i++)
            re[i/4][i%4] = -1;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                if (!isEmpty[i+1][j+1]) {
                    if (re[boxType[i][j]][0] == -1) {
                        re[boxType[i][j]][0] = i;
                        re[boxType[i][j]][1] = j;
                    }
                    else {
                        re[boxType[i][j]][2] = i;
                        re[boxType[i][j]][3] = j;
                    }
                }
            }
        }
        if (re[0][1] != -1) {
            qDebug() << re[0][0] << re[0][1];
            y1 = re[0][0];
            x1 = re[0][1];
            y2 = re[0][2];
            x2 = re[0][3];
            if (isRemovable())
                return false;
        }
        if (re[1][1] != -1) {
            qDebug() << re[1][0] << re[1][1];
            y1 = re[1][0];
            x1 = re[1][1];
            y2 = re[1][2];
            x2 = re[1][3];
            if (isRemovable())
                return false;
        }
        if (re[2][1] != -1) {
            qDebug() << re[2][0] << re[2][1];
            y1 = re[1][0];
            x1 = re[1][1];
            y2 = re[1][2];
            x2 = re[1][3];
            if (isRemovable())
                return false;
        }
        return true;
    }
    return false;
}

bool DoublePlayer::horizontalPath(int dir)
{
    int i, j, tmp;

    for (i = x1 + dir; (i >= 0 && i <= 5); i += dir) {
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

bool DoublePlayer::verticalPath(int dirY, int dirX)
{
    int i, j, tmp;

    for (j = y1 + dirY; (j >= 0 && j < 5); j += dirY) {
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

void DoublePlayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //画背景
    painter.drawPixmap(0, 0, this->width(), this->height(), backgroundPix);

    //人物移动
    role1->moveInGame();
    role2->moveInGame();

    //画路线
    if (isPaintable) {
        QPainterPath path(QPoint(x1*100+150, y1*100+50));
        painter.setPen(QPen(Qt::black, 10));
        path.lineTo(x01*100+150, y01*100+50);
        if (x02 != -1 && y02 != -1)
            path.lineTo(x02*100+150, y02*100+50);
        path.lineTo(x2*100+150, y2*100+50);
        painter.drawPath(path);
    }

    //画倒计时
    QString clockStr;
    QRectF clock(300, 505, 195, 95);
    painter.setPen(QPen(QColor(126, 126, 0), 5));
    painter.setFont(QFont("Arial", 18));
    if (countdown > 9)
        clockStr = QString("倒计时 00:%1").arg(countdown);
    else
        clockStr = QString("倒计时 00:0%1").arg(countdown);
    painter.drawRoundedRect(clock, 20, 15);
    painter.drawText(clock, Qt::AlignCenter, clockStr);

    //画分数
    QRectF scoreBoard1(0, 505, 195, 95);
    painter.setPen(QPen(QColor(0, 126, 126), 5));
    QString scoreStr1 = QString("得分 %1").arg(score[1]);
    painter.drawRoundedRect(scoreBoard1, 20, 15);
    painter.drawText(scoreBoard1, Qt::AlignCenter, scoreStr1);

    QRectF scoreBoard2(600, 505, 195, 95);
    QString scoreStr2 = QString("得分 %1").arg(score[2]);
    painter.drawRoundedRect(scoreBoard2, 20, 15);
    painter.drawText(scoreBoard2, Qt::AlignCenter, scoreStr2);
}

void DoublePlayer::keyPressEvent(QKeyEvent *event)
{
    if (isWin) return;

    if (isPaintable) {
        isPaintable = false;
    }

    if (event->key() == Qt::Key_P)
        pauseGame();

    if (isPause) return;

    switch (event->key()) {
    case Qt::Key_W:
        roleAction(role1, -4);
        break;
    case Qt::Key_S:
        roleAction(role1, 4);
        break;
    case Qt::Key_A:
        roleAction(role1, -1);
        break;
    case Qt::Key_D:
        roleAction(role1, 1);
        break;
    case Qt::Key_I:
        roleAction(role2, -4);
        break;
    case Qt::Key_K:
        roleAction(role2, 4);
        break;
    case Qt::Key_J:
        roleAction(role2, -1);
        break;
    case Qt::Key_L:
        roleAction(role2, 1);
        break;
    }
    role1->changeDirection();
    role2->changeDirection();

    this->update();

    if (isWin) {
        setAnimation();
        disconnect(pauseButton, &QPushButton::clicked,
                   this, &DoublePlayer::doublePlayerSceneBack);
        timer->stop();
    }
}

void DoublePlayer::setMyLabel(QLabel *& label, QPixmap pix, int y, int x)
{
    label = new QLabel(this);
    label->setGeometry(0, 0, pix.width(), pix.height());
    label->setPixmap(pix);
    if (x == -1) x = (this->width() - label->width()) * 0.5;
    label->move(x, y);
}

void DoublePlayer::setAnimation()
/***播放动画***/
{
    QLabel * label;

    if (score[1] == score[2]) label = deuceLabel;
    else if (score[1] > score[2]) label = winLabel1;
    else label = winLabel2;

    //将图片移动下来
    QPropertyAnimation * animation = new QPropertyAnimation(label, "geometry");

    //设置时间间隔
    animation->setDuration(1000);

    //设置开始位置
    animation->setStartValue(QRect(label->x(), label->y(),
                                   label->width(), label->height()));

    //设置结束位置
    animation->setEndValue(QRect(label->x(), label->y() + 250,
                                 label->width(), label->height()));

    //设置缓和曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);

    //设置动画
    animation->start();
    //emit playSceneBack();
}

void DoublePlayer::exchange(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

void DoublePlayer::pauseGame()
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

void DoublePlayer::saveGameInf()
{
    //d1+4,x1,y1,d2+4,x2,y2,bT,iE,coundown,i,s1/10,i,s2/10,i,aP1,i,aP2,i,aT1,i,aT2,e
    //1 + 1 + 1 + 1 + 1 + 1 + 12 + 30 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 = 69
    int i = 0;
    std::stringstream ss;

    ss << role1->direction + 4 << role1->posX << role1->posY;
    ss << role2->direction + 4 << role2->posX << role2->posY;
    for (i = 0; i < 12; i++)
            ss << boxType[i/4][i%4];
        for (i = 0; i < 30; i++)
            ss << isEmpty[i/6][i%6];
    ss << countdown << 'i'
       << score[1]/10 << 'i' << score[2]/10 << 'i'
       << activePos1[1] << 'i' << activePos1[2] << 'i'
       << activeType[1] << 'i' << activeType[2] << 'e';

    ss >> archiveData;
    //qDebug() << archiveData[0];

    archiveFile->open(QIODevice::WriteOnly);
    if (archiveFile->write(archiveData) == -1)
        qDebug() << "读取文件错误";
    archiveFile->close();
}

void DoublePlayer::loadGameInf()
{
    int i, begin, end, tmp;
    int array[7] = {0};

    archiveFile->open(QIODevice::ReadOnly);
    if (-1 == archiveFile->read(archiveData, 70))
        qDebug() << "读取文件错误";
    archiveFile->close();

    role1->direction = archiveData[0] - 52;
    role1->posX = archiveData[1] - 48;
    role1->posY = archiveData[2] - 48;
    role2->direction = archiveData[3] - 52;
    role2->posX = archiveData[4] - 48;
    role2->posY = archiveData[5] - 48;

    for (i = 0; i < 12; i++)
        boxType[i/4][i%4] = archiveData[i+6] - 48;
    for (i = 0; i < 30; i++)
        isEmpty[i/6][i%6] = archiveData[i+18] - 48;

    begin = 47;
    i = 0;
    for (end = 48; i < 7; end++) {
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
    score[1] = array[1] * 10;
    score[2] = array[2] * 10;
    activePos1[1] = array[3];
    activePos1[2] = array[4];
    activeType[1] = array[5];
    activeType[2] = array[6];

    qDebug() << activePos1[1] << activePos1[2];
}
