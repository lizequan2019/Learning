#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include "CLS_SshManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private Q_SLOTS:


    void SlotRecMsg(QString); //获取执行shell命令并显示

    void on_btn_execshell_clicked();  //执行shell命令

    void on_btn_uploadfile_clicked(); //上传文件

    void on_btn_opendir_clicked();    //打开目录选择上传文件

private:
    Ui::MainWindow *ui;

    CLS_SshManager * m_pSshmagr;

     QFileDialog * m_pfileDlg;
};
#endif // MAINWINDOW_H
