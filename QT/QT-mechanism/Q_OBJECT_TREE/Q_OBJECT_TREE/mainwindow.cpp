#include "mainwindow.h"
#include "ui_mainwindow.h"


class A:public QObject
{
  public: //子类化 QObject
  A()
  {

  }

  A(QObject *p):QObject(p)
  {

  }

  ~A()
  {
      cout<<objectName().toStdString()<<"=~A"<<endl;
  }
};

class B:public QObject
{
  public: //子类化 QObject
  B()
  {

  }

  B(QObject *p):QObject(p)
  {

  }

  ~B()
  {
      cout<<objectName().toStdString()<<"=~B"<<endl;
  }

};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    A ma; //父对象通常创建在栈上


    A *pa1=new A(&ma); //在堆上指定父对象
    A *pa2=new A(&ma);
    B *pb1=new B(pa1);
    B *pb2=new B(pa1);

    //为各对象设置对象名
    ma.setObjectName("ma");

    pa1->setObjectName("pa1");
    pa2->setObjectName("pa2");

    pb1->setObjectName("pb1");
    pb2->setObjectName("pb2");

    A ma1;
    B mb1;
    mb1.setParent(&ma1); //在栈上把 ma1 指定为 mb1 的父对象，此处父对象创建于子对象之前。
    ma1.setObjectName("ma1");
    mb1.setObjectName("mb1");


    B mb2;
    A ma2;
    /*错误，在栈上指定父对象时，父对象应创建于子对象之前，此处会导致子对象 mb2 被删除两次。*/
    //mb2.setParent(&ma2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

