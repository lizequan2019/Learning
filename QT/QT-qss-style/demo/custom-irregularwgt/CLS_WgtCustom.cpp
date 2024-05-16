#include "CLS_WgtCustom.h"

CLS_WgtIrregularCombination::CLS_WgtIrregularCombination(QWidget *parent) : QWidget(parent)
{
    this->resize(300,300);
    this->setObjectName("combination");

    QPixmap pix;
    pix.load(":/pic/PIC/Dashboard.svg");
    this->setFixedSize(pix.size());
    this->setMask(pix.mask());

    m_pbtn = new QPushButton(this);
    m_pbtn->setObjectName("combination_btn");
    m_pbtn->setText("button");
    m_pbtn->move(80,80);
}


void CLS_WgtIrregularCombination::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}











//====================================

CLS_WgtIrregularSingle::CLS_WgtIrregularSingle(QWidget *parent) : QWidget(parent)
{
    this->setObjectName("single");
    QPixmap pix;
    pix.load(":/pic/PIC/china.svg");
    this->setFixedSize(pix.size());
    this->setMask(pix.mask());
}

void CLS_WgtIrregularSingle::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}
