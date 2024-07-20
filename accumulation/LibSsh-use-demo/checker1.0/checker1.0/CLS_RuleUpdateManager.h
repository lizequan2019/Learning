/*
 *  此类用于向数据库服务器更新规则(组)数据
 */
#ifndef CLS_RULEUPDATEMANAGER_H
#define CLS_RULEUPDATEMANAGER_H

#include <QFile>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <mutex>
#include <QMessageBox>
#include "PublicData.h"
#include "CLS_LoadRuleManager.h"
#include "CLS_WebServiceManager.h"


class CLS_RuleUpdateManager : public QObject , public QRunnable
{
    Q_OBJECT
private:
    CLS_RuleUpdateManager() = default;
    ~CLS_RuleUpdateManager() = default;
    CLS_RuleUpdateManager(const CLS_RuleUpdateManager &) = delete;
    CLS_RuleUpdateManager & operator= (const CLS_RuleUpdateManager&) = delete;

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

    static std::mutex m_mutex;
    static CLS_RuleUpdateManager * pInstance;
    static CLS_RuleUpdateManager * getInstance();
    static void Release(); //手动回收
    static std::once_flag flag_new;

    void run() override;

    /******************************************************
    函数 : GetToken
    功能描述 : 通过webservice接口获取数据服务器交互的凭证
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void GetToken();

    /******************************************************
    函数 : UpdateRule
    功能描述 : 通过webservice接口从服务器上更新规则(组)
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 更新分为全量更新和增量更新 访问的webservice接口是一样的，只不过全量更新传递的updateTime值为空
          内部逻辑会判断使用全量更新还是增量更新
    *******************************************************/
    void UpdateRule();

    /******************************************************
    函数 : SetUserInfo
    功能描述 : 设置用户信息(用户名密码) 用于正确执行GetToken
    输入参数 : STRUCT_USER & 用户信息
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetUserInfo(STRUCT_USER &);


    /******************************************************
    函数 : SetOperaType
    功能描述 : 设置通过webservice接口进行的操作类型 主要是为了区分全量更新还是增量更新
    输入参数 : OP_TYPE  操作类型
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetOperaType(OP_TYPE);


private:
    QString m_token;           //和数据库服务交互的凭证

    OP_TYPE m_opType;          //访问webservice接口进行的操作类型

    STRUCT_USER m_userinfo;    //用户信息

    CLS_DownloadManager m_downloadMgr;  //文件下载管理类  用于下载规则对应的脚本文件

signals:

    //更新完毕
    void SigUpdateOver();

    //更新错误
    void SigUpdateErr();
};

#endif // CLS_RULEUPDATEMANAGER_H


