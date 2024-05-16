#include "mainwindow.h"
#include "ui_mainwindow.h"

//宏定义 区分方式1和方式2
//#define ModeOne 1

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef ModeOne
    /*方式一
     *  这种方式即使客户端没有成功连接到breakpad服务，在崩溃时也会自动捕获dump文件，
     *  如果连接到breakpad服务则由服务完成捕获崩溃
     *
     *  对于dump文件生成路径的赋值需要注意
     *     (1)如果连接到breakpad服务，那么路径不起作用，即使赋值为空也行
     *     (2)如果没有连接到breakpad服务，那么路径必须要赋有效值，否则程序会报错
     */

    //dump文件生成路径
    const wstring DumpPath = L"C:\\ClientDumps\\";

    handler = new ExceptionHandler(DumpPath,
                                   NULL,
                                   ShowDumpResults,
                                   NULL,
                                   ExceptionHandler::HANDLER_ALL,
                                   MiniDumpNormal,
                                   kPipeName,
                                   &custom_info);
    if(handler == nullptr)
    {
        qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<" new ExceptionHandler error";
    }

#else
    /*方式二
     *   不连接breakpad服务则不能捕获dump，但是程序还可以正常运行
     *   dump文件路径值不起作用
     */
    crash_generation_client = new CrashGenerationClient(kPipeName,MiniDumpNormal,&custom_info);
    if (crash_generation_client && crash_generation_client->Register())
    {
          const wstring DumpPath;
          handler = new ExceptionHandler(DumpPath,
                                         NULL,
                                         ShowDumpResults,
                                         NULL,
                                         ExceptionHandler::HANDLER_ALL,
                                         crash_generation_client);

          if(handler)
          {
              qDebug()<<"IsOutOfProcess = "<<handler->IsOutOfProcess();
          }
          else
          {
              qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<" new ExceptionHandler error";
              delete crash_generation_client;
              crash_generation_client = nullptr;
          }
    }
    else
    {
        qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<" crash_generation_client Register error";
    }
#endif
}

MainWindow::~MainWindow()
{
    if(handler)
    {
        delete handler;
        handler = nullptr;
    }

    if(crash_generation_client)
    {
        delete crash_generation_client;
        crash_generation_client = nullptr;
    }

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
#define C 1

    #ifdef A
            //除0异常
            int a = 10;
            int b = 0;
            int c = a/b;
            qDebug()<<"c = "<<c;
    #endif

    #ifdef B
            //数组越界
            int arr[10] = {1,2,3,4,5,6,7,8,9,10};
            qDebug()<<"arr[11] = "<<arr[11];

            vector<int> vec1 = {1,2,3,4};
            qDebug()<<"vec1[11] = "<<vec1.at(11);
    #endif

    #ifdef C
            //抛出异常
            try
            {
                int a = 10;
                int b = 0;
                int c = a/b;
                qDebug()<<"c = "<<c;
            }

            catch (...)
            {
                throw;
            }

    #endif
}

