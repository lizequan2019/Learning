//#if _MSC_VER >= 1600 //vs2010及其以上版本
//#pragma execution_character_set("utf-8")  //这句是关键
//#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#define MY 1

//自定义结构体
class B
{
   public:
           QIcon i;
           QString s;
           QPoint p;
};


//重载数据流的 << >> 运算符
QDataStream &operator<<(QDataStream &out, const B &mb)
{
     out<<mb.i<<mb.s<<mb.p;
     return out;
} //内部代码比较简单

QDataStream &operator>>(QDataStream &in, B &mb)
{
    in>>mb.i>>mb.s>>mb.p;
    return in;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


//=============二进制数据序列化&反序列化========================
    QPushButton *pb = new QPushButton(QStringLiteral("AAA"),this);  //按钮

    QFile f(QStringLiteral("./1.txt")); //文件
    f.open(QIODevice::WriteOnly);
    QDataStream out(&f);

#ifndef MY
    //使用类中自带的<< >> 运算符
    QIcon i(QStringLiteral("./2.ico"));
    QPoint p(22,22);
    out <<i<<QString("BBB")<<p; //向数据流输出
#else
    //   读/写 自定义类型需重载>>和<<运算符
    B mb;
    mb.i=QIcon(QLatin1String("./3.ico"));
    mb.s=QStringLiteral("qqq");
    mb.p=QPoint(22,22);
    out<<mb;
#endif

    f.close(); //关闭文件


    //反序列化
    f.open(QIODevice::ReadOnly);
    QDataStream in(&f);

#ifndef MY
    QIcon i1;
    QString s;
    QPoint p1;
    in>>i1>>s>>p1;
#else
    B mb1;
    in>>mb1;
#endif

    pb->setIcon(mb1.i);
    pb->setText(mb1.s);
    pb->move(mb1.p);
    f.close();


//下面文本流的操作 需要在MinGw编译器或者设置execution_character_set("utf-8") 的msvc编译器环境下才能得到正确的答案


//===============下面是文本流 QTextStream的使用(偏重编码设置)==============

    //以GB码文本内容写入文件中
    QFile ftext(QStringLiteral("./GB2312.txt"));
    ftext.open(QIODevice::WriteOnly);
    QTextStream outtext(&ftext);

    //使用 QTextCodec::codecForName()函数设置编码的好处是可以根据返回值判断设置的编码是否有效。
    QTextCodec * textcode = QTextCodec::codecForName("GB2312");
    if(textcode != nullptr)
    {
       qDebug()<<"set gb2312 ok";
       outtext.setCodec(textcode);
    }

    outtext<<QString("这是一个 GB2312 编码文件");
    ftext.close();



//=======以下将检测文本内容是UTF-8还是其他编码=========

    QFile ff(QStringLiteral("./BIG5.txt")); //这里还可以打开其他编码的  【TODO读取Big5文件有?符号】
    ff.open(QIODevice::ReadOnly);
    QByteArray  input = ff.readAll();
    qDebug()<<" file content  =  "<<input;
    ff.close();


    //首先使用UTF8编码转换，根据转换状态决定是否进行其他编码转换
    QTextCodec::ConverterState state;
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(input.constData(),input.size(), &state);

    qDebug()<<"state.invalidChars = " << state.invalidChars;  //【TODO 这里的转换状态值没有搞懂大于0就不是UTF8编码】
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("Big5")->toUnicode(input);
    }
    else
    {
        text = input;
    }
    qDebug()<<"read big5 text = " << text;


    qDebug()<<"===================================================================";


    ff.setFileName(QStringLiteral("./GB2312.txt"));
    ff.open(QIODevice::ReadOnly);
    input = ff.readAll();
    qDebug()<<" file content  =  "<<input;
    ff.close();

    codec = QTextCodec::codecForName("UTF-8");
    text = codec->toUnicode(input.constData(),input.size(), &state);

    qDebug()<<"state.invalidChars = " << state.invalidChars;  //【TODO 这里的转换状态值没有搞懂大于0就不是UTF8编码】
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("GB2312")->toUnicode(input);
    }
    else
    {
        text = input;
    }
    qDebug()<<"read gb2312 text = " << text.toStdString().data();

    qDebug()<<"===================================================================";









//=================gbk 与 utf8 的文本输出======================
//编译器是MSVC
//Qt默认编码是utf-8  本地编码 windows(gbk)

//结论 :  在终端的输出都是把gbk编码的内容转换成utf8编码

//解决中文乱码的问题  不使用 #pragma execution_character_set("utf-8")



    const char * char_ = "中文";
    qDebug() << "char_ output to loacl = " << char_;   //不进行转换输出是乱码

    QString qstr = "中文2";
    qDebug() << "qstr output to loacl = " << qstr;


    //第一种方法
    //     获取当前编码
    //    qDebug()<<"  "<<QTextCodec::codecForLocale()->name();
    //    QTextCodec * pCodec = QTextCodec::codecForLocale();

    QTextCodec * pCodec = QTextCodec::codecForName("GB2312");
    if(pCodec)
    {
        QString qstr_utf8 = pCodec->toUnicode(char_);   //本地字符集GBK到Unicode的转换
        qDebug() << "1. qstr_utf8  putout to loacl = " << qstr_utf8;

        qstr_utf8 = pCodec->toUnicode(qstr.toStdString().data());      //QString 转换过来的输出的还是乱码
        qDebug() << "1.1 qstr_utf8  putout to loacl = " << qstr_utf8;

        const char * a = qstr.toStdString().data();                    //QString 转换过来的输出的还是乱码
        qstr_utf8 = pCodec->toUnicode(a);
        qDebug() << "1.2 qstr_utf8  putout to loacl = " << qstr_utf8;
    }

    //第二种方法
    QString qstr_utf8 = QString::fromLocal8Bit(char_);  //本地字符集GBK到Unicode的转换
    qDebug() << "2. qstr_utf8  putout to loacl = " << qstr_utf8;


    qstr_utf8 = QString::fromLocal8Bit(qstr.toStdString().data());  //QString 转换过来的输出的还是乱码
    qDebug() << "2.2 qstr_utf8  putout to loacl = " << qstr_utf8;


    //第三种方法
    qstr_utf8 = QStringLiteral("中文");
    qDebug() << "3. qstr_utf8  putout to loacl = " << qstr_utf8;
}

MainWindow::~MainWindow()
{
    delete ui;
}

