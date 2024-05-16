#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout * pvlayout = new QVBoxLayout;
    pvlayout->setContentsMargins(0,0,0,0);

    CLS_MyWgt * pwgt = new CLS_MyWgt(this);

    pvlayout->addWidget(pwgt);

    this->centralWidget()->setLayout(pvlayout);

}

MainWindow::~MainWindow()
{
    delete ui;
}

