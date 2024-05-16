#ifndef CLS_SSHMANAGER_H
#define CLS_SSHMANAGER_H

#pragma comment(lib, "ws2_32.lib")

#include <QDataStream>
#include <QFile>
#include <QObject>
#include <QDebug>
#include <QVector>
#include <QThread>
#include <QFileInfo>

#include "libssh2_config.h"
#include "libssh2.h"
#include "libssh2_sftp.h"


#ifdef HAVE_WINDOWS_H
# include <windows.h>
#endif
#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
# ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

//总等待时间是30s  原来ssh2 demo中写的都是while循环直至成功，这里加一个超时处理
const int const_iTryCount = 6000;  //重复次数
const int const_iWaitTime = 5;      //等待时间 单位ms


static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = nullptr;
    fd_set *readfd = nullptr;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
    {
        readfd = &fd;
    }

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
    {
        writefd = &fd;
    }

    rc = select(socket_fd + 1, readfd, writefd, nullptr, &timeout);

    return rc;
}

//输出libssh错误原因
static void  OutputErrReason(int _ret, int _line, const QString &_fun, const QString &_file)
{
    if(_ret == LIBSSH2_ERROR_SOCKET_NONE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] The socket is invalid";
    }
    else if(_ret == LIBSSH2_ERROR_BANNER_SEND)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] Unable to send banner to remote host";
    }
    else if(_ret == LIBSSH2_ERROR_KEX_FAILURE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] Encryption key exchange with the remote host failed";
    }
    else if(_ret == LIBSSH2_ERROR_SOCKET_SEND)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] Unable to send data on socket";
    }
    else if(_ret == LIBSSH2_ERROR_SOCKET_DISCONNECT)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] The socket was disconnected";
    }
    else if(_ret == LIBSSH2_ERROR_PROTO)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] An invalid SSH protocol response was received on the socket";
    }
    else if(_ret == LIBSSH2_ERROR_EAGAIN)
    {//这个属于IO阻塞 不算错误
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [WARNING] Marked for non-blocking I/O but the call would block";
    }
    else if(_ret == LIBSSH2_KNOWNHOST_CHECK_FAILURE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] something prevented the check to be made";
    }
    else if(_ret == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] no host match was found";
    }
    else if(_ret == LIBSSH2_KNOWNHOST_CHECK_MISMATCH)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] host was found, but the keys didn't match!";
    }
    else if(_ret == LIBSSH2_ERROR_ALLOC)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR] An internal memory allocation call failed.";
    }
    else if(_ret == LIBSSH2_ERROR_SOCKET_SEND)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  Unable to send data on socket.";
    }
    else if(_ret == LIBSSH2_ERROR_PASSWORD_EXPIRED)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  LIBSSH2_ERROR_PASSWORD_EXPIRED  目前不知道这是什么意思";
    }
    else if(_ret == LIBSSH2_ERROR_PASSWORD_EXPIRED)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  failed, invalid username/password or public/private key.";
    }
    else if(_ret == LIBSSH2_ERROR_SOCKET_NONE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  The socket is invalid.";
    }
    else if(_ret == LIBSSH2_ERROR_SOCKET_RECV)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  LIBSSH2_ERROR_SOCKET_RECV";
    }
    else if(_ret == LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE";
    }
    else if(_ret == LIBSSH2_ERROR_INVALID_MAC)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  LIBSSH2_ERROR_INVALID_MAC";
    }
    else if(_ret == LIBSSH2_ERROR_BAD_USE)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  LIBSSH2_ERROR_BAD_USE";
    }
    else if(_ret == 0)
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [OK] 执行正确";
    }
    else
    {
        qDebug()<<"[LINE] = "<<_line<<" [FUN] = "<<_fun<<" [file] = "<<_file<<"  [ERROR]  未知错误 错误码是"<<_ret;
    }
}


class CLS_SshManager : public QObject
{
    Q_OBJECT
public:
    CLS_SshManager(QObject *parent = nullptr);

