&emsp;&emsp;在工作中使用qt，发现qt生成的日志文件和在终端输出的中文内容总是出现乱码。这已经影响了开发和排查问题的效率，于是决定研究一下。开发平台是在win10下，开发环境是qt5.15.2+vs2019。

&emsp;&emsp;经过排查发现，程序中使用的日志库是公司底层模块同事用C/C++语言写的，也就是说每次qt程序写日志都是将日志内容传给C/C++语言程序再生成日志文件或者输出到终端，为什么不直接用qt去实现日志功能，我只能说是历史包袱。 

&emsp;&emsp;上网查了很多资料，知道是qt和C/C++字符编码格式的问题，于是写个demo进行测试，如下。


```c
#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //设置执行字符编码
#endif

#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <QLibrary>

#ifdef Q_OS_WIN
  #include <windows.h>
#endif


/*
 *   windows下需要使用
 *   下面这连个转换函数也是从网上抄的，实际上实现的是多字节和宽字节互相转换的功能
 *   而中文对应的UTF8正好是多字节(3个字节表示)，而中文对应的GBK正好是宽字节(固定的2个字节表示)
 */
#ifdef Q_OS_WIN

std::string UTF8ToGBK(const char* strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len+1];
    memset(wszGBK, 0, len*2+2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len+1];
    memset(szGBK, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    if(wszGBK) delete[] wszGBK;
    if(szGBK) delete[] szGBK;
    return strTemp;
}

std::string GBKToUTF8(const char* strGBK)
{
    int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if(wstr) delete[] wstr;
    if(str) delete[] str;
    return strTemp;
}

#endif



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    //设置中文编码
    #if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))

        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);

    #else

        QTextCodec *codec = QTextCodec::codecForName("UTF-8");   //设置本地字符编码
        QTextCodec::setCodecForLocale(codec);
        qDebug()<<"本地环境编码 = "<<codec->name();

    #endif


//中文编码16进制查询网站https://www.qqxiuzi.cn/bianma/zifuji.php

/*
    测试字符串输出中文是否正确，有很多转换的方式，如下，
    qDebug() << str;
    qDebug() << QStringLiteral("2中文");
    qDebug() << QString::fromLatin1("3中文");
    qDebug() << QString::fromLocal8Bit("4中文");
    qDebug() << QString::fromUtf8("5中文");
    qDebug() << QString::fromWCharArray(L"6中文");
*/


/*数据输出测试

  windwos下 :
                终端显示       qtide
                日志文件       将日志内容输出到txt文件中

  linux下 :
                终端显示       默认终端  qtide
                日志文件       将日志内容输出到文件中

*/


//C语言风格代码测试 char const * p
{
    char const * ch = " C 常量指针变量指向中文字符串，然后输出\n";

    //使用qt方式输出
    qDebug()<<"C Format Code "<<__LINE__<<" "<<ch;

#ifdef Q_OS_WIN
    //转码输出
    printf("C Format Code %d %s",__LINE__,UTF8ToGBK(ch).data());
    fflush(stdout); //使得printf按顺序输出与编码无关

    std::cout<<"C Format Code "<<__LINE__<<" "<<UTF8ToGBK(ch)<<std::endl;
#endif

    //不转码输出用作参照
    printf("C Format Code %d %s",__LINE__,ch);
    fflush(stdout);
    std::cout<<"C Format Code "<<__LINE__<<" "<<ch<<std::endl;


    //使用C/C++的方式写入文件
    FILE * fp;
#ifdef Q_OS_WIN
    //windwos下转码写入
    fp = fopen("./C/cwirteToGbk.txt","w");
    if(fp)
    {
        fwrite(UTF8ToGBK(ch).data(),1,strlen(UTF8ToGBK(ch).data()),fp);
    }
    fclose(fp);
#endif

    //不转码写入
    fp = fopen("./C/cwirte.txt","w");
    if(fp)
    {
        fwrite(ch,1,strlen(ch),fp);
    }
    fclose(fp);

}


//C++ 风格代码测试 string
{
    std::string cplusstr = " 中文\n";

    //qt方式输出
    qDebug()<<"C++ Format Code "<<__LINE__<<" "<<cplusstr.data();


    //C++方式输出
#ifdef Q_OS_WIN
    std::cout<<"C++ Format Code "<<__LINE__<<" "<<UTF8ToGBK(cplusstr.data());
    fflush(stdout);
#endif

    std::cout<<"C++ Format Code "<<__LINE__<<" "<<cplusstr.data();
    fflush(stdout);

    //C++方式写入文件
    std::ofstream ofs;
#ifdef Q_OS_WIN
    ofs.open("./CPLUSPLUS/cpluswirteToGbk.txt", std::ios::out);
    ofs << UTF8ToGBK(cplusstr.data());
    ofs.close();
#endif

    ofs.open("./CPLUSPLUS/cpluswirte.txt", std::ios::out);
    ofs << cplusstr;
    ofs.close();
}


//qt 风格代码测试 QString
{
    QString qtest = "  中文";
    qDebug()<<"qt Format Code "<<__LINE__<<qtest;

    //转换成不同的编码进行输出
    QByteArray array = qtest.toUtf8();
    qDebug()<<"qt Format Code "<<__LINE__<<array.constData();
    qDebug()<<"qt Format Code "<<__LINE__<<" array hex = "<<array.toHex();

    array = qtest.toLocal8Bit();
    qDebug()<<"qt Format Code "<<__LINE__<<array.constData();
    qDebug()<<"qt Format Code "<<__LINE__<<" array hex = "<<array.toHex();

    //qt方式写文件
    QFile file("./QT/qtwrite.txt");
    file.open(QIODevice::WriteOnly);
    file.write(qtest.toStdString().data(),qtest.toStdString().size());
    file.close();

    file.setFileName("./QT/qtwriteToUtf-8.txt");
    file.open(QIODevice::WriteOnly);
    file.write(qtest.toUtf8(),(qtest.toUtf8()).size());
    file.close();

    file.setFileName("./QT/qtwriteToGbk.txt");
    file.open(QIODevice::WriteOnly);
    file.write(qtest.toLocal8Bit(),(qtest.toLocal8Bit()).size());
    file.close();
}


/*
 *  与动态库交互数据
 *
 *  向C/C++库中传入数据和从C/C++中获取数据测试
 *  C和C++库在windwos下 V2019 编译
 */

//windows下的测试
#ifdef Q_OS_WIN
{
      //定义函数指针
      typedef std::string (*ptransdatastr)(std::string);    //输入string返回string
      typedef char * (*ptransdatachar)(char *);             //输入char * 返回char *


      QLibrary * qlib = new QLibrary();
      qlib->setFileName("./candcplusforwin.dll");
      if(qlib->load())
      {
          ptransdatastr ptransdatastrfun  = (ptransdatastr)(qlib->resolve("transdatastr"));
          if(ptransdatastrfun)
          {
                QString cplus = " 中文";
                std::string dllredata = ptransdatastrfun(UTF8ToGBK(cplus.toLocal8Bit().constData())).data();

                qDebug()<<"return from dll transdatastr : "<<__LINE__<<dllredata.data();

                qDebug()<<"return from dll transdatastr : "<<__LINE__<<GBKToUTF8(dllredata.data()).data();
          }
          else
          {
                qDebug()<<"解析符号transdatastr失败";
          }


          ptransdatachar ptransdatacharfun = (ptransdatachar)(qlib->resolve("transdatachar"));
          if(ptransdatacharfun)
          {
              QString cplus = " 中文";
              std::string dllredata = ptransdatacharfun((char *)UTF8ToGBK(cplus.toUtf8().constData()).data());

              qDebug()<<"return from dll transdatachar : "<<__LINE__<<dllredata.data();

              qDebug()<<"return from dll transdatachar : "<<__LINE__<<GBKToUTF8(dllredata.data()).data();
          }
          else
          {
              qDebug()<<"解析符号transdatachar失败";
          }
      }
      else
      {
          qDebug()<<"加载库candcplusforwin.dll失败";
      }
}
#endif

    return a.exec();
}



/*
    本地环境编码 =  "UTF-8"
    C Format Code  123    C 常量指针变量指向中文字符串，然后输出

    C++ Format Code  167    中文

    qt Format Code  196 "  中文"
    qt Format Code  200   中文
    qt Format Code  201  array hex =  "2020e4b8ade69687"
    qt Format Code  204   中文
    qt Format Code  205  array hex =  "2020e4b8ade69687"
    return from dll transdatastr :  250 ??????c++??????  ????
    return from dll transdatastr :  252 来自于c++库数据  中文
    return from dll transdatachar :  266 ??????c++??????  ????
    return from dll transdatachar :  268 来自于c++库数据  中文
    C Format Code 127  C 常量指针变量指向中文字符串，然后输出
    C Format Code 130  C 常量指针变量指向中文字符串，然后输出

    C Format Code 134  C 甯搁噺鎸囬拡鍙橀噺鎸囧悜涓枃瀛楃涓诧紝鐒跺悗杈撳嚭
    C Format Code 136  C 甯搁噺鎸囬拡鍙橀噺鎸囧悜涓枃瀛楃涓诧紝鐒跺悗杈撳嚭

    C++ Format Code 172  中文
    C++ Format Code 176  涓枃
    transdatastr  9   中文
    transdatastr 10   中文
    transdatachar  24   中文
    transdatachar 25   中文
*/
```


