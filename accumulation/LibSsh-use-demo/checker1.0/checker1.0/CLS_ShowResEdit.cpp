#include "CLS_ShowResEdit.h"


CLS_ShowResEdit::CLS_ShowResEdit(QWidget *parent)
     :QTextEdit(parent)
     ,m_slightNum(0)
     ,m_seriousNum(0)
     ,m_errNum(0)
{
    this->setReadOnly(true);
    this->setObjectName("showresedit");
    this->setContextMenuPolicy(Qt::NoContextMenu);
}


void CLS_ShowResEdit::ParseRes(QString _res)
{
    m_qstrlist.clear();

    //解析执行结果，按照\n符分割
    QStringList qstrlist = _res.split("\n");
    for(int i = 0; i < qstrlist.size(); ++i)
    {
        QString qstrsingle = qstrlist[i];
        qstrsingle = qstrsingle.trimmed();

        if(qstrsingle.isEmpty() //||
//           qstrsingle == const_SSH_EXEC_SUCCESS ||
//           qstrsingle == const_SSH_EXEC_FAIL
           )
        {//列举出不应该输出的字符串
            continue;
        }

        m_qstrlist.push_back(qstrlist[i]);
    }
}

void CLS_ShowResEdit::OutputRes()
{
    if(m_qstrlist.isEmpty())
    {
        return;
    }

    for(int i = 0; i < m_qstrlist.size(); ++i)
    {

        //首先获取需要输出的文本内容
        QString qstrForContent = m_qstrlist[i];
        if(qstrForContent.startsWith("\033["))
        {
            int start = qstrForContent.indexOf('m');

            if(start != -1){
                qstrForContent = qstrForContent.right(qstrForContent.length()-start-1);
                qstrForContent = qstrForContent.trimmed();
            }

            start = qstrForContent.indexOf("\033[0m");
            if(start != -1){
                qstrForContent = qstrForContent.remove(start,4);
            }
        }


        //然后用正则表达式来检索颜色字符
        QString qstrForColor = m_qstrlist[i];
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
                    QColor qcolor = iter.value();

                    if(qcolor == const_yellow)
                    {
                        m_slightNum++;
                    }
                    else if(qcolor == const_purple)
                    {
                        m_seriousNum++;
                    }
                    else if(qcolor == const_red)
                    {
                        m_errNum++;
                    }

                    this->setTextColor(qcolor);
                }
                ++qlistiter;
            }
        }

        // 写入一行内容
        this->append(qstrForContent);

        // 最后恢复原来的颜色
        this->setTextColor(QColor(0,0,0));
    }
}


const char * CLS_ShowResEdit::CalcExecResState()
{
    if(m_errNum != 0)
    {
        return  const_ErrStatePicPath;
    }

    if(m_seriousNum != 0)
    {
        return  const_SeriousStatePicPath;
    }

    if(m_slightNum != 0)
    {
        return  const_SlightStatePicPath;
    }

    m_slightNum = 0;
    m_seriousNum = 0;
    m_errNum = 0;

    return const_OkStatePicPath;
}
