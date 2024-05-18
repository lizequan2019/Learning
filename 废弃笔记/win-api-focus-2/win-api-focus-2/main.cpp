/*
 * 本例说明 : windwos下的界面处理离不开对系统窗口的控制，本例通过调用windows api
 *           实现获取置顶窗口的句柄和标题
 */
#include <QCoreApplication>
#include <Windows.h>
#include <WinUser.h>
#include <QThread>
#include <QDebug>

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    while(1)
    {
        QString qstrText;

        qstrText.clear();
        HWND hForgroundWnd = GetForegroundWindow(); //获取激活的窗口句柄

        if(hForgroundWnd)
        {
            wchar_t szCaption[MAX_PATH] = { 0 };
            GetWindowText(hForgroundWnd, szCaption, MAX_PATH * sizeof(WCHAR));
            std::wstring strCaption = szCaption;
            int winId = (int)hForgroundWnd;
            qstrText  =  "窗口句柄 = " + QString::number(winId) +"  窗口标题"+ QString::fromStdWString(strCaption);
        }

        qDebug()<<qstrText;

        QThread::msleep(500);
    }

    return a.exec();
}
