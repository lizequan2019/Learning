#include "CLS_WgtVideoWindows.h"
#include "ui_CLS_WgtVideoWindows.h"

#pragma execution_character_set("utf-8")

CLS_WgtVideoWindows::CLS_WgtVideoWindows(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLS_WgtVideoWindows)
{
    ui->setupUi(this);
    ui->w3->setToolTip("这是另一个组合控件的QWidget");
    ui->w4->setToolTip("这是另一个组合控件的QWidget");

    ui->w3->setStyleSheet("border-image:url(\"./pic.png\")");
}

CLS_WgtVideoWindows::~CLS_WgtVideoWindows()
{
    delete ui;
}
