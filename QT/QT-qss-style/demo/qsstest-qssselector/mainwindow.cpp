#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->hide();
    ui->menubar->hide();

    //类型选择器
    m_map[ui->tab_typeselector] = "typeselector.qss";

    //属性选择器
    m_map[ui->tab_attriselector] = "attrselector.qss";

    //非子类选择器
    m_map[ui->tab_noinherselector] = "noinherselector.qss";

    //直接后代选择器
    m_map[ui->tab_directselector] = "directselector.qss";

    //后代选择器
    m_map[ui->tab_laterselector] = "laterselector.qss";

    //后代选择器后面跟名称选择器
    m_map[ui->tab_laterselectorwithid] = "laterselectorwithid.qss";


    //向类型选择器界面 中加入自定义的控件
    pMyFrameTypeSelect = new CLS_MyFrame((QFrame *)ui->tab_typeselector);
    pMyLabelTypeSelect = new CLS_MyLabel((QLabel *)ui->tab_typeselector);
    pMyFrameTypeSelect->setGeometry(300,300,50,50);
    pMyLabelTypeSelect->setText("自定义控件");
    pMyLabelTypeSelect->setGeometry(370,300,100,50);

    //向属性选择器 中的控件 设置属性
    ui->btn_attrselector_red->setProperty("color","red");
    ui->btn_attrselector_green->setProperty("color","green");
    ui->btn_attrselector_yellow->setProperty("color","yellow");
    ui->btn_attrselector_blue->setProperty("color","blue");


    //向后代选择器(跟随id选择器) 中增加空间

    //在第一层wgt动态增加一个按钮
    QPushButton * wgt_one = new QPushButton(ui->wgt_laterwithid_one);
    wgt_one->setGeometry(270,29,150,28);
    wgt_one->setText("不设置setObjectName");

    //在第二层wgt动态增加一个按钮
    QPushButton * wgt_two = new QPushButton(ui->wgt_laterwithid_two);
    wgt_two->setGeometry(270,20,150,28);
    wgt_two->setText("不设置setObjectName");

    //在第三层wgt动态增加一个按钮
    QPushButton * wgt_three = new QPushButton(ui->wgt_laterwithid_three);
    wgt_three->setGeometry(270,28,150,28);
    wgt_three->setObjectName("wgt_three");
    wgt_three->setText("设置setObjectName");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetQss(QWidget *_ptab, QString _filename, bool _flag)
{
    if(_ptab == nullptr)
    {
        QMessageBox::critical(this,"错误","控件不存在");
        return;
    }

    QString qstrContent;
    if(_flag)
    {
        QFile fileTabQss;
        fileTabQss.setFileName("./" + _filename); //类型选择器样式表
        if(!fileTabQss.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this,"错误","打开文件" + _filename + "失败");
            return;
        }

        qstrContent = fileTabQss.readAll();
    }

    qDebug()<<" qstrContent = "<<qstrContent;
    _ptab->setStyleSheet(qstrContent);
}


void MainWindow::on_btn_loadqss_clicked()
{
    QWidget * ptab = ui->tab_selector->currentWidget();
    QString qstrfileName = m_map[ptab];
    qDebug()<<"load qss file = "<<qstrfileName;
    SetQss(ptab,qstrfileName);
}


void MainWindow::on_btn_unlaodqss_clicked()
{
    QWidget * ptab = ui->tab_selector->currentWidget();
    QString qstrfileName = m_map[ptab];
    qDebug()<<"unload qss file = "<<qstrfileName;
    SetQss(ptab,qstrfileName,false);
}

