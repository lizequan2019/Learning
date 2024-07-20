/*
 *  管理访问Webservice接口的类
 *  此类是一个单例类
 */
#ifndef CLS_WEBSERVICEMANAGER_H
#define CLS_WEBSERVICEMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <mutex>
#include "PublicData.h"

class CLS_WebServiceManager : public QObject
{
    Q_OBJECT

private:
    CLS_WebServiceManager() = default;
    ~CLS_WebServiceManager() = default;
    CLS_WebServiceManager(const CLS_WebServiceManager &) = delete;
    CLS_WebServiceManager & operator= (const CLS_WebServiceManager&) = delete;

public:
    static std::mutex m_mutex;
    static CLS_WebServiceManager * pInstance;
    static CLS_WebServiceManager * getInstance();
    static void Release(); //手动回收
    static std::once_flag flag_new;

    /******************************************************
    函数 :  SendPost
    功能描述 : 访问post接口        和服务器交互的凭证
    输入参数 : QString        url
              QJsonObject   要发送给服务器的数据
              QJsonObject & 服务器返回的数据
              QString       token 有的post接口需要在header中传递token值
    输出参数 : 无
    返回值 : int 返回0正确   返回-1访问超时  返回-2服务器返回错误
    其它 : 无
    *******************************************************/
    int SendPost(QString, QJsonObject ,QJsonObject &,  QString = "");

private:
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
};


#endif // CLS_WEBSERVICEMANAGER_H
