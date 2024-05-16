#include "CLS_DlgCalendarRadioBox.h"
#include "ui_CLS_DlgCalendarRadioBox.h"

CLS_DlgCalendarRadioBox::CLS_DlgCalendarRadioBox(QString _inifilepath,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLS_DlgCalendarRadioBox),
    m_qstrIniFilePath(_inifilepath)
{
    ui->setupUi(this);

    InitSheet();
    connect(ui->DateCalendar,SIGNAL(selectionChanged()),this,SLOT(SlotSelectionChanged()));
    connect(ui->DateCalendar,SIGNAL(currentPageChanged(int,int)),this,SLOT(SlotCurrPageChanged(int,int)));
}


CLS_DlgCalendarRadioBox::~CLS_DlgCalendarRadioBox()
{
    delete ui;
}


QString CLS_DlgCalendarRadioBox::GetDayTime()
{
    return m_qstrDayTime;
}


QString CLS_DlgCalendarRadioBox::GetHourTime()
{
    return m_qstrHourTime;
}


void CLS_DlgCalendarRadioBox::RestoreDate()
{
    QDate tdate=QDate::currentDate();
    ui->DateCalendar->setSelectedDate(tdate);
}


void CLS_DlgCalendarRadioBox::InitSheet()
{
    //控件的属性设置
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::FramelessWindowHint;         //设置无边框
    setWindowFlags(flags);
    setAttribute(Qt::WA_StyledBackground);    //设置可以使用qss样式表来修饰QDialog控件
    this->setObjectName("dlgcalendarradiobox");


    //为了显示  星期标题为  一 二 三 四 五 六 日
    ui->DateCalendar->setLocale(QLocale::Chinese);


    //这里将周六和周日对应的日期文字颜色设置的和周一相同，这样周一到周日对应的日期颜色就都是相同的了
    QTextCharFormat tcf = ui->DateCalendar->weekdayTextFormat(Qt::Monday);
    ui->DateCalendar->setWeekdayTextFormat(Qt::Sunday, tcf);
    ui->DateCalendar->setWeekdayTextFormat(Qt::Saturday, tcf);

    //读取ini配置文件获取颜色配置
    if (QFile().exists(m_qstrIniFilePath))
    {
        QSettings setting(m_qstrIniFilePath, QSettings::IniFormat);
        setting.beginGroup("Color");
        m_CurrentDayBackColor = QColor(setting.value("CurrentDayBackColor").toString().toUInt(nullptr,16));
        m_CurrentDaysTextColor = QColor(setting.value("CurrentDaysTextColor").toString().toUInt(nullptr,16));
        setting.endGroup();
    }


    //设置当前月份的日期的文字颜色
    QDate tdate=QDate::currentDate();
    m_iyear = tdate.year();
    m_imonth= tdate.month();
    QTextCharFormat selectFormat;
    selectFormat.setForeground(m_CurrentDaysTextColor);
    SetMonthDaysColor(m_iyear,m_imonth,selectFormat);
}


void CLS_DlgCalendarRadioBox::SetMonthDaysColor(int _year, int _month, QTextCharFormat _format)
{
    QDate date = QDate(_year,_month,1);  //这里获取上次所选年月中的1号日期，为的是获取此月份的总天数
    for (int i = 0; i < date.daysInMonth(); ++i)
    {
        QDate date = QDate(m_iyear, m_imonth, i + 1);
        ui->DateCalendar->setDateTextFormat(date,_format);
    }
}





void CLS_DlgCalendarRadioBox::on_btn_ok_clicked()
{
    m_qstrDayTime = ui->DateCalendar->selectedDate().toString("yyyy-MM-dd");
    m_qstrHourTime = ui->TimeEdit->dateTime().toString("hh:mm:ss");
    RestoreDate();
    accept();
}



void CLS_DlgCalendarRadioBox::on_btn_close_clicked()
{
    RestoreDate();
    reject();
}



void CLS_DlgCalendarRadioBox::SlotSelectionChanged()
{
    //恢复上次选中的月份所有日期的文字颜色,此处 QTextCharFormat() 表示不设置样式
    SetMonthDaysColor(m_iyear, m_imonth, QTextCharFormat());


    //修改当前月所有日期的文字颜色，用以区别其他月份日期
    QDate tdate = ui->DateCalendar->selectedDate();    //获取当前日历被选择的日期
    m_iyear = tdate.year();  //重新赋值所选日期的年份
    m_imonth= tdate.month(); //重新赋值所选日期的月份
    QTextCharFormat selectFormat;
    selectFormat.setForeground(m_CurrentDaysTextColor);
    SetMonthDaysColor(m_iyear, m_imonth, selectFormat);

    //修改系统当天日期的背景颜色
    tdate = QDate::currentDate();
    QTextCharFormat todayFormat;
    todayFormat.setBackground(m_CurrentDayBackColor);
    ui->DateCalendar->setDateTextFormat(tdate,todayFormat);
}


void CLS_DlgCalendarRadioBox::SlotCurrPageChanged(int _year, int _month)
{
    //恢复上次选中的月份所有日期的文字颜色,此处 QTextCharFormat() 表示不设置样式
    SetMonthDaysColor(m_iyear, m_imonth, QTextCharFormat());

    //修改当前月所有日期的文字颜色，用以区别其他月份日期
    m_iyear = _year;  //重新赋值所选日期的年份
    m_imonth= _month; //重新赋值所选日期的月份
    QTextCharFormat selectFormat;
    selectFormat.setForeground(m_CurrentDaysTextColor);
    SetMonthDaysColor(m_iyear, m_imonth, selectFormat);

    //修改系统当天日期的背景颜色
    QDate tdate = QDate::currentDate();
    QTextCharFormat todayFormat;
    todayFormat.setBackground(m_CurrentDayBackColor);
    ui->DateCalendar->setDateTextFormat(tdate,todayFormat);
}



