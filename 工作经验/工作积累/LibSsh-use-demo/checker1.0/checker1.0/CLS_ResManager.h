/*
 *   ssh脚本执行结果管理类
 *   此类是一个单例类，类中包含了导出pdf所需要的所有信息，如果pdf修改信息，此类成员变量也应该做出相应的修改
 *   此类只保存一次执行(支持多个服务器同时执行)的结果，下一次的执行结果会覆盖掉当前的内容
 *
 */

#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif


#ifndef CLS_RESMANAGER_H
#define CLS_RESMANAGER_H

#include <QObject>
#include <mutex>
#include "PublicData.h"
#include <QMap>
#include <QVector>
#include <QString>
#include "CLS_LoadRuleManager.h"
class CLS_ResManager : public QObject
{
    Q_OBJECT

private:
    CLS_ResManager() = default;
    ~CLS_ResManager() = default;
    CLS_ResManager(const CLS_ResManager &) = delete;
    CLS_ResManager & operator= (const CLS_ResManager&) = delete;

    class gc  //资源回收类
    {
        ~gc()
        {
            if(pInstance)
            {
                delete pInstance;
                pInstance = nullptr;
            }
        }
    };


public:

    //作为单例模式类需要的一些元素 不涉及到业务
    static std::mutex m_mutex;
    static CLS_ResManager * pInstance;
    static CLS_ResManager * getInstance();
    static void Release(); //手动回收
    static std::once_flag flag_new;

    //Ssh项Id 与 导出pdf基础信息的映射关系
    QMap<int,STRUCT_RES> m_mapSshBasicMsg;

    //Ssh项Id 与 其检测的规则组id集合的映射关系
    QMap<int, QVector<QString>> m_mapSshBatRule;

    //检测的(规则组id+规则id 这是为了准确定位)和规则检测结果的映射关系
    QMap<QString, QString> m_mapRuleRes;

    //以日志的形式输出类中所有成员变量的值，用于自检使用
    void OutputMsg();

    //清空类中所有内容
    void Clear();


    //下面的函数都是为了方便使用而创建的

    /******************************************************
    函数 :      GetSshIdFromSerIp
    功能描述 :  通过服务器IP返回对应的ssh项id
    输入参数 :  QString 服务器ip
    输出参数 :  无
    返回值 :    int对应的ssh项id   -1表示没有检测到  其他正整数表示ssh项id号
    其它 : 无
    *******************************************************/
    int GetSshIdFromSerIp(QString);

    /******************************************************
    函数 :      GetBatRuleNameFromSshId
    功能描述 :  通过ssh项id获取对应的所有规则组名称
    输入参数 :  int对应的ssh项id
    输出参数 :  无
    返回值 :    QString对应的所有规则组名称  返回空表示没有检测到
    其它 :
    *******************************************************/
    QStringList GetBatRuleNameFromSshId(int);

};

#endif // CLS_RESMANAGER_H
