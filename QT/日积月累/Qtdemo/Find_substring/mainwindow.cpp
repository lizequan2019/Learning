#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextDocument>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    indexNo = 0;
    TotalNum= 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::find()
{
    qlstExtrSelections.clear();
    QString findtext = ui->lineEdit->text();  //获取要搜索的文本
    QTextDocument *document = ui->textEdit->document(); //获取document的指针
    QTextCursor qCursorHighlight = ui->textEdit->textCursor(); //获取文本框的光标的指针

    qCursorHighlight.setPosition(0); //因为是从前向后搜索，所以光标设置为在文本的最前面

    while (qCursorHighlight.isNull() == false)
    {
       //查找指定的文本，匹配整个单词
        qCursorHighlight = document->find(findtext,qCursorHighlight);
        if (qCursorHighlight.isNull() == false)
        {
            QTextEdit::ExtraSelection qExtrSel;
            qExtrSel.format.setForeground(Qt::red);
            qExtrSel.format.setBackground(Qt::green);
            qExtrSel.cursor = qCursorHighlight;
            qlstExtrSelections.append(qExtrSel);
        }
    }
    ui->textEdit->setExtraSelections(qlstExtrSelections);
}

void MainWindow::location(FIND_POS _findpos)
{
    find();
    TotalNum = qlstExtrSelections.size();
    if(TotalNum == 0 || ui->lineEdit->text().isEmpty())
    {
        indexNo = 0;
        return;
    }
    else
    {
       if(_findpos == FIND_FIRST)
       {
           indexNo = 1;
       }
       else if(_findpos == FIND_PRE)
       {
           indexNo--;
           if(indexNo < 1)
           {
               indexNo = TotalNum;
           }
       }
       else if(_findpos == FIND_NEXT)
       {
           indexNo++;
           if(indexNo > TotalNum)
           {
               indexNo = 1;
           }
       }
        qlstExtrSelections[indexNo - 1].format.setBackground(Qt::blue);
        ui->textEdit->setExtraSelections(qlstExtrSelections);
    }
    ui->textEdit->setTextCursor(qlstExtrSelections[indexNo - 1].cursor);
    ui->textEdit->moveCursor(QTextCursor::Right,QTextCursor::MoveAnchor);
    ui->label->setText(QString("第%1处 共%2处").arg(indexNo).arg(TotalNum));
}


void MainWindow::on_pushButton_clicked()
{
   location(FIND_FIRST);
}

void MainWindow::on_btn_before_clicked()
{
   location(FIND_PRE);
}

void MainWindow::on_btn_after_clicked()
{
   location(FIND_NEXT);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        indexNo = TotalNum = 0;
        ui->label->setText("");
    }
}


void MainWindow::on_btn_movetop_clicked()
{
    QScrollBar *scrollbar = ui->textEdit->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->minimum());
}

void MainWindow::on_btn_central_clicked()
{
    QScrollBar *scrollbar = ui->textEdit->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->maximum()/2);
}

void MainWindow::on_btn_bottom_clicked()
{
    QScrollBar *scrollbar = ui->textEdit->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->maximum());
}
