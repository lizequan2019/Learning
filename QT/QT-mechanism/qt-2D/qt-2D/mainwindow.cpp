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

/*
   若绘制设备是一个QWidget部件, 则QPainter只能在paintEvent()处理函数中使用(即,需要子类化QWidget部件,并重新实现该函数)
 */
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.drawLine(QPoint(11,11),QPoint(20,20));

    QPoint p1(11,50);
    QPoint p2(111,50);
    QPoint p3(11,60);
    QPoint p4(111,60);
    QPoint p[]={p1,p2,p3,p4};
    painter.drawLines(p,2);

    QLineF n1(11.1,100.1,111.1,100.1);
    QLineF n2(11.1,120.1,111.1,120.1);
    QVector<QLineF> v1; v1.append(n1); v1.append(n2);
    painter.drawLines(v1);




    QRectF rectangle(200,200,200,100);
    painter.drawRect(rectangle);
    int startAngle = 0;
    int spanAngle = 180 * 16;
    painter.drawArc(rectangle, startAngle, spanAngle);

    rectangle.setRect(200,350,200,100);
    painter.drawRect(rectangle);
    startAngle = 60 * 16;
    spanAngle = 30 * 16;
    painter.drawPie(rectangle, startAngle, spanAngle);

    rectangle.setRect(200,500,200,100);
    painter.drawRect(rectangle);
    startAngle = 0 * 16;
    spanAngle = 120 * 16;
    painter.drawChord(rectangle, startAngle, spanAngle);



//    QPointF p5[] = {QPointF(500,500), QPointF(600,500), QPointF(515,550), QPointF(550,465),QPointF(585,550) };
//    painter.drawPolygon(p5, 5);

    QPointF p6[] = {QPointF(700,700), QPointF(800,700), QPointF(715,750), QPointF(750,665),QPointF(785,750) };
    painter.drawPolyline(p6, 5);

    QPointF p7[] = {QPointF(700,700), QPointF(750,665), QPointF(800,700), QPointF(785,750),QPointF(715,750) };
    painter.drawConvexPolygon(p7, 5);


    QPainterPath ph;
    QBrush bs(QColor(1,188,1));
    painter.setBrush(bs);
    ph.moveTo(QPointF(500,600));
    ph.lineTo(QPointF(500,500));
    ph.lineTo(QPointF(600,500));
    ph.moveTo(QPointF(600,500));
    ph.lineTo(QPointF(600,600));
    painter.drawPath(ph);



    painter.drawText(QPointF(300,40), "hell \n world");

    QRect r1(300,70,55,133);
    painter.drawText(r1, "F&AaaFF ddd3213213131111111111111111");

    QRect r2(300,100,55,133);
    painter.drawText(r2, Qt::TextDontClip|Qt::TextWordWrap,"F&AaaFF ddd3213213131111111111111111");




    //设置笔刷颜色
    QBrush bs2(QColor(188,1,1)); painter.setBrush(bs2);
    //设置字体
    QFont f;  f.setPointSize(22);  painter.setFont(f);

    //绘制矩形，注意绘制顺序。
    QRectF r5(600,70,55,77);
    QRectF r6(600,70,177,99);

    //绘制带矩形的文本
    painter.drawRect(r6);  //大矩形绘制在上面
    painter.drawRect(r5);  //小矩形绘制在下面


    //在给定的矩形内绘制文本。矩形和对齐标志定义了文本的定位。
    painter.drawText(r5, Qt::TextShowMnemonic|Qt::TextWordWrap|Qt::AlignBottom,"F&AaaFF ddd",&r6);
    painter.drawText(QPointF(600,70),"FFFFF\n DD");
    painter.drawStaticText(QPointF(700,70),QStaticText("XXX"));



    QLinearGradient g(QPointF(0.00,0.00),QPointF(1.00,1.00));//  起始/终点坐标以小数点指定
    g.setCoordinateMode(QGradient::ObjectBoundingMode); //设置渐变坐标模式
    g.setColorAt(0,QColor(255,255,255)); //渐变起始颜色
