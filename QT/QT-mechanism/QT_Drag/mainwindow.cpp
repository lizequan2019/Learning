#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextDocumentFragment>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pedtA = new myTextEdit;
    m_pedtB = new myTextEdit;

    m_pedtA->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    m_pedtA->setParent(this);

    m_pedtB->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    m_pedtB->setParent(this);

    QTextDocumentFragment fragment;
    //fragment = QTextDocumentFragment::fromHtml("<img src='./1.ico'>");
    //m_pedtA->textCursor().insertFragment(fragment);
    m_pedtA->setVisible(true);


    m_pedtB->setPlainText("this is B");

    m_pedtA->setGeometry(0,0,200,200);
    m_pedtB->setGeometry(220,0,200,200);

    //A不支持放置  B支持放置
    m_pedtA->setAcceptDrops(false);
    m_pedtB->setAcceptDrops(true);

    m_pedtA->show();
    m_pedtB->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}


