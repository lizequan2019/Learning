#include "mainwindow.h"
#include "ui_mainwindow.h"


//创建崩溃捕获服务器
bool CrashServerStart()
{
  //不会再创建服务器的其他实例。
  if (crash_server)
  {
       return false;
  }

  //路径的判断
  if (_wmkdir(dump_path.c_str()) && (errno != EEXIST))
  {
      return false;
  }

  //关键函数
  crash_server = new CrashGenerationServer(kPipeName,
                                           nullptr,
                                           ShowClientConnected,
                                           nullptr,
                                           ShowClientCrashed,
                                           nullptr,
                                           ShowClientExited,
                                           nullptr,
                                           nullptr,
                                           nullptr,
                                           true,
                                           &dump_path);
  if(crash_server == nullptr)
  {
      return false;
  }

  if (!crash_server->Start())
  {
        delete crash_server;
        crash_server = nullptr;
        return false;
  }

  return true;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if(crash_server)
    {
        delete crash_server;
        crash_server = nullptr;
    }
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(CrashServerStart())
    {
        qDebug()<<"CrashServerStart return true";
    }
    else
    {
        qDebug()<<"CrashServerStart return false";
    }
}

