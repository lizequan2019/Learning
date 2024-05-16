/*
 *  本例说明:  在应用中增加一个放大镜功能，放大鼠标周围的区域
 *            在放大的过程中不能影响正常功能，要注意窗口嵌套的问题(遇到一种就解决一种)
 *
 *  支持的平台: windwos
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <WinUser.h>
#include <wingdi.h>
#include <stdint.h>
#include <stdio.h>

#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QLabel>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QPoint>
#include <QKeyEvent>
#include <QPainter>
#include <QBitmap>
#include <QLayout>
#include <QTimer>

#include "CLS_WgtVideoWindows.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


//放大镜相关的宏定义
//放大的机制  预览界面大小时固定的  增加放大倍数则需要缩小裁剪界面的
//放大系数
const int DOUBLING = 2;   //为了数据好处理 设置为2的倍数

//预览界面默认大小
const int PREVIEW_WIDTH_DEF = 578;
const int PREVIEW_HIGH_DEF  = 332;

//裁剪界面默认大小
const int CROP_WIDTH_DEF = PREVIEW_WIDTH_DEF / DOUBLING;
const int CROP_HIGH_DEF  = PREVIEW_HIGH_DEF  / DOUBLING;

//鼠标距离主界面边界距离 如果小于设置的距离，那么裁剪界面将会根据情况进行调整
const int MOUSE_BORDER_X_DEF = CROP_WIDTH_DEF / 2;
const int MOUSE_BORDER_Y_DEF = CROP_HIGH_DEF  / 2;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:

    void SlotTimeout();

    //响应放大鼠标区域图像槽函数
    void SlotMagnifyArea(QPoint);

    //响应使用或停止使用放大镜槽函数  修改m_blMagnifierShow标志量的值
    void on_btn_tobig_clicked();

    //响应播放槽函数
    void on_btn_play_clicked();

    void on_btn_gethandle_clicked();

private:
    Ui::MainWindow *ui;

    CLS_WgtVideoWindows * m_pwgtVideoWindow;

    //播放mp4视频需要的对象
    QMediaPlayer * m_pMediaPlayer;
    QMediaPlaylist * m_pMediaPlaylist;

    //放大预览界面指针
    QLabel * m_pMagnifier;

    //是否使用放大镜标志量  true表示使用 false表示不使用
    bool m_blMagnifierShow;

    QTimer * m_pUpdateTimer;


signals:
    //放大鼠标附近区域图像信号
    void SignMagnifyArea(QPoint);
};
#endif // MAINWINDOW_H
