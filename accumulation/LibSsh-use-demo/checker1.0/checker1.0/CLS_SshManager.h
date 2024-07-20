/*
 *  ssh管理类 可以使用qt线程池进行调用，ssh功能方面调用的是第三方开源库libssh2
 *           每个Ssh项控件中都有一个ssh管理类
 *
 *           【TODO本类中通过ssh执行命令行功能，目前并非支持所有的shell命令，所以使用前需要测试】
 *           【TODO本类中上传文件的功能，上传的路径需要进行测试，可能有权限问题】
 */


#ifndef CLS_SSHMANAGER_H
#define CLS_SSHMANAGER_H

#pragma comment(lib, "ws2_32.lib")

#include <QDataStream>
#include <QFile>
#include <QObject>
#include <QRunnable>
#include <QDebug>
#include "libssh2_config.h"
#include "libssh2.h"
#include "libssh2_sftp.h"
#include "PublicData.h"
#include <QVector>
#include "CLS_LoadRuleManager.h"
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


const int const_iTryCount = 20;  //重复次数
const int const_iWaitTime = 5; //等待时间 单位ms


static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = nullptr;
    fd_set *readfd = nullptr;
    int dir;

    timeout.tv_sec = 120;
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


class CLS_SshManager : public QObject , public QRunnable
{
    Q_OBJECT
public:
    CLS_SshManager(QObject *parent = nullptr);

    void run() override;

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
    int ExecCommand(QJsonObject _userdata = QJsonObject());


    /******************************************************
    函数 :  UploadFile
    功能描述 : 将本地指定文件上传到服务器指定位置
    输入参数 : 无
    输出参数 : 无
    返回值 : 返回0表示成功  返回-1表示失败
    其它 : 无
    *******************************************************/
    int UploadFile();


    /******************************************************
    函数 :  AddSelRule
    功能描述 : 增加选择的规则
    输入参数 : QString 规则对应的id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void AddSelRule(QString);


    /******************************************************
    函数 :  AddSelBatRule
    功能描述 : 增加选择的规则组
    输入参数 : QString 规则组对应的id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void AddSelBatRule(QString);


    /******************************************************
    函数 :  DelSelRule
    功能描述 : 删除选择的规则
    输入参数 : QString 规则对应的id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void DelSelRule(QString);


    /******************************************************
    函数 :  DelSelBatRule
    功能描述 : 删除选择的规则组
    输入参数 : QString 规则组对应的id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void DelSelBatRule(QString);


    /******************************************************
    函数 :  SetSshCfg
    功能描述 : 设置ssh连接信息
    输入参数 : STRUCT_SSH ssh信息
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSshCfg(STRUCT_SSH);


    /******************************************************
    函数 :  SetSshId
    功能描述 : 设置此管理类对象对应的Ssh项控件的Id号
    输入参数 : int  id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSshId(int);


    /******************************************************
    函数 :  GetSelRule
    功能描述 : 向外界返回已经选择的规则
    输入参数 : 无
    输出参数 : 无
    返回值 : QVector<QString>  已选择规则id的集合
    其它 : 无
    *******************************************************/
    QVector<QString> GetSelRule();


    /******************************************************
    函数 :  GetSelBatRule
    功能描述 : 向外界返回已经选择的规则组
    输入参数 : 无
    输出参数 : 无
    返回值 : QVector<QString>  已选择规则组id的集合
    其它 : 无
    *******************************************************/
    QVector<QString> GetSelBatRule();


    /******************************************************
    函数 :  GetSshCfg
    功能描述 : 获取Ssh配置信息
    输入参数 : 无
    输出参数 : 无
    返回值 : STRUCT_SSH
    其它 : 无
    *******************************************************/
    STRUCT_SSH GetSshCfg();


    /******************************************************
    函数 :  ClearSelRule
    功能描述 : 清空已选择的规则
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void ClearSelRule();


    /******************************************************
    函数 :  ClearSelBatRule
    功能描述 : 清空已选择的规则组
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void ClearSelBatRule();


private:
    /******************************************************
    函数 :  GenerateJsonEchoRes
    功能描述 : 将从Ssh接收到的字符串返回结果转换成json格式
    输入参数 : QJsonObject 存储规则(组)的id的json,下面是字段格式，可以根据情况扩展
                {
                  "ruleid":"",
                  "batruleid":""
                }
              QString 从Ssh返回的字符串结果
              EXEC_SCRIPT_STAT 脚本执行状态
    输出参数 : 无
    返回值 : QJsonObject json
    其它 :  返回的json结构
            {
                "userdata":
                {
                  "ruleid":"",
                  "batruleid":""
                },
                "res":"",         //存储脚本执行的状态或者ssh执行脚本返回的结果
                "stat":0          //脚本执行状态
            }
    *******************************************************/
    QJsonObject GenerateJsonEchoRes(QJsonObject, QString, EXEC_SCRIPT_STAT);


signals:
    /******************************************************
    函数 :  SigEchoRes
    功能描述 :  信号，向外界传输ssh执行结果
    输入参数 : int Ssh项控件的Id号 QJsonObject  GenerateJsonEchoRes函数生成的json
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SigEchoRes(int,QJsonObject);

    /******************************************************
    函数 :  SigSshErrMsg
    功能描述 :  信号，向外界传输ssh本身的错误
    输入参数 : QString  文本信息，用于弹窗显示
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SigSshErrMsg(QString);
private:

    //主界面对应的Ssh项控件的Id号,此变量是将执行结果与显示界面控件对应的桥梁
    int m_id;

    //程序运行时存储的选中的规则 集合元素为规则的id
    QVector<QString> m_vecrule;

    //程序运行时存储的选择的规则组 集合元素为规则组的id
    QVector<QString> m_vecbatrule;

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
