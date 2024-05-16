#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtCore>
#include <QtNetwork>

struct STRUNCT_DownloadInfo
{
    STRUNCT_DownloadInfo() {}
    QUrl m_qurlString;
    QString m_qstrLocalPath;
};

class CLS_DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit CLS_DownloadManager(QObject *parent = nullptr);

    void append(const STRUNCT_DownloadInfo & _structDownloadInfo);

    bool isEmpty();

    void closeDownload();

    /******************************************************
    函数 : SetDownloadCfg
    功能描述 :  设置每次下载的信息
    输入参数 : _qstrurl 表示服务器文件路径(包含文件名)
             _qstrlocalfile 表示保存到本地文件的路径(包含文件名)
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetDownloadCfg(QString _qstrurl, QString _qstrlocalfile);

signals:
    void finished();

private slots:
    void startNextDownload();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();

private:
    bool isHttpRedirect() const;
    void reportRedirect();

    QNetworkAccessManager m_networkMgr;
    QQueue<STRUNCT_DownloadInfo> m_queueDownload;
    QNetworkReply *m_pCurrentDownload = nullptr;
    QFile m_fileOutput;
    QElapsedTimer m_timerDownload;

    int m_iDownloadedCount = 0;
    int m_iTotalCount = 0;
};

#endif
