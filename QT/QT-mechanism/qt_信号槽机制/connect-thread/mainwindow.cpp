#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "public_data.h"

#include <iostream>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
#ifdef INHERIT_TYPE
    m_pth1->quit();
    m_pth1->wait();
#endif

#ifdef MOVE_TYPE
    m_pth2->quit();     //退出
    m_pth2->wait();     //等待
#endif

    delete ui;
}

void MainWindow::DealThread1TimeOut()
{
    qInfo()<<CurTime()<<"[Main Work Thread] Deal InheritThread DealTimeout"<<"      "<<QThread::currentThreadId();
}

void MainWindow::DealThread2TimeOut()
{
    qInfo()<<CurTime()<<"[Main Work Thread] Deal MoveToThread DealTimeout]"<<"      "<<QThread::currentThreadId();
}


void MainWindow::on_pushButton_clicked()
{
    qInfo()<<CurTime()<<"==========[Main Work Thread] on_pushButton_clicked do=========="<<QThread::currentThreadId();

    qInfo()<<CurTime()<<"[Main Work Thread] MainThIdleTime is "<<MainThIdleTime<<"(s)";

    qInfo()<<CurTime()<<"[Main Work Thread] ChildThIdleTime is "<<ChildThIdleTime<<"(s)";

#ifdef DIRECT
    qInfo()<<CurTime()<<"[Main Work Thread] use Qt::DirectConnection";
#endif

#ifdef QUEUE
    qInfo()<<CurTime()<<"[Main Work Thread] use Qt::QueuedConnection";
#endif

#ifdef AUTO
    qInfo()<<CurTime()<<"[Main Work Thread] use Qt::AutoConnection";
#endif


#ifdef INHERIT_TYPE
    qInfo()<<CurTime()<<"[Main Work Thread] use INHERIT_TYPE";
    //子线程1 继承QThread并执行run

    m_pth1 = new InheritThread;
    connect(m_pth1,SIGNAL(SigTimeout()),this,SLOT(DealThread1TimeOut()),CONN_TYPE);
    m_pth1->start(); //开启线程
#endif

#ifdef MOVE_TYPE
    qInfo()<<CurTime()<<"[Main Work Thread] use MOVE_TYPE";
    //子线程2 继承QObject并执行moveToThread

    m_pth2obj = new MoveToThread;
    m_pth2     = new QThread;
    m_pth2obj->moveToThread(m_pth2);

    //子线程发射信号
    connect(m_pth2obj,SIGNAL(SigTimeout()),this,SLOT(DealThread2TimeOut()),CONN_TYPE);

    //发射信号启动子线程函数
    connect(this,SIGNAL(SigNoticeThread2StartWork()),m_pth2obj,SLOT(Dowork()),CONN_TYPE);

    m_pth2->start(); //开启线程

    //发射信号启动子线程2
    emit SigNoticeThread2StartWork();
#endif



    //阻塞主线程
    for(int i = 0; i < 4; ++i){
        QThread::sleep(1);
        qInfo()<<CurTime()<<"[Main Work Thread] Sleep 1s   "<<QThread::currentThreadId();
    }

    qInfo()<<CurTime()<<"==========[Main Work Thread] on_pushButton_clicked done==========";
}

