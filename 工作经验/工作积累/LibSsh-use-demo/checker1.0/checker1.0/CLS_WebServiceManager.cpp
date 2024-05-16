#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_WebServiceManager.h"

std::mutex CLS_WebServiceManager::m_mutex;
std::once_flag CLS_WebServiceManager::flag_new;
CLS_WebServiceManager * CLS_WebServiceManager::pInstance = nullptr;
CLS_WebServiceManager * CLS_WebServiceManager::getInstance()
{
    std::call_once (flag_new,[&]()
    {
        if(!pInstance)
        {
           pInstance = new CLS_WebServiceManager();

        }
    });

    return pInstance;
}

void CLS_WebServiceManager::Release()
{
    std::lock_guard<std::mutex> lock_(m_mutex);
    if(pInstance)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}


int CLS_WebServiceManager::SendPost(QString _url, QJsonObject _jsoncontent, QJsonObject & _jsonret, QString _token)
{
    int ret = ACCESS_WEBSER_RES_ERR;

    QNetworkAccessManager networkAccessManager;
    QNetworkRequest  qNetReq;
    qNetReq.setUrl(_url);
    qNetReq.setRawHeader("Content-Type", "charset='utf-8'");
    qNetReq.setRawHeader("Content-Type", "application/json");
    qNetReq.setRawHeader("Accept-charset", "utf-8");
    qNetReq.setRawHeader("Accept-Encoding", "utf-8");
    qNetReq.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)");
    qNetReq.setRawHeader("token", _token.toStdString().data());


    //json文本内容转换为字符串
    QJsonDocument qjsonDoc;
    qjsonDoc.setObject(_jsoncontent);

    QEventLoop eventloop;
    QTimer requestTimeoutTimer;
    requestTimeoutTimer.setSingleShot(true);// 单次触发
    requestTimeoutTimer.setInterval(10000); //超时时间10s

    QObject::connect(&requestTimeoutTimer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
    requestTimeoutTimer.start();

    QObject::connect(&networkAccessManager, &QNetworkAccessManager::finished, &eventloop, [&](QNetworkReply* )
    {
        requestTimeoutTimer.stop();
        eventloop.quit();
    });


    QNetworkReply* replay;
    replay  = networkAccessManager.post(qNetReq, qjsonDoc.toJson()); //访问接口
    eventloop.exec();


    if (requestTimeoutTimer.isActive())
    {
        qDebug("[%s] timeout", __FUNCTION__);
        requestTimeoutTimer.stop();
        ret = ACCESS_WEBSER_RES_TIMEOUT;
    }

    if(replay && (replay->error() == QNetworkReply::NoError))
    {
        QString qstrReplay = replay->readAll();
        qjsonDoc = QJsonDocument::fromJson(qstrReplay.toStdString().data());
        if(qjsonDoc.isObject())
        {
            _jsonret = qjsonDoc.object();
            ret = ACCESS_WEBSER_RES_OK;
        }
    }

    return ret;
}

