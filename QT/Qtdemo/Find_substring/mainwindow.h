#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include <QTextBlock>
#include <QTextEdit>

enum FIND_POS{
    FIND_FIRST,  //查找第一个
    FIND_PRE,    //查找上一个
    FIND_NEXT    //查找下一个
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void find();
    void location(FIND_POS _findpos);

private slots:
    void on_pushButton_clicked();

    void on_btn_before_clicked();

    void on_btn_after_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_btn_movetop_clicked();

    void on_btn_central_clicked();

    void on_btn_bottom_clicked();

private:
    Ui::MainWindow *ui;
    int indexNo;
    int TotalNum;
    QList<QTextEdit::ExtraSelection> qlstExtrSelections; //存储被搜索的文本的位置以及显示格式
};
#endif // MAINWINDOW_H
