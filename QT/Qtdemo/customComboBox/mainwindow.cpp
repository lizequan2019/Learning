#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pCombox = new MultiSelectComboBox(this);
    connect(m_pCombox,SIGNAL(SigSelChange(SELECT_TYPE,QString)),this,SLOT(SlotSelChange(SELECT_TYPE,QString)));

    m_pCombox->setGeometry(100,200,200,30);

    QVariant var;
    STRUCT_CK_USER_DATA userdata;


    userdata.m_userdata = "我是规则 1";  //不要使用QStringLiteral进行汉字转换
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule1"),var);

    userdata.m_userdata = QStringLiteral("i am rule 2");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule2"),var);

    userdata.m_userdata = QStringLiteral("i am rule 3");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule3"),var);

    userdata.m_userdata = QStringLiteral("i am rule 4");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule4"),var);

    userdata.m_userdata = QStringLiteral("i am rule 5");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule5"),var);

    userdata.m_userdata = QStringLiteral("i am rule 6");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule6"),var);

    userdata.m_userdata = QStringLiteral("i am rule 7");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule7"),var);

    userdata.m_userdata = QStringLiteral("i am rule 8");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule8"),var);

    userdata.m_userdata = QStringLiteral("i am rule 9");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule9"),var);

    userdata.m_userdata = QStringLiteral("i am rule 10");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule10"),var);

    userdata.m_userdata = QStringLiteral("i am rule 11");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule11"),var);

    userdata.m_userdata = QStringLiteral("i am rule 12");
    var.setValue(userdata);
    m_pCombox->addItem(QStringLiteral("rule12"),var);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(m_pCombox)
    {
        m_pCombox->clearCheckBox();
    }
}

void MainWindow::SlotSelChange(SELECT_TYPE _seltype, const QString &_qstr)
{
    QString msg;
    if(_seltype == SELECT_TYPE::SELECT_TYPE_SEL)
    {
        msg = QStringLiteral("sel state    ");
    }
    else if(_seltype == SELECT_TYPE::SELECT_TYPE_REMOVESEL)
    {
        msg = QStringLiteral("rm sel state  ");
    }

    ui->textEdit->append(msg + _qstr);
}

void MainWindow::on_pushButton_2_clicked()
{
     m_pCombox->addItem("增加按钮");
}
