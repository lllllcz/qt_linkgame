#include "test.h"
#include "playscene.h"

Test::Test()
{

}

void Test::case_testcase1()
{
    PlayScene * ps = new PlayScene(true);
    ps->activePos = 1;
    ps->secondPos = 10;
    QVERIFY(ps->isRemovable() == false);
    delete ps;
}

void Test::case_testcase2()
{
    PlayScene * ps = new PlayScene(true);
    ps->activePos = 1;
    ps->secondPos = 2;
    QVERIFY(ps->isRemovable() == true);
    delete ps;
}

void Test::case_testcase3()
{
    PlayScene * ps = new PlayScene(true);
        ps->activePos = 1;
        ps->secondPos = WIDTH * HEIGHT - 2;
        QVERIFY(ps->isRemovable() == false);
        delete ps;
}

//QTEST_MAIN(Test)
