#ifndef CLS_WGTCUSTOM_H
#define CLS_WGTCUSTOM_H

#include <QObject>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QBitmap>
#include <QPushButton>

//自定义不规则单独界面
class CLS_WgtIrregularSingle: public QWidget
{
    Q_OBJECT
public:
    explicit CLS_WgtIrregularSingle(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e);
};




//自定义不规则组合界面
class CLS_WgtIrregularCombination : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *e);
public:
    explicit CLS_WgtIrregularCombination(QWidget *parent = nullptr);
private:
    QPushButton * m_pbtn;
};

#endif // CLS_WGTCUSTOM_H