    /******************************************************
    函数 :     InitSshFun
    功能描述 : 初始化libssh库函数
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 注意 这是一个静态成员函数，设置这个性质是根据libssh2官网api描述
    *******************************************************/
    static void InitSshFun();

    /******************************************************
    函数 :     GetUploadFile
    功能描述 :  获取上传文件名称
    输入参数 :
    输出参数 : 无
    返回值 : QString 文件名称
    其它 : 无
    *******************************************************/
    QString GetUploadFile();

    /******************************************************
    函数 :     SetServerIp
    功能描述 : 设置远程服务器ip
    输入参数 : QString ip
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetServerIp(const QString &ip);

    /******************************************************
    函数 :   SetServerPort
    功能描述 :    设置远程服务器端口，构造函数已经将其初始化22
    输入参数 : int 端口
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetServerPort(int port);

    /******************************************************
    函数 :  SetUserName
    功能描述 : 设置远程服务器登陆用户名
    输入参数 : QString  用户名
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetUserName(const QString & username);

    /******************************************************
    函数 :   SetPasswd
    功能描述 :  设置远程服务器密码
    输入参数 : QString  密码
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetPasswd(const QString & passwd);

    /******************************************************
    函数 :     SetShellCommand
    功能描述 :  设置要执行的shell命令,有的shell命令执行结果为空例如who am i
    输入参数 : QString  命令
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetShellCommand(const QString & command);

    /******************************************************
    函数 :  SetSftpLoacalFile
    功能描述 :  置将要上传的本地文件 路径+文件名
    输入参数 : 文件 路径+文件名
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/

    void SetSftpLoacalFile(const QString & localfile);
    /******************************************************
    函数 :   SetSftpRemotePath
    功能描述 :  设置远程服务器路径    路径
    输入参数 : QString  路径
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSftpRemotePath(const QString & remote);

    /******************************************************
    函数 :  socketcon
    功能描述 :  socket连接封装
    输入参数 : 无
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 无
    *******************************************************/
    int socketcon();

    /******************************************************
    函数 :   SshEstablishConn
    功能描述 :    建立ssh协议连接
    输入参数 : 无
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 无
    *******************************************************/
    int SshEstablishConn();

    /******************************************************
    函数 :  DisConnSsh
    功能描述 : 断开ssh连接
    输入参数 : 无
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 无
    *******************************************************/
    int DisConnSsh();

    //执行shell命令   返回0表示成功  返回-1表示失败
    /******************************************************
    函数 : ExecCommand
    功能描述 :  执行shell命令
    输入参数 : QJsonObject  向外传递信息时需要用到，可以不填写
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 此函数主要负责 执行shell命令和上传文件
    *******************************************************/
    int ExecCommand();

    /******************************************************
    函数 :  UploadFile
    功能描述 : 将本地指定文件上传到服务器指定位置
    输入参数 : 无
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 无
    *******************************************************/
    int UploadFile();

Q_SIGNALS:
    /******************************************************
    函数 :  SigEchoRes
    功能描述 :  信号，向外界传输ssh执行结果
    输入参数 : int Ssh项控件的Id号 QJsonObject  GenerateJsonEchoRes函数生成的json
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SigEchoRes(QString);


private:

    LIBSSH2_CHANNEL * m_pchannel;
    LIBSSH2_SESSION * m_psession;
    LIBSSH2_SFTP * m_psftp;

    //连接参数
    QString m_serverip;        //服务器ip
    int m_port;                //服务器端口
    QString m_username;        //用户名
    QString m_passwd;          //密码
    int m_sock;                //sock连接

    //功能参数
    QString m_command;         //shell命令
    QString m_loaclfile;       //sftp要上传的文件
    QString m_remotefilepath;  //远端的文件路径


    //执行上传文件和shell命令在断开连接时会有冲突，使用以下标识位解决
    bool m_blExecShell; //是否执行shell命令标志量
    bool m_blUploadFile; //是否上传文件标志量
};

#endif // CLS_SSHMANAGER_H
