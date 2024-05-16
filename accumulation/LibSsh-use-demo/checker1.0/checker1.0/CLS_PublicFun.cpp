#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_PublicFun.h"

QString GetShFileName(QString _url)
{
    QStringList tempStringList =  _url.split("/");
    QString filename;
    int count = tempStringList.count();
    if(count > 0)
    {
       filename =  tempStringList[count-1];
    }
    return filename;
}

QString TimeStampToDate(QString _timestamp)
{
    QString date = "";
    if(_timestamp.isEmpty())
    {
        qDebug()<<__FUNCTION__<<" "<<__LINE__<<" 时间戳为空";
        return date;
    }
    QDateTime time = QDateTime::fromMSecsSinceEpoch(_timestamp.toULongLong());
    date = time.toString("yyyy-MM-dd hh:mm:ss.zzz");
    return date;
}

QString PublicGetCurrentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString CalcTimeDiff(QString _startTime, QString _endTime)
{
    //转换成开始时间、结束时间
    QDateTime startDateTime =  QDateTime::fromString(_startTime,"yyyy-MM-dd hh:mm:ss");
    QDateTime endDateTime   =  QDateTime::fromString(_endTime,"yyyy-MM-dd hh:mm:ss");

    //获取时间差
    int secTotal = startDateTime.secsTo(endDateTime);

    //转换时间
    int hour;
    int min;
    int sec;
    QString qstrTakeTime;
    if(secTotal > 0)
    {//时间差是正常的情况
        hour =secTotal/3600;
        min  =secTotal%3600/60;
        sec  =secTotal%60;
        qstrTakeTime = QString("%1时 %2分 %3秒").arg(hour).arg(min).arg(sec);
    }
    return qstrTakeTime;
}

QString TransShellToHtml(QString _qstr)
{
   QString qstrRet = _qstr;

   if(_qstr.isEmpty())
   {//如果为空则直接返回
       return qstrRet;
   }

   //首先获取需要输出的文本内容
   QString qstrForContent = _qstr;
   if(qstrForContent.startsWith("\033["))
   {//如果字符串是以\033[开头的可以解析
       int start = qstrForContent.indexOf('m');

       if(start != -1)
       {
           qstrForContent = qstrForContent.right(qstrForContent.length()-start-1);
           qstrForContent = qstrForContent.trimmed();
       }

       start = qstrForContent.indexOf("\033[0m");
       if(start != -1)
       {
           qstrForContent = qstrForContent.remove(start,4);
       }
   }
   else
   {//如果不是\033[开头，说明没有颜色信息直接返回原字符串内容
       return qstrRet + "<br/>";
   }


   //然后用正则表达式来检索颜色字符
   QString qstrForColor = _qstr;
   QString colStr;
   QStringList list;
   QRegularExpression reg(R"(\[[0-9;]+m)");
   QRegularExpressionMatchIterator it = reg.globalMatch(qstrForColor);

   while (it.hasNext())
   {
       QRegularExpressionMatch match2 = it.next();
       list << match2.captured(0); //获取所有匹配语法的字符串
   }

   if(list.count() > 0)
   {
       QString qstr = list[0]; //只是使用第一个匹配到的字符串
       colStr = qstr.mid(1,qstr.size()-2);

       QStringList qlistcolor = colStr.split(";");

       auto qlistiter = qlistcolor.begin();
       while(qlistiter != qlistcolor.end())
       {
           auto iter = const_SHELLCOLOR.find((*qlistiter).toUInt());
           if(iter != const_SHELLCOLOR.end())
           {
               QString qstrColor =  RGBColorToHex(iter.value());
               //拼接一个html格式的字符串
               qstrRet = QString("<font color=\"%1\"> %2 </font> <br/>").arg(qstrColor).arg(qstrForContent);
           }
           ++qlistiter;
       }
   }
   return qstrRet;
}

QString RGBColorToHex(QColor _color)
{
     //r的rgb转化为16进制
     QString redStr = QString("%1").arg(_color.red(),2,16,QChar('0'));
     //g的rgb转化为16进制
     QString greenStr = QString("%1").arg(_color.green(),2,16,QChar('0'));
     //b的rgb转化为16进制
     QString blueStr = QString("%1").arg(_color.blue(),2,16,QChar('0'));
     //将各rgb的拼接在一起形成#000000
     QString hexStr = "#"+redStr+greenStr+blueStr;
     //返回hexStr
     return hexStr;
}

int CheckIpFormat(QString _ip)
{
    QRegExp regExpIp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if(_ip.isEmpty())
    {
        return CHECK_IP_RES_EMPTY;
    }
    else if (!regExpIp.exactMatch(_ip))
    {
        return CHECK_IP_RES_ERR;
    }
    return CHECK_IP_RES_OK;
}



void RemoveRuleCfgJson()
{
    QFile file(const_CacheFilePath);
    if(file.exists())
    {//删除规则信息文件
        if(!file.remove())
        {
            qDebug()<<"规则信息文件 "<<const_CacheFilePath<<" 删除失败";
        }
    }
    else
    {
        qDebug()<<"规则信息文件 "<<const_CacheFilePath<<" 不存在";
    }
}



void RemoveShellDir()
{
    QDir dir(const_LocalShFilePath);
    if(dir.exists())
    {//递归删除存放脚本文件的目录
        if(!dir.removeRecursively())
        {
            qDebug()<<"存放脚本文件的目录 "<<const_LocalShFilePath<<" 不存在";
        }
    }
    else
    {
        qDebug()<<"存放脚本文件的目录 "<<const_LocalShFilePath<<" 不存在";
    }
}




void RemovePdfDir()
{
    QDir dir(QString("./")+const_PdfDir);
    if(dir.exists())
    {//递归删除存放pdf文件的目录
        if(!dir.removeRecursively())
        {
            qDebug()<<"存放pdf文件的目录 "<<const_PdfDir<<" 不存在";
        }
    }
    else
    {
        qDebug()<<"存放pdf文件的目录 "<<const_PdfDir<<" 不存在";
    }
}
