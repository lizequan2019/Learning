#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QVector>
#include <vector>

//客户端demo 需要使用的头文件
#include "exception_handler.h"
#include "crash_generation_client.h"

using namespace google_breakpad;
using namespace std;


static ExceptionHandler* handler = nullptr;
static CrashGenerationClient * crash_generation_client = nullptr;

//windwos有名管道，与breakpad服务通讯
static const wchar_t kPipeName[] = L"\\\\.\\pipe\\BreakpadCrashServices\\TestServer";

//客户端的信息
static size_t kCustomInfoCount = 2;
static CustomInfoEntry kCustomInfoEntries[] =
{
    CustomInfoEntry(L"prod", L"CrashTestApp"),
    CustomInfoEntry(L"ver", L"1.0"),
};
static CustomClientInfo custom_info =
{
    kCustomInfoEntries,
    kCustomInfoCount
};

// 写入mini转储后要运行的回调函数。
static bool ShowDumpResults(const wchar_t* dump_path,
                     const wchar_t* minidump_id,
                     void* context,
                     EXCEPTION_POINTERS* exinfo,
                     MDRawAssertionInfo* assertion,
                     bool succeeded)
{
    qDebug()<<"======="<<__LINE__<<" "<<__FUNCTION__<<"=======";
    return succeeded;
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
