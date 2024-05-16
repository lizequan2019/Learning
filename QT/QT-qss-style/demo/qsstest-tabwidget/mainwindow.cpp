#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    QFile f("./style.qss");
    f.open(QIODevice::ReadOnly);
    QString qstr = f.readAll();
    this->setStyleSheet(qstr);
    f.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QFile f("./style.qss");
    f.open(QIODevice::ReadOnly);
    QString qstr = f.readAll();
    this->setStyleSheet(qstr);
    f.close();
}
