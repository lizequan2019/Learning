#include "mainwindow.h"
#include "ui_mainwindow.h"



QVariant mySheetStyle(const QString & start, const QString & end, qreal progress)
{
    int red   = (int)(40 * progress) + 50;
    int green = (int)(50 * progress) + 50;
    int blue  = (int)(200 * progress) + 50;

    QString style = QString("#btn_A { background-color: rgb(%1,%2,%3)} ").arg(red).arg(green).arg(blue);
    qDebug()<<"style = "<<style;
    return style;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("#btn_A{ background-color:red} ");
    qRegisterAnimationInterpolator<QString>(mySheetStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_btn_B_clicked()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->btn_A, "styleSheet");
    animation->setDuration(1000);
    animation->setStartValue(QString("QPushButton{ background-color:#000000} "));
    animation->setEndValue(QString("QPushButton{ background-color:#ffffff} "));
    animation->start();
}
