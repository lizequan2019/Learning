/*
 *  本类主要作用是将本地html文件转换为pdf
 *  本类基于qt自带工程项目修改
 *  TODO: 本类目前导出多分pdf文件时，在逻辑上还需要和主界面进行交互，逻辑复杂
 *        这是一个需要优化的点，应该时外部直接将所有需要转换的文件参数一次性全传进来
 */

#ifndef CLS_HTMLTOPDF_H
#define CLS_HTMLTOPDF_H

#include <QDir>
#include <QObject>
#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QWebEnginePage>
#include <functional>
#include <QDebug>
#include "PublicData.h"

using namespace std;
using namespace std::placeholders;


class CLS_HtmlToPdf : public QObject
{
    Q_OBJECT

public:
    explicit CLS_HtmlToPdf();

    void Convert();

    /******************************************************
    函数 :  SetHtmlPath
    功能描述 :  设置要被转化的html文件参数
    输入参数 :  QString 路径+文件名
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetHtmlPath(QString _qstr);

    /******************************************************
    函数 :  SetPdfPath
    功能描述 :  设置导出的pdf文件参数
    输入参数 :  QString 路径+文件名
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetPdfPath(QString _qstr);

    //导出pdf数量类型标志位  true表示导出全部的pdf false表示导出当前pdf文件
    bool m_blexportAll;

signals:

    /******************************************************
    函数 :  SignExportFinish
    功能描述 :  发射导出pdf完成信号
    输入参数 :  int 导出类型 EXPORT_PDF_NUM_TYPE  表示导出pdf数量类型
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SignExportFinish(int);

private slots:

    void loadFinished(bool ok);

    void pdfPrintingFinished(const QString &filePath, bool success);

private:

    //需要加载的html  路径+文件名字
    QString m_qstrHtmlurl;

    //需要加载的pdf   路径+文件名字
    QString m_qstrPdfPath;

    //QScopedPointer 类似智能指针功能 目前先这么用
    QScopedPointer<QWebEnginePage> m_page;
};


#endif // CLS_HTMLTOPDF_H
