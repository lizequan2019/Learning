#pragma execution_character_set("utf-8")

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_close_clicked();

private:
    Ui::MainWindow *ui;
    bool blMousePress;
    int m_icorrectX;
    int m_icorrectY;
    QPushButton * pButton;
};
#endif // MAINWINDOW_H
