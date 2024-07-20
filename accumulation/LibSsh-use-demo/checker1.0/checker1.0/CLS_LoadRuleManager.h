/*
 *    本地规则管理类，主要作用
 *    (1)加载或保存本地规则json文件
 *    (2)更新规则时 增删改当前规则(组)信息
 *    (3)向程序其他部分业务提供规则(组)信息
 *    此类是一个单例类
 */


#ifndef CLS_LOADRULEMANAGER_H
#define CLS_LOADRULEMANAGER_H

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <mutex>
#include <QFile>
#include <QMessageBox>
#include "CLS_DownloadManager.h"
#include "PublicData.h"
#include "CLS_PublicFun.h"

class CLS_LoadRuleManager : public QObject
{
    Q_OBJECT

private:
    CLS_LoadRuleManager() = default;
    ~CLS_LoadRuleManager() = default;
    CLS_LoadRuleManager(const CLS_LoadRuleManager &) = delete;
    CLS_LoadRuleManager & operator= (const CLS_LoadRuleManager&) = delete;

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
    static CLS_LoadRuleManager * pInstance;
    static CLS_LoadRuleManager * getInstance();
    static void Release(); //手动回收
    static std::once_flag flag_new;

public:

    /******************************************************
    函数 :  GetBatRule
    功能描述 : 获取当前程序运行时最新的规则组
    输入参数 : 无
    输出参数 : 无
    返回值 : QMap<QString,STRUCT_BATRULE>  QString 规则组id    STRUCT_BATRULE 规则组数据结构
    其它 : 无
    *******************************************************/
    QMap<QString,STRUCT_BATRULE> GetBatRule();

    /******************************************************
    函数 :  GetRule
    功能描述 : 获取当前程序运行时最新的规则
    输入参数 : 无
    输出参数 : 无
    返回值 : QMap<QString,STRUCT_RULE> QString 规则id  STRUCT_RULE 规则数据结构
    其它 : 无
    *******************************************************/
    QMap<QString,STRUCT_RULE> GetRule();


    /******************************************************
    函数 : UpdateRule
    功能描述 :  将外界传入的带有规则和规则组信息和关系的json和程序中缓存的m_mapRule数据做比较
               json与m_mapRule中id相同的规则进行数据覆盖，并且还会下载对应的脚本文件
               json有新id的规则进行数据增加，并且还会下载对应的脚本文件
               json中规则的m_isDel标志为true进行数据删除
               如果传入的QJsonObject参数是来自服务器全量更新的接口此更新则为全量更新，如果来自增量更新的接口则为增量更新
    输入参数 : QJsonObject &  外界传入的带有规则和规则组信息和关系的json
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void UpdateRule(QJsonObject &);

    /******************************************************
    函数 : UpdateBatRule
    功能描述 : 更新规则组数据 与更新规则数据逻辑相同  此函数再更新和增加数据时不需要下载对应的脚本文件
    输入参数 : QJsonObject &
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void UpdateBatRule(QJsonObject &);

    /******************************************************
    函数 : SetUpdateTime
    功能描述 :  设置规则更新时间
    输入参数 :  QString 更新时间
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetUpdateTime(QString);

    /******************************************************
    函数 : GetUpdateTime
    功能描述 :  向外界输出更新时间
    输入参数 :  无
    输出参数 : 无
    返回值 : QString 更新时间
    其它 : 无
    *******************************************************/
    QString GetUpdateTime();

    /******************************************************
    函数 : LoadLoaclRules
    功能描述 : 在程序启动时使用从本地固定路径加载包含规则(组)数据的json文件，
              并解析缓存在内存中
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void LoadLoaclRules();

    /******************************************************
    函数 : SaveRuleToLoacl
    功能描述 : 在更新后将程序中的规则数据保存到本地缓存文件
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SaveRuleToLoacl();

    /******************************************************
    函数 : SetSerIp
    功能描述 : 设置数据服务器对应的ip
    输入参数 : QString ip
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSerIp(QString);

    /******************************************************
    函数 : SetSerPort
    功能描述 : 设置数据服务器对应的端口
    输入参数 : QString 端口
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSerPort(QString);

    /******************************************************
    函数 : ClearProgramCache
    功能描述 : 清空程序缓存数据
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void ClearProgramCache();

private:

    /******************************************************
    函数 : JsonTransToStrRule
    功能描述 : 将一段描述规则和规则组信息和关系的json数据分别转换到STRUCT_BATRULE类型map 和STRUCT_RULE类型规则
              如果有id相同的规则(组)，后续的规则(组)不会插入到map中
    输入参数 : QJsonObject 包含规则和规则组信息和关系的json数据

    输出参数 : QMap<QString,STRUCT_BATRULE> & 保存转换完成后的规则组数据
              QMap<QString,STRUCT_RULE>    & 保存转换完成后的规则数据
    返回值 : 无
    其它 : 此解析只会将没有被废弃的规则组 规则信息保存到m_mapBatRule、m_mapRule 中
    *******************************************************/
    void JsonTransToStrRule(QJsonObject,QMap<QString,STRUCT_BATRULE> & ,QMap<QString,STRUCT_RULE> &);


    /******************************************************
    函数 : StrRuleTransToJson
    功能描述 : 与JsonTransToStrRule 方法逻辑相反
    输入参数 :
              QMap<QString,STRUCT_BATRULE> 即将被转换的规则组数据
              QMap<QString,STRUCT_RULE>    即将被转换的规则数据
    输出参数 : 无
    返回值 : QJsonObject 转换完成后包含规则和规则组信息和关系的json数据
    其它 : 无
    *******************************************************/
    QJsonObject StrRuleTransToJson(QMap<QString,STRUCT_BATRULE> ,QMap<QString,STRUCT_RULE>);

private:

    QString m_updateTime;              //记录从服务器更新的时间，以时间戳形式记录

    CLS_DownloadManager m_downloadMgr; //sh脚本文件下载管理

    QString m_qstrSerIp;               //数据服务器对应的ip

    QString m_qstrSerPort;             //数据服务器对应的端口


    //规则组数据集合  规则组id对应规则组结构信息
    QMap<QString,STRUCT_BATRULE> m_mapBatRule;

    //规则数据集合    规则id对应规则结构信息
    QMap<QString,STRUCT_RULE> m_mapRule;
};

#endif // CLS_LOADRULEMANAGER_H
