#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "crash_generation_server.h"
#include "client_info.h"

using namespace google_breakpad;
using namespace std;

//崩溃捕获服务对象指针
static CrashGenerationServer* crash_server = nullptr;

//windows 有名管道
static const wchar_t kPipeName[] = L"\\\\.\\pipe\\BreakpadCrashServices\\TestServer";

//dump存储路径
static std::wstring dump_path = L"C:\\Dumps\\";

//客户端连接提示回调函数
static void ShowClientConnected(void* context, const ClientInfo* client_info)
{
    qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__;
}

//客户端崩溃提示回调函数
static void ShowClientCrashed(void* context, const ClientInfo* client_info, const wstring* dump_path)
{
    qDebug()<<" capture dump  from pid       = "<<client_info->pid();
    qDebug()<<" capture dump  from context   = "<<context;
    qDebug()<<" capture dump  from dump_path = "<<dump_path;
}

//客户端退出提示回调函数
static void ShowClientExited(void* context, const ClientInfo* client_info)
{
    qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__;
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
