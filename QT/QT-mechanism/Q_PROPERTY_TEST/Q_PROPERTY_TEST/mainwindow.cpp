#include "mainwindow.h"
#include "ui_mainwindow.h"

class C{};

Q_DECLARE_METATYPE(C);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myclass m;


    /*QVariant 没有专门的 char 构造函数，此处的字符 a 会被转换为 int 型，因此 v中存储的是数值 97，而不是字符 a 。*/
    QVariant v('a');

    qDebug()<<v.value<int>()<<endl;  //输出 97
    qDebug()<<v.value<char>()<<endl; //输出 a，将 97 转换为 char 型，并输出转换后的值。
    qDebug()<<v.toChar().toLatin1()<<endl; /*输出 a，原因同上，注意 toChar 返回的类型是 QChar 而不是 char。*/
    qDebug()<<v.toString().toStdString().data()<<endl; /*输出 97，把存储在 v 中的值转换为 QString，然后以字符串形式输出。*/
    qDebug()<<v.typeName()<<endl; //输出 int，可见存储在 v 中的值的类型为 int
    qDebug()<<v.typeToName(v.type())<<endl; /*输出 int，其中 type 返回存储值的枚举形式表示的类型，而typeToName 则以字符串形式显示该枚举值所表示的类型。*/


    char c='b';
    v.setValue(c);
    qDebug()<<v.toString().toStdString().data()<<endl; //输出 b

    /*输出 char，若是使用 QVariant 构造函数和直接赋值 char 型字符，此处会输出 int，这是 setValue 与 构造函数和直接赋值 的区别。*/
    qDebug()<<v.typeName()<<endl;



    C mc; //自定义类型 C 的对象 mc 在有Q_DECLARE_METATYPE声明的情况下  构造函数和赋值运算符都是无效的只有setValue可以
    //QVariant v1(mc); //错误，没有相应的构造函数。
    QVariant v2;
    //v2=mc; //错误，没有与类型 C 匹配的赋值运算符函数。
    v2.setValue(mc); //错误，自定义类型 C 未使用宏 Q_DECLARE_METATYPE 声明。

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

}
