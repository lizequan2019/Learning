#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QQueue>
#include <QJsonObject>
#include <QThread>


#if _MSC_VER >= 1600 //vs2010及其以上版本
    #pragma execution_character_set("utf-8")  //这句是关键
#endif

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
    void on_btn_create_treeitem_clicked();

    void on_btn_delete_treeitem_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
