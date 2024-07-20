#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif


#include "CLS_SshManager.h"



CLS_SshManager::CLS_SshManager(QObject *parent)
    : QObject{parent}
    ,m_psession(nullptr)
    ,m_pchannel(nullptr)
    ,m_psftp(nullptr)
    ,m_sock(-1)
    ,m_port(22)
    ,m_blExecShell(false)
    ,m_blUploadFile(false)
{

}


void CLS_SshManager::InitSshFun()
{
    int ret;
    if(libssh2_init(0) != 0)
    {
        qDebug()<<"libssh2 initialization failed : "<<ret;
    }
}

QString CLS_SshManager::GetUploadFile()
{
    if(m_loaclfile.isEmpty())
    {
        return QStringLiteral("");
    }

    QFileInfo qfileInfo;
    qfileInfo.setFile(m_loaclfile);
    return qfileInfo.fileName();
}

void CLS_SshManager::SetServerIp(const QString &ip)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [设置远端服务器ip] "<<ip;
    m_serverip = ip;
}

void CLS_SshManager::SetServerPort(int port)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [设置远端服务器端口] "<<QString::number(port);
    m_port = port;
}

void CLS_SshManager::SetUserName(const QString & username)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [设置远端服务器用户名] "<<username;
    m_username = username;
}

void CLS_SshManager::SetPasswd(const QString &passwd)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [设置远端服务器密码] "<<passwd;
    m_passwd = passwd;
}

void CLS_SshManager::SetShellCommand(const QString &command)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [即将执行的脚本] "<<command;
    m_command = command;
}

void CLS_SshManager::SetSftpLoacalFile(const QString &localfile)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [本地脚本文件路径] "<<localfile;
    m_loaclfile = localfile;
}

void CLS_SshManager::SetSftpRemotePath(const QString &remotefile)
{
    qDebug()<<__FUNCTION__<<" "<<__LINE__<<" [远端脚本文件路径] "<<remotefile;
    m_remotefilepath = remotefile;
}

int CLS_SshManager::socketcon()
{
    WSADATA wsadata;
    int ret = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if(ret != 0)
    {
        qDebug()<<"WSAStartup failed with error : "<<ret;
        return -1;
    }

    m_sock= socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(m_port);
    sin.sin_addr.s_addr = inet_addr(m_serverip.toStdString().data());
    if(::connect(m_sock, reinterpret_cast<struct sockaddr*>(&sin),sizeof(struct sockaddr_in)) != 0)
    {
        qDebug()<<"websocekt failed to connect    [PORT]="<<m_port<<" [IP]="<<m_serverip;
        return -1;
    }

    return 0;
}

int CLS_SshManager::SshEstablishConn()
{
    int ret = 0;

    //初始化ssh会话对象 设置不阻塞
    m_psession = libssh2_session_init();
    if(m_psession == nullptr)
    {
        qDebug()<<"libssh2_session_init failed : "<<ret;
        return -1;
    }
    libssh2_session_set_blocking(m_psession, 0);


    //执行ssh握手
    while((ret = libssh2_session_handshake(m_psession, m_sock)) == LIBSSH2_ERROR_EAGAIN)
    {
        waitsocket(m_sock, m_psession);
    }
    if(ret != 0)
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
        return -1;
    }


    //使用 用户名+密码方式 验证
    int count = 0;
    while((ret = libssh2_userauth_password(m_psession,
                                           m_username.toStdString().data(),
                                           m_passwd.toStdString().data())) == LIBSSH2_ERROR_EAGAIN)
    {
        if(count >= const_iTryCount)
        {
            break;
        }
        count++;
        QThread::msleep(const_iWaitTime);
    }
    if(ret != 0)
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
        return -1;
    }


    //创建一个会话通道
    while((m_pchannel = libssh2_channel_open_session(m_psession)) == nullptr
          && (ret = libssh2_session_last_error(m_psession, nullptr, nullptr, 0)) == LIBSSH2_ERROR_EAGAIN)
    {
        waitsocket(m_sock, m_psession);
    }
    if(m_pchannel == nullptr)
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
        return -1;
    }

    return 0;
}


