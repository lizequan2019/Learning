#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
      //自定义不规则单独界面
      CLS_WgtIrregularSingle * a = new CLS_WgtIrregularSingle(this);
      QWidget * ason = new QWidget(a);
      ason->setObjectName("ason");

      //自定义不规则组合界面
      CLS_WgtIrregularCombination * b = new CLS_WgtIrregularCombination(this);
      b->move(300,0);

      //不规则控件
      QPushButton * c = new QPushButton(this);
      QPixmap pix;
      pix.load(":/pic/PIC/map.svg");
      c->setObjectName("btn_c");
      c->setFixedSize(pix.size());
      c->setMask(pix.mask());
      c->move(600,0);


      //加载qss
      QFile file("./qss.qss");
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
      QString qsscontent = file.readAll();
      this->setStyleSheet(qsscontent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot()
{
    qDebug()<<"*********************";
}