//    g.setColorAt(0.5,QColor(111,1,1));
    g.setColorAt(1,QColor(255,255,0));//渐变最终停止点颜色
    g.setSpread(QGradient::PadSpread); //设置渐变色传播模式

    //创建画刷
    QBrush lineBrush(g);
    painter.setBrush(lineBrush);
    painter.drawRect(700,200,111,111); //渐变填充区域 1
    painter.drawRect(700,340,111,111); //渐变填充区域 2
//    painter.drawLine(66.1,0,66.1,444);
//    painter.drawLine(110.5,0,110.5,444); //用于测量
//    painter.drawLine(233.1,0,233.1,444);
//    painter.drawLine(277.5,0,277.5,444); //用于测量




    QLinearGradient h(QPointF(11,11),QPointF(155,11));

    h.setColorAt(0,QColor(111,255,255));
     //青色
    h.setColorAt(0.5,QColor(111,1,1));
     //红色
    h.setColorAt(1,QColor(255,255,1));
     //黄色
    h.setSpread(QGradient::RepeatSpread);

    //创建两个画刷
    QBrush lineBrush2(g);
    QBrush lineBrush3(QColor(1,111,1));

    //设置画笔
    QPen pn;
    pn.setCapStyle(Qt::FlatCap); //该端头样式可消除端头延长部分的影响
    pn.setWidth(10);
     //线宽
    pn.setStyle(Qt::DashLine);   //设置为虚线
    pn.setBrush(lineBrush2);     //设置画笔的画刷为 bs(渐变色)
    painter.setPen(pn);          //设置画笔

    //设置字体
    QFont f1;
    f.setPointSize(33);
    painter.setFont(f);
    painter.setBackground(lineBrush3);         //设置背景画刷为 lineBrush3(纯色)
    painter.setBackgroundMode(Qt::OpaqueMode); //设置背景模式为不透明
    painter.drawText(11,800,"FFXXXXF");        //绘制文字
    painter.drawLine(11,750,513,750);            //绘制直线


   //====================================

    QPainter painter2;
    painter2.begin(this);

    //设置笔刷
    QBrush bstrnas(QColor(255,255,1));
    painter2.setBrush(bstrnas);

    //绘制矩形
    QRectF r(0,11,22,55);
    painter2.drawRect(r);      //变换 0:坐标原点(0,0)


    painter2.translate(50,10); //平移
    painter2.drawRect(r);      //绘制矩形  坐标原点(50,10)



    painter2.save(); //保存状态


    //变换 2:
    painter2.translate(50,50); //平移,缩放(Y 向 2 倍),旋转(逆时针)
    painter2.scale(1,2);
    painter2.rotate(-60);
    painter2.drawRect(r);      //坐标原点(100,60)


    painter2.restore();        //恢复状态,此时坐标系位于(50,10)处

    //变换 3:
    painter2.translate(100,0); //平移
    painter2.drawRect(r);      //坐标原点(150,10)

    //变换 4:
    painter2.translate(50,0);  //平移,旋转(逆时针),缩放(Y 向 2 倍),注意连续变换与变换 2 的顺序。
    painter2.rotate(-60);
    painter2.scale(1,2);
    painter2.drawRect(r);      //坐标原点(200,10)


    //重置为初始状态
    painter2.resetTransform();


    //变换 5:
    painter2.translate(200,50);
    painter2.drawRect(r); //坐标原点(200,50);

    painter2.end();
    painter.end();
    /*
        //创建 pr 对象,并立即开始在部件 this 上绘制图形。
        QPainter pr(this);
        //绘制一条从点(11,11)到点(111,111)的直线
        pr.drawLine(QPoint(11,11), QPoint(111,111));
        //函数结束时,在 QPainter 的析构函数中调用 end()函数结束绘制。
    */
}

