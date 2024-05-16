#ifndef PUBLIC_DATA_H
#define PUBLIC_DATA_H

#include <QDateTime>


//方便切换 连接方式
//#define DIRECT
#define QUEUE
//#define AUTO

#ifdef DIRECT
#define CONN_TYPE  Qt::DirectConnection
#endif

#ifdef QUEUE
#define CONN_TYPE  Qt::QueuedConnection
#endif

#ifdef AUTO
#define CONN_TYPE  Qt::AutoConnection
#endif

//方便切换两种类型的线程
#if 0
    #define INHERIT_TYPE //继承QThread的类型
#else
    #define MOVE_TYPE    //将对象移动到QThread线程中类型
#endif

//主子线程模拟运行时间
const int MainThIdleTime  = 4;
const int ChildThIdleTime = 7;

//获取当前时间
QString CurTime();

#endif // PUBLIC_DATA_H
