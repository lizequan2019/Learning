/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "CLS_HtmlToPdf.h"


CLS_HtmlToPdf::CLS_HtmlToPdf()
    :m_page(new QWebEnginePage)
    ,m_blexportAll(false)
{
    connect(m_page.data(), &QWebEnginePage::loadFinished,this, &CLS_HtmlToPdf::loadFinished);
    connect(m_page.data(), &QWebEnginePage::pdfPrintingFinished,this, &CLS_HtmlToPdf::pdfPrintingFinished);
}


void CLS_HtmlToPdf::Convert()
{
   //加载
   m_page->load(QUrl::fromUserInput(m_qstrHtmlurl));
}

void CLS_HtmlToPdf::SetHtmlPath(QString _qstr)
{
   m_qstrHtmlurl = _qstr;
}

void CLS_HtmlToPdf::SetPdfPath(QString _qstr)
{
   m_qstrPdfPath = _qstr;
}


void CLS_HtmlToPdf::loadFinished(bool ok)
{
    if (!ok)
    {
        qDebug() << "failed to load URL "<<m_qstrHtmlurl;
        return;
    }

    QFileInfo qfileinfo = QFileInfo(m_qstrPdfPath);
    QString  path = qfileinfo.absolutePath(); //获取文件路径(不包含文件名)

    //如果目录不存在就自动创建
    QDir qdir;
    if(!qdir.exists(path))
    {
       qdir.mkdir(path);
    }

    m_page->printToPdf(m_qstrPdfPath);
}

void CLS_HtmlToPdf::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (!success)
    {
        qDebug() << "failed to print to output file "<<filePath;
        QCoreApplication::exit(1);
    }
    else
    {
        if(m_blexportAll == true)
        {
            emit SignExportFinish(EXPORT_PDF_NUM_TYPE_MULTI);
        }
        else
        {
            emit SignExportFinish(EXPORT_PDF_NUM_TYPE_SINGLE);
        }
    }
}
