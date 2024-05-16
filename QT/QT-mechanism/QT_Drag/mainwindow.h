#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QTextEdit>

QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class myTextEdit : public QTextEdit
{
     Q_OBJECT
public:

     void mousePressEvent(QMouseEvent *e)
     {//在该事件中启动拖放

         if(e->button() == Qt::RightButton)
         {
            QDrag *dg=new QDrag(this);
            /*将需要拖动的数据放入 QMimedata 对象中，该对象用于保存需要传递的数据，
            * 数据的内容完全由程序员自行设定。通常为界面上所选择内容。
            */

            QMimeData *md=new QMimeData;
            md->setText(this->toPlainText()); //这是 md 中存储的内容(即拖放时传递的数据)。
            dg->setMimeData(md); //步骤 1：设置拖动的数据。该函数会获得 md 的所有权。

            //步骤 2：启动拖放
            Qt::DropAction aa = dg->exec(Qt::CopyAction|Qt::MoveAction|Qt::LinkAction,Qt::LinkAction);
            qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<"   aa = "<<aa<<endl;
            if(aa == Qt::MoveAction)//在放置事件结束后才会返回，是阻塞函数但是不会阻塞主线程
            {
                 this->clear();
            }

            return;
         }
         QTextEdit::mousePressEvent(e);
     }

     //步骤 3：处理拖拽内容进入控件的事件
     void dragEnterEvent(QDragEnterEvent * e)
     {
         e->setDropAction(Qt::CopyAction);

         e->accept(); //接受拖动进入事件
        //e->ignore(); /*若忽略该事件，则不会再发送之后的事件，拖放至此结束，这会导致鼠标光标显示为禁用的图形。*/
     }


     void dragMoveEvent(QDragMoveEvent * e)
     {
         if(e->keyboardModifiers()==Qt::CTRL)
         {
            qDebug()<<"**** "<<__LINE__<<" "<<__FUNCTION__<<endl;
            e->setDropAction(Qt::CopyAction);
         }
         else if(e->keyboardModifiers()==Qt::SHIFT)
         {
            qDebug()<<"**** "<<__LINE__<<" "<<__FUNCTION__<<endl;
            e->setDropAction(Qt::MoveAction);
         }
         else if(e->keyboardModifiers()==Qt::ALT)
         {
            qDebug()<<"**** "<<__LINE__<<" "<<__FUNCTION__<<endl;
            e->setDropAction(Qt::LinkAction);
         }
         else
         {
             qDebug()<<"**** "<<__LINE__<<" "<<__FUNCTION__<<endl;
             e->setDropAction(Qt::CopyAction);
         }

         e->accept();
     }


     //步骤 4：处理拖动中的数据(当然也可不作任何处理)
     void dropEvent(QDropEvent * e)
     {//放置事件

         if(e->keyboardModifiers()== Qt::ALT)
         {
            qDebug()<<"**** "<<__LINE__<<" "<<__FUNCTION__<<endl;
            e->setDropAction(Qt::CopyAction);
         }

         //设置此部件的文本为拖动对象中的文本。
         setText(e->mimeData()->text());
         //此事件不影响后续事件，可接受也可忽略。
         e->accept();
     }
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui;

    myTextEdit * m_pedtA;
    myTextEdit * m_pedtB;
};
#endif // MAINWINDOW_H
