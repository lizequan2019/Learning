#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mybutton = new myButton;
    mybutton->setGeometry(0,0,200,200);
    mybutton->setParent(this);
    mTextEdit = new QTextEdit(this);
    mTextEdit->setGeometry(250,0,200,200);
    mTextEdit->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *e) //父对象的事件处理
{
    if(e->type()==QEvent::KeyPress)
    {
       qDebug()<<"[parent] event keyDwon"<<endl;
    }

    if(e->type()==QEvent::MouseButtonPress)
    {
        qDebug()<<"[parent] event mouseDwon"<<endl;
    }

    return QMainWindow::event(e);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"[parent] mousePressEvent mouseDwon"<<endl;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    qDebug()<<"[parent] keyPressEvent keyDwon e->key() = "<<e->key()<<endl;
    qDebug()<<"e->modifiers() = "<<e->modifiers()<<endl;
}

bool MainWindow::eventFilter(QObject *w, QEvent *e)
{
    return QMainWindow::eventFilter(w,e);
}


bool myButton::event(QEvent *e)//子对象的事件处理
{
    if(e->type()==QEvent::MouseButtonPress)
    {//判断是否是鼠标按下事件。
        qDebug()<<"[son] event mouseDwon"<<endl;
        //return false;
    }

    if(e->type()==QEvent::KeyPress)
    {//判断是否是键盘按下事件。
        qDebug()<<"[son] event keyDwon"<<endl;
        return  true;
    }

    return QPushButton::event(e);
}

void myButton::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"[son] mousePressEvent keyDwon"<<endl;
    //默认执行 accept 不再向父对象传递事件
}

void myButton::keyPressEvent(QKeyEvent *e)
{
    qDebug()<<"[son] keyPressEvent keyDwon"<<endl;

    //设置忽略还会继续传递
    e->ignore();
}

void MainWindow::on_pushButton_clicked()
{
    QKeyEvent * event = new QKeyEvent(QEvent::KeyPress,Qt::Key_F4,Qt::AltModifier);
    QApplication::postEvent(this,event);
}
