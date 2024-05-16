#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    calendar = new CLS_DlgCalendarRadioBox("./color.ini",this);
    calendar->show();
    this->setObjectName("mainwindow");
    on_btn_changestyle_clicked();


    ParsingObjectStructure(calendar);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ParsingObjectStructure(const QObject *obj, int spaceCount)
{
    qDebug() << QString("%1%2 : %3").arg("", spaceCount,'+').arg(obj->metaObject()->className()).arg(obj->objectName());
    QObjectList list = obj->children();

    foreach (QObject * child, list)
    {
        ParsingObjectStructure(child, spaceCount + 4);
    }
}


void MainWindow::on_btn_changestyle_clicked()
{
    QFile file("./style.qss");
    file.open(QIODevice::ReadOnly);

    QByteArray array = file.readAll();
    qApp->setStyleSheet(array);
}

void MainWindow::on_pushButton_clicked()
{
    calendar->show();
}
