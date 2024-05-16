#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma execution_character_set("utf-8")


typedef struct WIDGET
{
    HWND hwnd;
    RECT rctA;
}widget;

QVector<widget> vechwnd;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_blMagnifierShow(false)
{
    ui->setupUi(this);

    //播放视频做准备
    m_pMediaPlayer = new QMediaPlayer(this);
    m_pMediaPlaylist = new QMediaPlaylist(m_pMediaPlayer);
    m_pMediaPlayer->setVideoOutput(ui->w1);
    ui->w1->setAspectRatioMode(Qt::IgnoreAspectRatio);


    //放大镜界面初始化
    m_pMagnifier = new QLabel(this);
    m_pMagnifier->hide();
    m_pMagnifier->setStyleSheet("border : 1px solid");  //设置一个一像素的黑边框
    m_pMagnifier->setScaledContents(true);              //界面内容可以铺满
    m_pMagnifier->setAlignment(Qt::AlignCenter);        //界面内容可以水平垂直居中
    m_pMagnifier->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput);//无边框 置顶  鼠标点击透传
    this->installEventFilter(this);
    m_pMagnifier->installEventFilter(this);             //主界面通过事件过滤器监控m_pMagnifier事件
    connect(this,SIGNAL(SignMagnifyArea(QPoint)),this,SLOT(SlotMagnifyArea(QPoint))); //信号的连接

    //初始化界面视频窗口
    m_pwgtVideoWindow = new CLS_WgtVideoWindows;

    QHBoxLayout * phlayout = new QHBoxLayout;
    phlayout->addWidget(ui->w1);
    phlayout->addWidget(ui->w2);

    QHBoxLayout * phlayout2 = new QHBoxLayout;
    phlayout2->addStretch();
    phlayout2->addWidget(ui->btn_play);
    phlayout2->addWidget(ui->btn_tobig);
    phlayout2->addWidget(ui->btn_gethandle);
    phlayout2->addStretch();

    QVBoxLayout * pvlayout = new QVBoxLayout;
    pvlayout->addLayout(phlayout);
    pvlayout->addWidget(m_pwgtVideoWindow);
    pvlayout->addLayout(phlayout2);
    pvlayout->addStretch();
    this->centralWidget()->setLayout(pvlayout);

    //视频窗口控件的注释
    ui->w1->setToolTip("这是QVideoWidget类");
    ui->w2->setToolTip("这是QWidget类");

    m_pUpdateTimer = new QTimer(this);//定时器
    m_pUpdateTimer->setInterval(1000);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(SlotTimeout()));
    m_pUpdateTimer->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
   if((watched == this || watched == m_pMagnifier)&&
      m_blMagnifierShow &&
      (event->type() == QKeyEvent::ShortcutOverride || event->type() == QKeyEvent::KeyPress))
   {
        QKeyEvent * p = (QKeyEvent *)event;
        if(p && p->key() == Qt::Key_Escape)
        {//esc键被按下 放大预览界面控件关闭 标志量重新赋值
            m_pMagnifier->close();

            //主界面窗体centralWidget()禁用接收鼠标事件
            this->centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            m_blMagnifierShow = !m_blMagnifierShow;

            return true;
        }
   }

   return QMainWindow::eventFilter(watched, event);
}

void MainWindow::SlotTimeout()
{
   if(m_blMagnifierShow)
   {
       QPoint glbmousepos = QCursor::pos();
       emit SignMagnifyArea(glbmousepos);
   }
}


