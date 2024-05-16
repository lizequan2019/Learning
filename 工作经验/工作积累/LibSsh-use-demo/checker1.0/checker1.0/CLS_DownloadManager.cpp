#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_DownloadManager.h"
#include <QTextStream>
#include <QDebug>

using namespace std;

CLS_DownloadManager::CLS_DownloadManager(QObject *parent)
    : QObject(parent)
{}

bool CLS_DownloadManager::isEmpty()
{
    return m_queueDownload.isEmpty();
}

void CLS_DownloadManager::closeDownload()
{
    if (m_pCurrentDownload != nullptr)
    {
        disconnect(m_pCurrentDownload, &QNetworkReply::downloadProgress, this, &CLS_DownloadManager::downloadProgress);
        disconnect(m_pCurrentDownload, &QNetworkReply::finished, this, &CLS_DownloadManager::downloadFinished);
        disconnect(m_pCurrentDownload, &QNetworkReply::readyRead, this, &CLS_DownloadManager::downloadReadyRead);

        m_pCurrentDownload->abort();
        m_pCurrentDownload->deleteLater();
        m_pCurrentDownload = nullptr;
        m_queueDownload.clear();
    }
}

void CLS_DownloadManager::SetDownloadCfg(QString _qstrurl, QString _qstrlocalfile)
{
    STRUNCT_DownloadInfo structDownloadInfo;
    structDownloadInfo.m_qurlString = _qstrurl;
    structDownloadInfo.m_qstrLocalPath = _qstrlocalfile;
    this->append(structDownloadInfo);
}

void CLS_DownloadManager::append(const STRUNCT_DownloadInfo & _structDownloadInfo)
{
    if (m_queueDownload.isEmpty())
    {
        QTimer::singleShot(0, this, &CLS_DownloadManager::startNextDownload);
    }

    m_queueDownload.enqueue(_structDownloadInfo);
    ++m_iTotalCount;
}

void CLS_DownloadManager::startNextDownload()
{
    if (m_queueDownload.isEmpty())
    {
        qDebug("CLS_DownloadManager::startNextDownload: %d/%d files downloaded successfully\n", m_iDownloadedCount, m_iTotalCount);
        emit finished();
        return;
    }

    STRUNCT_DownloadInfo structDownloadInfo = m_queueDownload.dequeue();

    QString filename = structDownloadInfo.m_qstrLocalPath;

    QFileInfo qfileinfoSrc(filename);
    QString qstrGetFile = qfileinfoSrc.absoluteDir().path();
    QDir dir(qstrGetFile);
    if(!dir.exists()) {
        dir.mkpath(qstrGetFile);
    }

    m_fileOutput.setFileName(filename);
    if (!m_fileOutput.open(QIODevice::WriteOnly))
    {
        qDebug() << "CLS_DownloadManager::startNextDownload: filename=" << filename << ",m_fileOutput.errorString=" << m_fileOutput.errorString();
        startNextDownload();
        return;
    }

    QNetworkRequest request(structDownloadInfo.m_qurlString);
    m_pCurrentDownload = m_networkMgr.get(request);
    if (m_pCurrentDownload != nullptr)
    {
        connect(m_pCurrentDownload, &QNetworkReply::downloadProgress, this, &CLS_DownloadManager::downloadProgress);
        connect(m_pCurrentDownload, &QNetworkReply::finished, this, &CLS_DownloadManager::downloadFinished);
        connect(m_pCurrentDownload, &QNetworkReply::readyRead, this, &CLS_DownloadManager::downloadReadyRead);
    }

    //准备输出
    qDebug("CLS_DownloadManager::startNextDownload: Downloading %s...", structDownloadInfo.m_qurlString.toEncoded().constData());

    m_timerDownload.start();
}

void CLS_DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    //计算下载速度
    double speed = bytesReceived * 1000.0 / m_timerDownload.elapsed();
    QString unit;
    if (speed < 1024)
    {
        unit = "bytes/sec";
    }
    else if (speed < 1024 * 1024)
    {
        speed /= 1024;
        unit = "kB/s";
    }
    else
    {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }
}

void CLS_DownloadManager::downloadFinished()
{
    m_fileOutput.close();

    if (m_pCurrentDownload != nullptr)
    {
        if (m_pCurrentDownload->error())
        {
            //下载失败
            qDebug() << "CLS_DownloadManager::downloadFinished: download fail:m_pCurrentDownload->errorString=" << m_pCurrentDownload->errorString();
            m_fileOutput.remove();
        }
        else
        {
            //让我们看看它是否真的是重定向
            if (isHttpRedirect())
            {
                reportRedirect();
                m_fileOutput.remove();
            }
            else
            {
                qDebug("CLS_DownloadManager::downloadFinished: download Succeeded.");
                ++m_iDownloadedCount;
            }
        }

        m_pCurrentDownload->deleteLater();
    }
    startNextDownload();
}

void CLS_DownloadManager::downloadReadyRead()
{
    if (m_pCurrentDownload != nullptr)
    {
        m_fileOutput.write(m_pCurrentDownload->readAll());
    }
}

bool CLS_DownloadManager::isHttpRedirect() const
{
    if (m_pCurrentDownload != nullptr)
    {
        int statusCode = m_pCurrentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        return statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 305 || statusCode == 307 || statusCode == 308;
    }
    return false;
}

void CLS_DownloadManager::reportRedirect()
{
    if (m_pCurrentDownload != nullptr)
    {
        int statusCode = m_pCurrentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QUrl requestUrl = m_pCurrentDownload->request().url();
        QTextStream(stderr) << "Request: " << requestUrl.toDisplayString() << " was redirected with code: " << statusCode << '\n';

        QVariant target = m_pCurrentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (!target.isValid())
        {
            return;
        }
        QUrl redirectUrl = target.toUrl();
        if (redirectUrl.isRelative())
        {
            redirectUrl = requestUrl.resolved(redirectUrl);
        }
        QTextStream(stderr) << "Redirected to: " << redirectUrl.toDisplayString() << '\n';
    }
}
