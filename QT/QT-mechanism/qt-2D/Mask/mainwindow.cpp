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

void MainWindow::paintEvent(QPaintEvent *event)
{
//==================图片处理 使用颜色蒙版========================
//        QImage image(200,200,QImage::Format_RGB32);
//        QImage image2(100,100,QImage::Format_RGB32);

        QPixmap pix(200,200);   //最底下的图片
        QPixmap pix2(100,100);  //上面一层的图片

        QPainter painter;

        painter.begin(&pix);
        painter.setPen(Qt::NoPen);       //不设置画笔没有描边
        QBrush qbrush(QColor(255,255,0));
        painter.setBrush(qbrush);
        painter.drawRect(QRect(0,0,200,200)); //绘制最下面的图片
        painter.end();


        painter.begin(&pix2);
        painter.setPen(Qt::NoPen);
        qbrush.setColor(QColor(255,0,0));
        painter.setBrush(qbrush);
        painter.drawRect(QRect(0,0,100,100));  //绘制上面的图片
        painter.end();

        painter.begin(&pix);
        painter.drawPixmap(QPoint(100,100),pix2,QRect(0,0,200,200));
        painter.end();

        painter.begin(this);
        //创建颜色蒙版 被蒙板黑色部分遮挡的像素会被显示，被白色部分遮档的像素不会被显示
        QBitmap bp = pix.createMaskFromColor(qRgb(255,255,0),Qt::MaskOutColor);

        pix.setMask(bp);   //图片设置颜色模板
        painter.drawPixmap(0,0,pix);
        painter.end();
//==========================================




//   普通蒙版 遮罩的大小必须与此像素贴图的大小相同。
     QPainter pix3;
     QPixmap pm("./1z.png");     //设置加载图片

                                 //遮罩上的像素值为1(黑色)表示像素贴图的像素不变, 值为0(白色)表示像素是透明的。
     QBitmap bitmap("./1x.png"); //设置蒙版图片 1 蒙版图片要和被设置的图片一样大

     pm.setMask(bitmap);

     pix3.begin(this);
     pix3.drawPixmap(10,300,pm);
     pix3.end();

}

