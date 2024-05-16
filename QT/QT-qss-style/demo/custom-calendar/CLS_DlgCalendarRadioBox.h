#ifndef CLS_DLGCALENDARRADIOBOX_H
#define CLS_DLGCALENDARRADIOBOX_H

#include <QDialog>
#include <QDate>
#include <QDebug>
#include <QToolButton>
#include <QTextCharFormat>
#include <QSettings>
#include <QFile>
namespace Ui {
class CLS_DlgCalendarRadioBox;
}

class CLS_DlgCalendarRadioBox : public QDialog
{
    Q_OBJECT

public:
    explicit CLS_DlgCalendarRadioBox(QString _inifilepath,QWidget *parent = nullptr);
    ~CLS_DlgCalendarRadioBox();

    QString GetDayTime();
    QString GetHourTime();

    //将日历控件恢复到当前的系统时间
    void RestoreDate();

    //初始化日历样式
    void InitSheet();

    //设置所选日期所在月份的所有日期背景颜色
    void SetMonthDaysColor(int _year,int _month, QTextCharFormat _format);

private slots:
    //关闭按钮的槽函数
    void on_btn_close_clicked();

    //确定按钮的槽函数
    void on_btn_ok_clicked();

    //当前选择日期修改的槽函数
    void SlotSelectionChanged();

    //日历当前页被修改的槽函数
    void SlotCurrPageChanged(int _year,int _month);

private:
    Ui::CLS_DlgCalendarRadioBox *ui;
    QString m_qstrDayTime;
    QString m_qstrHourTime;
    int m_iyear;
    int m_imonth;
    //当前日期背景颜色
    QColor m_CurrentDayBackColor;
    //当月所有日期文字颜色QColor
    QColor m_CurrentDaysTextColor;

    QString m_qstrIniFilePath;

signals:
    void SignSelTime(QString _qstrTime);
};

#endif // CLS_DLGCALENDARRADIOBOX_H
