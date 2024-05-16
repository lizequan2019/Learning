#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <QDebug>
#include <CLS_MyFrame.h>
#include <CLS_MyLabel.h>
#include <QLayout>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //_flag = true 表示加载模式    _flag = false 表示卸载模式
    void SetQss(QWidget * _ptab, QString _filename, bool _flag = true);


private slots:

    void on_btn_loadqss_clicked();

    void on_btn_unlaodqss_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QWidget*,QString> m_map;
    CLS_MyFrame * pMyFrameTypeSelect;
    CLS_MyLabel * pMyLabelTypeSelect;
};
#endif // MAINWINDOW_H
