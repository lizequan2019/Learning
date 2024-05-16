#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
#define C 1

    #ifdef A
            //除0异常
            int a = 10;
            int b = 0;
            int c = a/b;
            qDebug()<<"c = "<<c;
    #endif

    #ifdef B
            //数组越界
            int arr[10] = {1,2,3,4,5,6,7,8,9,10};
            qDebug()<<"arr[11] = "<<arr[11];

            vector<int> vec1 = {1,2,3,4};
            qDebug()<<"vec1[11] = "<<vec1.at(11);
    #endif

    #ifdef C
            //抛出异常
            try
            {
                int a = 10;
                int b = 0;
                int c = a/b;
                qDebug()<<"c = "<<c;
            }

            catch (...)
            {
                throw;
            }

    #endif
}

