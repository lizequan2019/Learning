#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextEdit>
#include <QPoint>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE




class myButton : public QPushButton
{
    protected:
        bool event(QEvent* e);
        void mousePressEvent(QMouseEvent* e);
        void keyPressEvent(QKeyEvent * e);
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool event(QEvent* e);               //事件处理方式 1：重写虚函数 QObject::event()
    void mousePressEvent(QMouseEvent* e);//事件处理方式 2：重写 QMainWindow 类中的此虚函数
    void keyPressEvent(QKeyEvent * e);
    bool eventFilter(QObject * w, QEvent * e);
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    myButton * mybutton;
    QTextEdit * mTextEdit;
};
#endif // MAINWINDOW_H
