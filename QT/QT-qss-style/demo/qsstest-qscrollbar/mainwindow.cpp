#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(300,300);
    loadQssFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadQssFile()
{
    QFile f("./style.qss");
    f.open(QIODevice::ReadOnly);
    QString str = f.readAll();
    this->setStyleSheet(str);
}


void MainWindow::on_pushButton_clicked()
{
    loadQssFile();
}
