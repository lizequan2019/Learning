#include "InheritThread.h"
#include "public_data.h"

InheritThread::InheritThread(QObject *parent)
    : QThread(parent)
{
    qInfo()<<CurTime()<<"[InheritThread WorkThread] Init              "<<QThread::currentThreadId();
}

void InheritThread::run()
{
    qInfo()<<CurTime()<<"[InheritThread WorkThread ] run               "<<QThread::currentThreadId();

    for(int i = 0; i < 7; ++i){
        QThread::sleep(1);
        qInfo()<<CurTime()<<"[InheritThread WorkThread] Sleep 1s   "<<QThread::currentThreadId();
    }

    ThWorkTimeout();
}

void InheritThread::ThWorkTimeout()
{
    qInfo()<<CurTime()<<"[InheritThread WorkThread] EmitTimeout       "<<QThread::currentThreadId();
    emit SigTimeout();
}
