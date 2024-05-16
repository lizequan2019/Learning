
/*
 *  这个demo验证了创建继承QWidget的自定义控件类本身(控件类中的其他控件可以被设置)不能被设置样式的问题
 *  参考 https://blog.csdn.net/qq_37354286/article/details/79845547 有两种方法
 *
 *
    void MyWidget::paintEvent(QPaintEvent *event) //第二种方法 我觉得这个不太好
    {
        Q_UNUSED(event);
        QStyleOption styleOpt;
        styleOpt.init(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
    }

 */

#ifndef CLS_MYWGT_H
#define CLS_MYWGT_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QFile>

class CLS_MyWgt : public QWidget
{
    Q_OBJECT
public:
    explicit CLS_MyWgt(QWidget *parent = nullptr);

signals:

};

#endif // CLS_MYWGT_H
