#ifndef INHERITTHREAD_H
#define INHERITTHREAD_H
/*
 *   本类继承QThread类 并且重载QThread run()函数的方法来创建线程
 */
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>

class InheritThread : public QThread
{
    Q_OBJECT

public:
    InheritThread(QObject *parent = nullptr);

    //重载run函数
    void run() override;

    void ThWorkTimeout();

signals:
    void SigTimeout();
};

#endif // INHERITTHREAD_H
