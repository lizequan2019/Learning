#include "MoveToThread.h"
#include "public_data.h"

MoveToThread::MoveToThread(QObject *parent)
    : QObject{parent}
{
    qInfo()<<CurTime()<<"[MoveToThread WorkThread] Init              "<<QThread::currentThreadId();
}

void MoveToThread::Dowork()
{
    qInfo()<<CurTime()<<"[MoveToThread WorkThread] run             "<<QThread::currentThreadId();

    for(int i = 0; i < 7; ++i){
        QThread::sleep(1);
        qInfo()<<CurTime()<<"[MoveToThread WorkThread] Sleep 1s   "<<QThread::currentThreadId();
    }

    ThWorkTimeout();
}

void MoveToThread::ThWorkTimeout()
{
    qInfo()<<CurTime()<<"[MoveToThread Work]  EmitTimeout         "<<QThread::currentThreadId();
    emit SigTimeout();
}