&emsp;&emsp;下面是C/C++动态库的代码

```c
string transdatastr(string str)
{
	cout << __FUNCTION__ << "  " << __LINE__ << "  " << str << endl;
	printf("%s %d  %s\n", __FUNCTION__, __LINE__,str.data());
	str = "来自于c++库数据 " + str;
	

	ofstream ofs;
	ofs.open("./windllwritestr.txt", ios::out);
	ofs << str;
	ofs.close();

	return  str;
}

char* transdatachar(char* ch)
{
	cout << __FUNCTION__ << "  " << __LINE__ << "  " << ch << endl;
	printf("%s %d  %s\n", __FUNCTION__, __LINE__, ch);
	string temp = ch;
	temp = "来自于c++库数据 " + temp;

	ofstream ofs;
	ofs.open("./windllwritech.txt", ios::out);
	ofs << ch;
	ofs.close();

	return (char *)temp.data();
}
```


&emsp;&emsp;我认为日志内容选择UTF-8编码格式就挺好，毕竟兼容性强，windwos和linux上都可以使用。所以在代码中我将执行字符集和本地字符编码都设置成了UTF-8。

&emsp;&emsp;在获取终端输出的时候，需要注意，我原本想使用debugview++输出上面的所有内容，结果发现C/C++输出的内容并没有显示，我只能在qt ide上的应用程序输出窗口显示，此时要注意这个窗口是以什么编码格式显示内容的，我这里设置的是GBK，所以下面代码中转码不会输出乱码，不转码输出乱码。

```c
    #ifdef Q_OS_WIN
        //转码输出
        printf("C Format Code %d %s",__LINE__,UTF8ToGBK(ch).data());
        fflush(stdout); //使得printf按顺序输出与编码无关

        std::cout<<"C Format Code "<<__LINE__<<" "<<UTF8ToGBK(ch)<<std::endl;
    #endif

        //不转码输出用作参照
        printf("C Format Code %d %s",__LINE__,ch);
        fflush(stdout);
        std::cout<<"C Format Code "<<__LINE__<<" "<<ch<<std::endl;
```

&emsp;&emsp;下图是设置窗口显示内容编码格式的位置

![avatar][pic]


[pic]:data:image/png;base64,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

