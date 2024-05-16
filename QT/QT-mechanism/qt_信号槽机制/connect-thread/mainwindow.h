/*
 *   本工程用于调研QThread运行子线程的两种方式和connect两种连接方式组合起来的效果
 *   connect的连接方式可以通过修改public_data.h内容来修改
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MoveToThread.h"
#include "InheritThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    MoveToThread * m_pth2obj;
    QThread        * m_pth2;

    InheritThread * m_pth1;

public slots:
    void DealThread1TimeOut();
    void DealThread2TimeOut();

signals:
    void SigNoticeThread2StartWork();

private slots:
    void on_pushButton_clicked();

};
#endif // MAINWINDOW_H