void MainWindow::SlotMagnifyArea(QPoint _qMousePos)
{
    if(m_blMagnifierShow == false)
    {//不使用放大功能则退出函数
        return;
    }

    if(_qMousePos.isNull())
    {//鼠标坐标位置无效退出
        return;
    }

    if(m_pMagnifier == nullptr)
    {//指针判空
        return;
    }

    //=============================================================================================
    //【TODO 问题的关键 截取整个界面的图片中w1窗口并没有显示出来】
    //【TODO QVideoQWidget 和 QWidget的区别】
    //获取控件的界面完整的截图(不带界面标题栏)

    //方法1:  不会显示w1窗口
//    QPixmap pixmap = this->grab(QRect(0,0,this->width(), this->height()));


    //方法2:  可以显示
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow(0);

    //=============================================================================================

    //获取鼠标位置的x y 坐标值
    int bx = _qMousePos.x();
    int by = _qMousePos.y();

    //现在判断鼠标位置相对于主界面的位置
    QPoint glbMousePoint = _qMousePos;
    int iglbMouseX = glbMousePoint.x();
    int iglbMouseY = glbMousePoint.y();

//=====================================================================================
//    QPoint glbMainTLPoint = this->geometry().topLeft();  //主界面的上下左右四个边界值
//    int iglbMainLeft = glbMainTLPoint.x();
//    int iglbMainTop  = glbMainTLPoint.y();


//    QPoint glbMainBRPoint = this->geometry().bottomRight();
//    int iglbMainRight = glbMainBRPoint.x();
//    int iglbMainBottom = glbMainBRPoint.y();

    int iglbMainLeft = 0;
    int iglbMainTop  = 0;
    int iglbMainRight  =  QApplication::desktop()->width();
    int iglbMainBottom =  QApplication::desktop()->height();
//=====================================================================================

    //计算鼠标位置距离主界面边界的距离  正常情况下  左边界 < x值  < 右边界       上边界 < y值 < 下边界  所以距离起码要大于0
    int iMouseDisLeft   = iglbMouseX - iglbMainLeft;  //距离左边界值
    int iMouseDisRight  = iglbMainRight - iglbMouseX; //据右边界值
    int iMouseDisTop    = iglbMouseY - iglbMainTop;   //据上边界值
    int iMouseDisBottom = iglbMainBottom - iglbMouseY;//据下边界值


    if(iMouseDisLeft < 0 || iMouseDisRight < 0 || iMouseDisTop < 0 || iMouseDisBottom < 0)
    {//鼠标位置距离主界面边界的距离异常 退出
        return;
    }


    //从exe界面截图中裁剪需要放大区域  放大区域的宽高是固定的除非修改CROP_WIDTH_DEF CROP_HIGH_DEF
    //这里需要说明一下，如果鼠标位置相距主界面边界小于 MOUSE_BORDER_X_DEF(X坐标)  MOUSE_BORDER_Y_DEF(Y坐标)，则裁剪出的图片不会是CROP_WIDTH_DEF *  CROP_HIGH_DEF像素
    //因此需要使用QPainter来补全图片


    //从exe界面裁剪出一个图片
    int cropstartx =  _qMousePos.x() - CROP_WIDTH_DEF / 2;  //裁剪区域起始位置 x
    int cropstarty =  _qMousePos.y() - CROP_HIGH_DEF  / 2;  //裁剪区域起始位置 y
    QPixmap final = pixmap.copy(cropstartx,cropstarty,CROP_WIDTH_DEF,CROP_HIGH_DEF);


    //获取裁剪出来图片的宽高
    int pixwidth = final.width();
    int pixhigh  = final.height();


    //判断图片是否需要补全
    QPoint drawStart(0,0);
    bool blcomplet = true;  //true需要补全   false不需要补全
    if(iMouseDisLeft <= MOUSE_BORDER_X_DEF)
    {//鼠标位于左边界
        drawStart.setX(CROP_WIDTH_DEF - pixwidth);
        if( iMouseDisTop <= MOUSE_BORDER_Y_DEF)
        {//且鼠标位于上边界
            drawStart.setY(CROP_HIGH_DEF - pixhigh);
        }
        //鼠标位于下边界Y不用处理
    }
    else if(iMouseDisRight <= MOUSE_BORDER_X_DEF)
    {//鼠标位于右边界X不用处理
        if(iMouseDisTop <= MOUSE_BORDER_Y_DEF)
        {//且鼠标位于上边界
            drawStart.setY(CROP_HIGH_DEF - pixhigh);
        }
        //鼠标位于下边界Y不用处理
    }
    else if(iMouseDisTop <= MOUSE_BORDER_Y_DEF)
    {//鼠标位于上边界
        drawStart.setY(CROP_HIGH_DEF - pixhigh);
    }
    else if(iMouseDisBottom <= MOUSE_BORDER_Y_DEF)
    {//鼠标位于下边界 Y不用处理
    }
    else
    {//鼠标位于中央区域
        blcomplet = false;
    }

    if(blcomplet)
    {//绘制一个背景界面用以补全

        QPainter qpainter;
        QPixmap backpix(CROP_WIDTH_DEF,CROP_HIGH_DEF);
        qpainter.begin(&backpix);
        QBrush qbrush(QColor(255,255,0));
        qpainter.setBrush(qbrush);
        qpainter.setPen(Qt::NoPen);
        qpainter.drawRect(QRect(0,0,CROP_WIDTH_DEF,CROP_HIGH_DEF));
        qpainter.end();


        QImage finalImage = final.toImage();
        qpainter.begin(&backpix);
        qpainter.drawPixmap(drawStart,final,QRect(0,0,CROP_WIDTH_DEF,CROP_HIGH_DEF));
        qpainter.end();


        QBitmap bp = backpix.createMaskFromColor(qRgb(255,255,0),Qt::MaskInColor);
        backpix.setMask(bp);


        final = backpix;
    }


    //将裁剪好的图片设置到显示控件QLabel
    final.scaled(QSize(PREVIEW_WIDTH_DEF,PREVIEW_HIGH_DEF), Qt::KeepAspectRatio);
    m_pMagnifier->setPixmap(final);


    //设置放大预览界面
    QPoint qplabel(bx-PREVIEW_WIDTH_DEF/2,by-PREVIEW_HIGH_DEF/2);

    m_pMagnifier->setGeometry(qplabel.x(),qplabel.y(),PREVIEW_WIDTH_DEF,PREVIEW_HIGH_DEF);
}


