#include "CLS_MyWgt.h"

CLS_MyWgt::CLS_MyWgt(QWidget *parent)
    : QWidget{parent}
{
    this->setAttribute(Qt::WA_StyledBackground, true); //第一种方法

    QPushButton * btn1 = new QPushButton(this);
    btn1->setText("btn1");
    btn1->setObjectName("btn1");

    QPushButton * btn2 = new QPushButton(this);
    btn2->setText("btn2");
    btn2->setObjectName("btn2");

    QPushButton * btn3 = new QPushButton(this);
    btn3->setText("btn3");
    btn3->setObjectName("btn3");

    QWidget * wgt = new QWidget(this);
    wgt->setObjectName("wgt");

    QHBoxLayout * phlayout = new QHBoxLayout;
    phlayout->setContentsMargins(0,0,0,0);
    phlayout->setSpacing(20);
    phlayout->addStretch();
    phlayout->addWidget(btn1);
    phlayout->addWidget(btn2);
    phlayout->addWidget(btn3);

    QVBoxLayout * pvlayout = new QVBoxLayout;
    pvlayout->setContentsMargins(0,0,0,0);
    pvlayout->setSpacing(0);
    pvlayout->addWidget(wgt);
    pvlayout->addLayout(phlayout);
    pvlayout->addStretch();

    this->setLayout(pvlayout);

    QFile file("./skin.qss");
    file.open(QIODevice::ReadOnly);
    this->setStyleSheet(file.readAll());
    file.close();

}
