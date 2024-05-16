#include "mainwindow.h"

#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QDateTime>

#include "exception_handler.h"

using namespace google_breakpad;


//breakpad需要的回调函数
bool callback(const wchar_t *dump_path,
              const wchar_t *id,
              void *context,
              EXCEPTION_POINTERS *exinfo,
              MDRawAssertionInfo *assertion,
              bool succeeded)
{
    if (succeeded)
    {//注意!! 进到if中说明程序崩溃已经发生，此时这里不应该再进行耗时或者堆栈操作的行为

        //最后存储dump文件的路径 xx/dump_path/xxxx-xx-xx-xx-xx-xx-xxx
        //毫秒级别存储dump文件
        QString path = QString::fromWCharArray(dump_path) + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
        QDir qdir(path);
        if(!qdir.exists())
        {
            qdir.mkdir(path);
        }

        //旧的dump文件的路径 路径+文件
        QString qoldfile = QString::fromWCharArray(dump_path) + "/" + QString::fromWCharArray(id) + ".dmp";

        //新的dump文件的路径 路径+文件
        QString qnewfile = path+"/"+QString::fromWCharArray(id) + ".dmp";

        //剪切dump文件到新的目录
        QFile qfile(qoldfile);
        if(qfile.exists())
        {
            if(qfile.rename(qoldfile,qnewfile))
            {
                qDebug() << "cut dump file success";
            }
        }

        qDebug() << "Create dump file success";
    }
    else
    {
        qDebug() << "Create dump file failed";
    }
    return succeeded;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建捕捉程序异常对象
    QString qstrpath = "C://Dumps";  //创建目录Dumps ，用于存储所有dump文件
    QDir qdir(qstrpath);
    if(!qdir.exists())
    {
        qdir.mkdir(qstrpath);
    }

    ExceptionHandler eh(qstrpath.toStdWString(),
                        NULL,
                        callback,
                        NULL,
                        ExceptionHandler::HANDLER_ALL);

    MainWindow w;
    w.show();
    return a.exec();
}
