#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fun_1();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fun_1()
{
    //QListView 测试
    {
        QStringListModel *model_listview = new QStringListModel;
        model_listview->setStringList(QStringList()<<"123"<<"456"<<"789");
        ui->listView->setModel(model_listview);

        QModelIndex index = ui->listView->model()->index(0,0).parent();
        qDebug()<<"listview index(0,0) is valid?"<<index.isValid();

        index = ui->listView->model()->index(1,0).parent();
        qDebug()<<"listview index(1,0) is valid?"<<index.isValid();
    }


    //QTreeView 测试
    {
        QStandardItemModel * model_treeview = new QStandardItemModel;
        QList<QStandardItem*> row;

        row.clear();
        QStandardItem *item0_0 = new QStandardItem("lzq1");
        item0_0->appendRow(new QStandardItem("lzq1_1"));
        QStandardItem *item0_1 = new QStandardItem("18");
        row << item0_0 << item0_1;
        model_treeview->appendRow(row);

        row.clear();
        QStandardItem *item1_0 = new QStandardItem("lzq2");
        item1_0->appendRow(new QStandardItem("lzq2_1"));
        QStandardItem *item1_1 = new QStandardItem("33");
        row << item1_0 << item1_1;
        model_treeview->appendRow(row);

        ui->treeView->setModel(model_treeview);
        /*  treeview 输出
         *   1              2
         *   lzq            18
         *      lzq1_1
         *
         *   lzq2           33
         */

        //这里判断的是lzq 数据对应的父索引
        QModelIndex index = ui->treeView->model()->index(0,0).parent();
        qDebug()<<"treeview index(0,0) is valid?"<<index.isValid();

        //这里判断的是lzq1_1数据对应的父索引
        index = ui->treeView->model()->index(0,0,item0_0->index()).parent();
        qDebug()<<"treeview index(0,0,item0_0->index()) is valid?"<<index.isValid();
    }


    //QTableView 测试
    {
        QStandardItemModel * model_tableview = new QStandardItemModel;
        QList<QStandardItem*> row;

        row.clear();
        QStandardItem *item0_0 = new QStandardItem("lzq");
        QStandardItem *item0_1 = new QStandardItem("18");
        QStandardItem *item0_2 = new QStandardItem("man");
        row << item0_0 << item0_1 << item0_2;
        model_tableview->appendRow(row);

        row.clear();
        QStandardItem *item1_0 = new QStandardItem("lzq2");
        QStandardItem *item1_1 = new QStandardItem("188");
        QStandardItem *item1_2 = new QStandardItem("woman");
        row << item1_0 << item1_1 << item1_2;
        model_tableview->appendRow(row);

        ui->tableView->setModel(model_tableview);

        QModelIndex index = ui->tableView->model()->index(0,2).parent();
        qDebug()<<"tableView index(0,2) is valid?"<<index.isValid();

        index = ui->tableView->model()->index(0,0).parent();
        qDebug()<<"tableView index(0,0) is valid?"<<index.isValid();

        index = ui->tableView->model()->index(1,0).parent();
        qDebug()<<"tableView index(1,0) is valid?"<<index.isValid();

        index = ui->tableView->model()->index(1,1).parent();
        qDebug()<<"tableView index(1,2) is valid?"<<index.isValid();
    }

}