int CLS_SshManager::DisConnSsh()
{
    int ret;
    int count = 0;

    //释放通道资源
    if(m_pchannel)
    {
        if(m_blExecShell)
        {//等待远端关闭通道 注意如果没有执行过shell命令行则不需要执行以下函数

            while((ret = libssh2_channel_wait_closed(m_pchannel)) == LIBSSH2_ERROR_EAGAIN)
            {
                waitsocket(m_sock, m_psession);
            }

            if(ret != 0)
            {
               OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
               return -1;
            }

            //获取远端返回状态
            int exitcode = libssh2_channel_get_exit_status(m_pchannel);
            //获取远程通道退出信号
            char * exitsignal = nullptr;
            while((ret = libssh2_channel_get_exit_signal(m_pchannel,&exitsignal,nullptr, nullptr, nullptr, nullptr, nullptr)) == LIBSSH2_ERROR_EAGAIN)
            {
                waitsocket(m_sock, m_psession);
            }
            if(ret != 0)
            {
                OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
                return -1;
            }
            if(exitsignal != nullptr)
            {
                qDebug()<<"远端通道没有完全退出 退出信号 : "<<exitsignal<<" 退出状态 : "<<exitcode;
                return -1;
            }
        }


        //释放通道资源
        while((ret = libssh2_channel_free(m_pchannel)) == LIBSSH2_ERROR_EAGAIN)
        {
            waitsocket(m_sock, m_psession);
        }
        if(ret != 0)
        {
            OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
            return -1;
        }
    }


    //关闭会话
    if(m_psession)
    {
        while((ret = libssh2_session_disconnect(m_psession,"Actively close links")) == LIBSSH2_ERROR_EAGAIN)
        {
            waitsocket(m_sock, m_psession);
        }
        if(ret != 0)
        {
            OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
            return -1;
        }


        while((ret = libssh2_session_free(m_psession)) == LIBSSH2_ERROR_EAGAIN)
        {
            waitsocket(m_sock, m_psession);
        }
        if(ret != 0)
        {
            OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
            return -1;
        }
    }

    //关闭libssh2 所有资源
    libssh2_exit();

    //关闭socket连接
    closesocket(m_sock);
    WSACleanup();

    m_blExecShell  = false;
    m_blUploadFile = false;

    return 0;
}

int CLS_SshManager::ExecCommand()
{
    if(this->socketcon() != 0)
    {
      qDebug()<<"与主机"<<m_serverip<<" 的socket连接建立失败";
      return -1;
    }

    if(this->SshEstablishConn() != 0)
    {
       qDebug()<<"与主机"<<m_serverip<<" 的ssh连接建立失败";
       return -1;
    }

    m_blExecShell = true;
    int ret = 0;
    int count = 0;
    if(m_pchannel == nullptr || m_psession == nullptr)
    {
        return -1;
    }

//=======================================================================

    if((ret == libssh2_channel_request_pty(m_pchannel, "vt100")) == 0)
    {
        qDebug() <<"libssh2_channel_request_pty exec ok";
    }
    else
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
    }



    if((ret == libssh2_channel_shell(m_pchannel)) == 0)
    {
        qDebug() <<"libssh2_channel_shell exec ok";
    }
    else
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
    }

    libssh2_channel_set_blocking(m_pchannel, 1); //必须要设置为不阻塞
    m_command += "\n";                           //每个命令必须加回车，相当于回车键了
    size_t ss =libssh2_channel_write(m_pchannel, m_command.toStdString().data(), m_command.size());
    if(ss < 0)
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
    }

    if(libssh2_channel_flush(m_pchannel) == 0)
    {
        qDebug() <<"libssh2_channel_flush exec ok";
    }

    //libssh2_channel_set_blocking(m_pchannel, 0); //必须要设置为不阻塞
//=======================================================================