void MainWindow::on_btn_tobig_clicked()
{
    if(m_pMagnifier == nullptr)
    {
        return;
    }

    m_blMagnifierShow = !m_blMagnifierShow;

    //判断是否要显示放大预览界面
    if(m_blMagnifierShow)
    {
        //主界面窗体centralWidget()禁用接收鼠标事件
        this->centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_pMagnifier->show();
    }
    else
    {
        this->centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_pMagnifier->hide();
    }
}


void MainWindow::on_btn_play_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"open file", ".", "all(*.*)");

    if(path.isEmpty())
    {
        return;
    }

    if(m_pMediaPlaylist == nullptr || m_pMediaPlayer == nullptr)
    {
        return;
    }


    m_pMediaPlaylist->clear();
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" path = "<<path;
    m_pMediaPlaylist->addMedia(QUrl::fromLocalFile(path));
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<"=============================";
    m_pMediaPlaylist->setCurrentIndex(0);
    m_pMediaPlayer->setPlaylist(m_pMediaPlaylist);
    m_pMediaPlayer->play();
}


/* 回调函数，用于捕获进程  */
BOOL CALLBACK  MyEnumProc(HWND hwnd, LPARAM param)
{
    LPWSTR lpString = (LPWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
    if (IsWindow(hwnd) &&
        IsWindowEnabled(hwnd) &&
        IsWindowVisible(hwnd))
    {
        if (GetWindowTextW(hwnd, lpString, MAX_PATH) > 0)
        {
           QWidget * pwgt = QWidget::find((int)hwnd);
//           if(pwgt)
//           {//通过这种操作不仅可以将句柄转换成界面，而且还可以筛选出来qt界面
//               qDebug()<<"***这是qt窗口***";
//           }

           RECT rctA;
           ::GetWindowRect(hwnd,&rctA);

           if(rctA.top >= 0 && rctA.left >= 0 && rctA.bottom >= 0 &&  rctA.right >= 0)
           {
               QString title = QString::fromStdWString(lpString);
               if(title != "Microsoft Text Input Application" && title != "设置")
               {
//                    //保存显示出来的窗口截图
//                    QScreen *screen = QGuiApplication::primaryScreen();
//                    QPixmap pixmap = screen->grabWindow((WId)hwnd);
//                    pixmap.save("./pic/"+ QString::number((int)hwnd)+".png");


                    // 打印出有标题的窗口
                    qDebug() << " [HWND] = "<<(int)hwnd<<" [title] = "<< title;


                      widget wgt;
                      wgt.hwnd = hwnd;
                      wgt.rctA = rctA;

                      vechwnd.push_back(wgt);
               }

            }
        }
    }
    free(lpString);
    return TRUE;
}

void MainWindow::on_btn_gethandle_clicked()
{
    ::EnumWindows(MyEnumProc, 0);  //获取winodows系统所有窗口的句柄

    qDebug()<<"==================================================";

    QPixmap res(1920,1080);
    res.fill(Qt::transparent);

    QPixmap pixmap;
    for(int i = vechwnd.size(); i > 0 ; --i)
    {
        QPainter qpainter(&res);

        HWND hwnd = vechwnd[i-1].hwnd;

        qDebug() << " [HWND] = "<<(int)hwnd;

        RECT rctA = vechwnd[i-1].rctA;


        //保存显示出来的窗口截图
        QScreen *screen = QGuiApplication::primaryScreen();
        pixmap = screen->grabWindow((WId)hwnd);

        qpainter.drawPixmap(rctA.left, rctA.top, rctA.right - rctA.left, rctA.bottom - rctA.top, pixmap);

    }

    res.save("./pic/finally.png");
}

