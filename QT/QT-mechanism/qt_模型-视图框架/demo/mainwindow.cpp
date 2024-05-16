#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <Qt>
#include <QTableView>
#include <QMimeData>
#include <QDebug>
using namespace std;


/*
 * 自定义模型至少需要实现以下 5 个纯虚函数
 * columnCout()、rowCount()、index()、parent()、data()
 */

class D:public QAbstractItemModel  //继承抽象类 QAbstractItemModel
{
 public:
         int col,rw;         //表格模型的列数和行数

         /*
          * 这是模型管理的数据
          * 数据：实际数据可使用 QList、数组、整型、或单独的一个类来保存，数据可存放在模型中，也可存放在文件等其他地方
          */
         QList<QVariant> s1;


         QList<int> rol;     //存储数据的角色

         //构造函数表示创建一个 i 行 j 列的表格模型
         D(int i, int j):rw(i),col(j)
         {
             for(int k = 0; k < col*rw; k++)
             {
                 s1.push_back(QVariant()); //这里是将表格的所有数据都初始化并存储到Qlist中
                 rol.push_back( -1);
             }
         }

         //1 返回表格模型的行数
         int rowCount(const QModelIndex & parent = QModelIndex()) const
         {
             return rw;
         }

         //2 返回表格模型的列数
         int columnCount(const QModelIndex & parent = QModelIndex()) const
         {
             return col;
         }

         //3 返回表格模型的父索引，因为所有单元格都是顶级节点，所以使用无效节点作为父节点
         QModelIndex parent(const QModelIndex &index) const
         {
              return QModelIndex();
         }

         //4 为每个单元格创一个唯一的索引
         QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
         {
            //帮助手册上写的就是让这么做
            return createIndex(row, column, (void*)&(s1.at(row*column+column)));
         }

         //5 返回视图上显示的数据，该函数会被视图多次调用(注：其他虚函数同样会被 Qt 调用多次)
         QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
         {
//            cout<<"QVariant data role =  "<<role<<endl;

            int i = index.row()*col+index.column(); //单元格所在数据 s1 中的位置

            if(role==Qt::CheckStateRole)
            {//处于选中状态的角色
                return QVariant();
            }

            if(role==Qt::TextAlignmentRole)
            {//文本对齐角色
                return Qt::AlignCenter;
            }

            if(role==Qt::DecorationRole)
            {//设置角色 DecorationRole(图片)的数据

               //若用户设置了 DecorationRole 角色的数据，则返回用户为该单元格设置的数据。
               if(rol.at(i)==Qt::DecorationRole)
               {
                   return s1.at(i);
               }
            }

            if(role==Qt::EditRole|role==Qt::DisplayRole)
            {//若用户为角色 EditRole 或 DisplayRole 角色设置了数据，则返回用户为该单元格设置的数据。
                if(rol.at(i)==Qt::EditRole|rol.at(i)==Qt::DisplayRole)
                {
                    return s1.at(i);
                }
            }

            //其余角色使用无效数据
             return QVariant();
         }

         //6 重载 setData 以使用户可以向模型中添加数据
         bool setData(const QModelIndex &index,const QVariant &value, int role = Qt :: EditRole)
         {
             s1.replace(index.row()*col+index.column(),value);
             rol.replace(index.row()*col+index.column(),role);
             emit dataChanged(index, index); //数据改变后，需要发送此信号。
             return true;
        }


        //重新实现有关拖拽的函数
        //1、重新实现 flags 函数，以开启模型的拖放功能
        Qt::ItemFlags flags(const QModelIndex &index) const
        {
          return Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled|Qt::ItemIsSelectable;
        }

        //2、重新实现 supportedDropActions 以使模型即可复制又可移动
        Qt::DropActions supportedDropActions() const
        {
            return Qt::CopyAction|Qt::MoveAction;
        }

        //3、重新实现 canDropMimeData 函数，以决定单元格是否允许放置拖动过来的数据。
        bool canDropMimeData(const QMimeData *data,
                             Qt::DropAction action,
                             int row, int column,
                             const QModelIndex &parent) const
        { /*本示例 parent 参数有效，row 和 column 是无效的，以下代码表示，第 3 行第 3 列不接受来自拖放的数据。*/
           if(parent.row()==2&&parent.column()==2)
               return false;
           else
               return true;
        }

        //4、重新实现 mimeData 函数，以编码拖动时的数据(就是把拖动时的数据保存起来，并返回)
        QMimeData* mimeData(const QModelIndexList &indexes) const
        {
           QMimeData *const pm = new QMimeData(); //创建一个 QMimeData 对象。
           /*注意：indexes 是被拖动的数据项的索引，若同时拖动了多个数据项，则 indexes 才会包含多个数据项。*/
           for(int k=0;k<indexes.size();k++)
           {
             const QModelIndex &i=indexes.at(k);
             if (i.isValid())
             {
               QByteArray t;
               //提取数据项的索引为 i 角色为 DisplayRole 的数据
               QVariant v = data(i, Qt::DisplayRole);
               t.append(v.toString()); //把提取的数据保存在 t 之中
               pm->setData("text/plain", t); //然后把 t 中的数据以 MIME 类型的形式编码到 pm 中。
             } //if 结束
           } //for 结束
           return pm;
        } //返回编码后的数据

        //5、重新实现 dropMimeData 函数，以解码拖动时的数据并对其进行处理
        bool dropMimeData(const QMimeData *data,
                          Qt::DropAction action,
                          int row, int column,
                          const QModelIndex &parent)
        {/* 本示例 parent 参数有效，row 和 column 无效，
            以下代码表示保存单元格所在数据 s1 中的位置。索引 parent 是拖动之后需要放置的位置的索引。*/
            int i=parent.row()*col+parent.column();
             QByteArray t=data->data("text/plain"); //解码由 mimeData 函数编码后的数据。

            //若是复制操作，则把新位置 parent 处的数据重置为 t
            if(action==Qt::CopyAction)
            {
                setData(parent,t);
            }

            //若是移动操作，则把新位置 parent 处的数据重置为 t
            if(action==Qt::MoveAction)
            {
                setData(parent,t);
            }

            return true;
        } //返回 true 表示数据已处理。

        //==================================================
        bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex())
        {
            qDebug()<<"col = "<<col;
            beginInsertColumns(parent, col,col);  //低效的方法

            //emit layoutAboutToBeChanged(QAbstractItemModel::VerticalSortHint);

            s1.append(QVariant());
            rol.append(-1);

            endInsertColumns();
            col++;
            return true;
        }

};

D *d;
QTableView *pv2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    d=new D(1,5);
    pv2=new QTableView(this); //使用表格视图来显示以上模型管理的数据

   //向模型中添加数据
    d->setData(d->index(0,0,QModelIndex()),"111",Qt::EditRole);
    d->setData(d->index(0,1,QModelIndex()),"111",Qt::EditRole);
    d->setData(d->index(0,2,QModelIndex()),"111",Qt::EditRole);
    d->setData(d->index(0,3,QModelIndex()),"111",Qt::EditRole);
    d->setData(d->index(0,4,QModelIndex()),"111",Qt::EditRole);
    //委托增加模型
    pv2->setModel(d);
    pv2->setDragEnabled(true);
    pv2->viewport()->setAcceptDrops(true);
    pv2->setDropIndicatorShown(true);
    pv2->setDragDropMode(QAbstractItemView::DragDrop);
    pv2->resize(900,500);
    pv2->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    d->insertColumns(3, 1);  //这个地方没有生效
    pv2->setModel(d);
    pv2->show();
}