//    //执行命令
//    while((ret = libssh2_channel_exec(m_pchannel, m_command.toStdString().data())) == LIBSSH2_ERROR_EAGAIN)
//    {
//        waitsocket(m_sock, m_psession);
//    }
//    if(ret != 0)
//    {
//        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
//        return -1;
//    }


    //读取执行结果
    do
    {
        qDebug("===============================================================================\n");

        if(libssh2_channel_flush(m_pchannel) == 0)
        {
            qDebug() <<"libssh2_channel_flush exec ok";
        }

        char buffer[102400] = {0};
        while((ret = libssh2_channel_read(m_pchannel, buffer, sizeof(buffer))) == LIBSSH2_ERROR_EAGAIN)
        {
            waitsocket(m_sock, m_psession);
        }

        if(ret < 0)
        {
            OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
            return -1;
        }
        else if(ret > 0)
        {
            QString res = buffer;

//            for(int i = 0; i < ret; ++i)
//            {
//                qDebug("buffer[%d]=%d   |%c|\n",i,buffer[i],buffer[i]);
//            }

            qDebug("[RET]%s\n",buffer);

//            Q_EMIT SigEchoRes(res);
//            if(res.trimmed() == "lzq@lzq:~$")
//            {
//                qDebug()<<"[进入即将退出流程]"<<endl;
//            }
        }



    }while(ret > 0);


//    if(this->DisConnSsh() != 0)
//    {
//        qDebug()<<"与主机"<<m_serverip<<" 连接的断开失败";
//        return -1;
//    }

    return 0;
}

int CLS_SshManager::UploadFile()
{
    if(this->socketcon() != 0)
    {
      qDebug()<<"与主机"<<m_serverip<<" 的socket连接建立失败";
      return -1;
    }

    if(this->SshEstablishConn() != 0)
    {
       qDebug()<<"与主机"<<m_serverip<<" 的ssh连接建立失败";
       return -1;
    }


    m_blUploadFile = true;
    int ret = 0;

    if(m_psession == nullptr)
    {
       qDebug()<<"m_psession is nullptr";
       return -1;
    }


    //打开文件读取数据
    QFile file;
    file.setFileName(m_loaclfile);
    if(!file.exists())
    {
        qDebug()<<m_loaclfile<<" is note exit";
        return -1;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"qfile open "<<m_loaclfile<<" error";
        return -1;
    }
    QDataStream readdata;
    readdata.setDevice(&file);


    libssh2_session_set_blocking(m_psession, 1);  //设置成阻塞 上传设置文件必要

    int count = 0;
    do
    {
       m_psftp = libssh2_sftp_init(m_psession);

       if(!m_psftp && (libssh2_session_last_errno(m_psession) != LIBSSH2_ERROR_EAGAIN))
       {
           qDebug()<<"libssh2_sftp_init err";
           return -1;
       }

       if(count >= const_iTryCount)
       {
           break;
       }
       count++;

       QThread::msleep(const_iWaitTime);

    } while(!m_psftp);


    LIBSSH2_SFTP_HANDLE * pSftpHandle;
    count = 0;
    do
    {
       pSftpHandle = libssh2_sftp_open(m_psftp,
                                       m_remotefilepath.toStdString().data(),
                                       LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                                       LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

      if(!pSftpHandle && (libssh2_session_last_errno(m_psession) != LIBSSH2_ERROR_EAGAIN))
      {
          qDebug()<<"libssh2_sftp_open err";
          return -1;
      }

      if(count >= const_iTryCount)
      {
          break;
      }
      count++;

    } while(!pSftpHandle);



    char mem[102400];
    size_t nread;
    char * ptr = nullptr;
    do
    {
       nread = readdata.readRawData(mem,sizeof(mem));
       if(nread <= 0)
       {
           qDebug()<<"lzq "<<__FUNCTION__<<" "<<__LINE__;
           break;
       }
       ptr = mem;

       do
       {
           ret = libssh2_sftp_write(pSftpHandle, ptr, nread);

           if(ret < 0)
           {
               qDebug()<<"lzq "<<__FUNCTION__<<" "<<__LINE__;
               break;
           }
           ptr += ret;
           nread -= ret;
       } while(nread);

    } while(ret > 0);

    libssh2_sftp_close(pSftpHandle);

    libssh2_sftp_shutdown(m_psftp);

    file.close();

    if(this->DisConnSsh() != 0)
    {
        qDebug()<<"与主机"<<m_serverip<<" 连接的断开失败";
        return -1;
    }

    return 0;
}
