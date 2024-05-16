#ifndef CLS_SHOWRESEDIT_H
#define CLS_SHOWRESEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QStringList>
#include <QDebug>
#include <QRegExpValidator>
#include <QFile>
#include <QEvent>
#include <QMouseEvent>
#include "PublicData.h"

class CLS_ShowResEdit : public QTextEdit
{
    Q_OBJECT

public:
    CLS_ShowResEdit(QWidget *parent = nullptr);

    /******************************************************
    函数 : ParseRes
    功能描述 :  解析传入的执行结果字符串，将字符串按照\n(linux 下的换行符)分割并存储到m_qstrlist
    输入参数 :  QString 执行结果字符串
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void ParseRes(QString);

    /******************************************************
    函数 : OutputRes
    功能描述 :  继续解析m_qstrlist，将字符串按照对应的shell脚本颜色附上颜色并输出到文本编辑框
    输入参数 :
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void OutputRes();

    /******************************************************
    函数 : CalcExecResState
    功能描述 :  根据执行结果执行计算本次脚本执行功能状态
    输入参数 :
    输出参数 : 无
    返回值 : const char *  对应状态的图标路径
    其它 : 无
    *******************************************************/
    const char * CalcExecResState();

private:

    QStringList m_qstrlist;//存储初步解析ssh返回结果的内容
    int  m_slightNum;      //轻微问题警报个数
    int  m_seriousNum;     //严重问题警报个数
    int  m_errNum;         //致命错误报警个数
};

#endif // CLS_SHOWRESEDIT_H
