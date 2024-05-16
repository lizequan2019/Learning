#include "mainwindow.h"

#include <QApplication>

class AA:public QApplication
{//重写虚函数 notify
 public:
        AA(int i,char *p[]):QApplication(i,p)
        {

        }

        bool notify(QObject *o,QEvent *e) //所有的事件都会走这里
        {
            if(e->type()==QEvent::KeyPress)
            {//键盘按下事件
                qDebug()<<"notify keyDwon"<<endl;
            }
            if(e->type()==QEvent::MouseButtonPress)
            {//判断是否是鼠标按下事件。
                qDebug()<<"notify mouseDown"<<endl;
            }
            return QApplication::notify(o,e);
        }
};


int main(int argc, char *argv[])
{
    AA aa(argc,argv);//在 Qt 中 QApplication 或其子类型的对象只能有一个

    MainWindow w;
    w.show();

    return aa.exec();
}
