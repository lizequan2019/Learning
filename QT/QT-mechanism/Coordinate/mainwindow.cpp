#include "mainwindow.h"
#include "ui_mainwindow.h"

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->lab_title)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * pmouse = (QMouseEvent *)event;
            QPoint p = pmouse->pos();
            m_icorrectX = p.x();
            m_icorrectY = p.y();
            blMousePress = true;
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
                blMousePress = false;
        }
        if(event->type() == QEvent::MouseMove && blMousePress == true)
        {
            QMouseEvent * pmouse = (QMouseEvent *)event;
            //现在获取的坐标对应的坐标系统是程序界面的，(0,0)点位于界面的左上角，
            //界面移动需要的坐标对应的坐标系是全屏幕
            QPoint p = pmouse->pos();

            //转换坐标系统，MainWindow->全屏幕
            QPoint qpint = this->mapToGlobal(p);

            //修正界面移动位置
            qpint.setX(qpint.x() - m_icorrectX);
            qpint.setY(qpint.y() - m_icorrectY);
            this->move(qpint);
        }
    }

    if(obj == ui->widget)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            //初始状态
            qDebug()<<"pButton 按钮所在widget坐标系的坐标 Pos = "<<pButton->pos();


            //使用mapTo mapFrom
            QPoint p = ui->widget->mapTo(this,pButton->pos());
            qDebug()<<"mapTo    pButton按钮坐标系转换  widget->this  Pos = "<<p;

            p = ui->widget->mapFrom(this,p);
            qDebug()<<"mapFrom  pButton按钮坐标系转换  this->widget  Pos = "<<p;

            //使用mapToParent mapFromParent
            p = ui->widget->mapToParent(pButton->pos());
            qDebug()<<"使用mapToParent    pButton按钮坐标系转换  widget->widget的直接父级窗口  Pos = "<<p;

            p = ui->widget->mapFromParent(p);
            qDebug()<<"mapFromParent     pButton按钮坐标系转换  widget的直接父级窗口->widget  Pos = "<<p;

            //使用mapToGlobal  mapFromGlobal
            p = ui->widget->mapToGlobal(pButton->pos());
            qDebug()<<"使用mapToGlobal    pButton按钮坐标系转换  widget->Global  Pos = "<<p;

            p = ui->widget->mapFromGlobal(p);
            qDebug()<<"mapFromGlobal     pButton按钮坐标系转换  Global->widget  Pos = "<<p;
        }
    }

    return QMainWindow::eventFilter(obj,event);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->lab_title->installEventFilter(this);
    ui->widget->installEventFilter(this);
    blMousePress = false;

    pButton = new QPushButton(ui->widget);
    pButton->setText("Test Button");
    //pButton->move(100,100);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_close_clicked()
{
    this->close();
}


