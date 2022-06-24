#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QtTest/QtTest>

class Test : public QObject
{
    Q_OBJECT
private slots:
    void case_testcase1();
    void case_testcase2();
    void case_testcase3();
public:
    Test();

signals:

};

#endif // TEST_H
