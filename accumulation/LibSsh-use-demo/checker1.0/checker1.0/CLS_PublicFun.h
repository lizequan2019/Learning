#ifndef CLS_PUBLICFUN_H
#define CLS_PUBLICFUN_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include "PublicData.h"
#include <QRegularExpression>
#include <QColor>
#include <QDir>
#include <QFile>

/******************************************************
 函数 :  GetShFileName
 功能描述 : 从url路径中截取中正确的sh脚本名称  url路径格式 http://xxxx/xxx/xxx.sh
 输入参数 : QString  url路径
 输出参数 : 无
 返回值 : QString sh脚本名称
 其它 : 无
*******************************************************/
QString GetShFileName(QString _url);


/******************************************************
 函数 :  TimeStampToDate
 功能描述 : 将时间戳(毫秒级)转换为字符串类型的时间格式(yyyy-MM-dd hh:mm:ss.zzz)
 输入参数 : QString 时间戳字符串
 输出参数 : 无
 返回值 : QString 字符串类型的时间格式
 其它 : 无
*******************************************************/
QString TimeStampToDate(QString _timestamp);

/******************************************************
 函数 :  PublicGetCurrentTime
 功能描述 : 获取当前时间并转换成字符串类型的时间格式(yyyy-MM-dd hh:mm:ss.zzz)
 输入参数 : 无
 输出参数 : 无
 返回值 : QString 字符串类型的时间格式
 其它 : 无
*******************************************************/
QString PublicGetCurrentTime();

/******************************************************
 函数 :  CalcTimeDiff
 功能描述 : 计算时间差返回时分秒字符串
 输入参数 : _startTime 开始时间    _endTime结束时间    时间格式都是yyyy-MM-dd hh:mm:ss
 输出参数 : 无
 返回值 : QString 时分秒字符串
 其它 : 无
*******************************************************/
QString CalcTimeDiff(QString _startTime, QString _endTime);


/******************************************************
 函数 :  RGBColorToHex
 功能描述 : 将QColor 对象转换成16进制的字符串"#xxxxxx"
 输入参数 : QColor 将被转换的对象
 输出参数 : 无
 返回值 : QString 16进制字符串
 其它 : 无
*******************************************************/
QString RGBColorToHex(QColor);


/******************************************************
 函数 :     TransShellToHtml
 功能描述 : 将一段ssh返回的字符串转换成html格式字符串
 输入参数 : QString 需要转换的字符串
 输出参数 : 无
 返回值 :   QString 转换完成后的字符串
 其它 :    目前修改的内容如下
          (1) 此函数会再字符串后加上 <br/>
          (2) shell脚本中的颜色信息会转换成16进制颜色值
*******************************************************/
QString TransShellToHtml(QString);


/******************************************************
 函数 :    CheckIpFormat
 功能描述 : 用于校验ip格式是否正确
 输入参数 : QString 需要校验的ip
 输出参数 : 无
 返回值   :  int  枚举类型CHECK_IP_RES  0表示格式正确  -1表示ip为空  -2表示ip格式不正确
 其它     :
*******************************************************/
int CheckIpFormat(QString);


/******************************************************
 函数 :    RemoveRuleCfgJson
 功能描述 : 删除规则信息配置文件
 输入参数 :
 输出参数 :
 返回值  :
 其它    :
*******************************************************/
void RemoveRuleCfgJson();


/******************************************************
 函数 :    RemovePdfDir
 功能描述 : 删除存储pdf文件的目录
 输入参数 :
 输出参数 :
 返回值  :
 其它    :
*******************************************************/
void RemovePdfDir();

/******************************************************
 函数 :    RemoveShellDir
 功能描述 : 删除存储shell脚本的目录
 输入参数 :
 输出参数 :
 返回值  :
 其它    :
*******************************************************/
void RemoveShellDir();


#endif // CLS_PUBLICFUN_H
