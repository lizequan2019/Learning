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
     this->setAutoDelete(false);
}

void CLS_SshManager::run()
{
      int ret = -1;//存储上传脚本文件和执行脚本文件的结果

      //创建存放shell脚本的目录
      this->SetShellCommand(QString("mkdir ") + const_ServerShFilePath);
      if(ExecCommand() != 0)
      {
          qDebug()<<"在远端服务器上创建目录 "<<const_ServerShFilePath<<" 失败";
          emit SigSshErrMsg("目标主机" + m_serverip + " SSH连接失败,请检查输入的ip、用户名、密码信息和网络");
          return;
      }
      else
      {
          qDebug()<<"在远端服务器上创建目录 "<<const_ServerShFilePath<<" 成功";
      }
      //========================================================================================================================

      //遍历m_vecbatrule 和 m_vecrule 进行上传脚本和执行脚本   TODO:目前规则选择界面在主界面上不显示
      /*已经过时，如果使用需要修补代码
      for(int i = 0; i < m_vecrule.size(); ++i)
      {
          STRUCT_RULE rule = CLS_LoadRuleManager::getInstance()->GetRule()[m_vecrule[i]];
          QString scriptfilename = rule.m_scriptfilename;
          QJsonObject userdata;
          QJsonObject json;

          userdata["ruleid"] = rule.m_sid;

          ret = UpLoad(m_sshinfo, scriptfilename);
          if(ret == 0)
          {
              json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_RUNNING);
              emit SigEchoRes(m_id,json);

              ret = ExecScript(m_sshinfo, userdata, scriptfilename);
              json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_DONE);
              emit SigEchoRes(m_id,json);

              if(ret == 0)
              {
                  json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_OK);
                  emit SigEchoRes(m_id,json);
              }
              else
              {
                  json = GenerateJsonEchoRes(userdata,"执行脚本文件失败,请重试",EXEC_SCRIPT_STAT_FAIL);
                  json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_FAIL);
                  emit SigEchoRes(m_id,json);
              }
          }
          else
          {
              json = GenerateJsonEchoRes(userdata,"上传脚本文件失败,请重试",EXEC_SCRIPT_STAT_FAIL);
              emit SigEchoRes(m_id,json);
          }
      }
      */



      //执行规则组中的规则
      for(int i = 0; i < m_vecbatrule.size(); ++i)
      {
          STRUCT_BATRULE batrule = CLS_LoadRuleManager::getInstance()->GetBatRule()[m_vecbatrule[i]];
          QVector<QString> vecrulesid = batrule.GetMergeRules();

          int j = 0;
          int uploadnum = 0; //上传规则失败次数累计
          int execnum = 0;   //执行规则失败次数累计
          while(j < vecrulesid.size())
          {
              STRUCT_RULE rule = CLS_LoadRuleManager::getInstance()->GetRule()[vecrulesid[j]];
              QString scriptfilename = rule.m_scriptfilename;
              QJsonObject userdata;
              QJsonObject json;

              userdata["ruleid"] = rule.m_sid;
              userdata["batruleid"] = batrule.m_sid;

              if(!QFile::exists(const_LocalShFilePath + scriptfilename))
              {
                  json = GenerateJsonEchoRes(userdata,"对应脚本不存在",EXEC_SCRIPT_STAT_FAIL);
                  emit SigEchoRes(m_id,json);
                  ++j;
                  continue; //脚本不存在不需要再执行下面的代码
              }

              this->SetSftpLoacalFile(const_LocalShFilePath + scriptfilename);
              this->SetSftpRemotePath(const_ServerShFilePath + scriptfilename);
              if(UploadFile() == 0)
              {

                  //开始运行
                  json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_RUNNING);
                  emit SigEchoRes(m_id,json);

                  this->SetShellCommand(QString("sh ") + const_ServerShFilePath + scriptfilename);

                  //在成功执行完脚本后，会发送 EXEC_SCRIPT_STAT_DONE 状态信号，
                  //在执行失败后，会发送 EXEC_SCRIPT_STAT_FAIL 状态信号


                  //注意，此次设置这种重复尝试机制是为了规避libssh2库不稳定造成的功能错误(是开源库的问题还是调用函数的问题，犹未可知)
                  //失败再次执行,限制尝试次数为const_TryLimitNum次，如果超出const_TryLimitNum次则放弃尝试，并向外部发送失败信号
                  int ret = 0;
                  do
                  {
                      ret = ExecCommand(userdata);
                      if(ret == 0)
                      {//执行成功
                            json = GenerateJsonEchoRes(userdata,"",EXEC_SCRIPT_STAT_DONE);
                            emit SigEchoRes(m_id,json);
                      }
                      else
                      {
                            if(execnum < const_TryLimitNum )
                            {//执行失败继续尝试
                                ++execnum;
                                qDebug()<<"[脚本执行失败]  尝试次数 "<<QString::number(execnum);
                            }
                            else
                            {//超出尝试次数限制放弃尝试跳出循环
                                json = GenerateJsonEchoRes(userdata,"脚本执行失败，请检查重试",EXEC_SCRIPT_STAT_FAIL);
                                emit SigEchoRes(m_id,json);
                                break;
                            }
                      }

                  }while(ret != 0);

              }
              else
              {
                  //失败再次执行,限制尝试次数为const_TryLimitNum次，如果超出const_TryLimitNum次则放弃尝试，并向外部发送失败信号
                  if(uploadnum < const_TryLimitNum)
                  {
                      ++uploadnum;
                      qDebug()<<"[脚本上传失败]  尝试次数 "<<QString::number(uploadnum);
                      continue;
                  }
                  else
                  {
                      json = GenerateJsonEchoRes(userdata,"上传脚本文件失败，请检查重试",EXEC_SCRIPT_STAT_FAIL);
                      emit SigEchoRes(m_id,json);
                  }
              }

              ++j;  //执行成功或者超出尝试次数时 计数器+1 进行下一个规则的执行
              uploadnum = 0;  //累计值清空
              execnum = 0;
          }
      }

      //========================================================================================================================
      //删除存放shell脚本的目录
      this->SetShellCommand(QString("rm -r ") + const_ServerShFilePath);
      if(ExecCommand() != 0)
      {
          qDebug()<<"在远端服务器上删除目录 "<<const_ServerShFilePath<<" 失败";
          emit SigSshErrMsg("目标主机" + m_serverip + " SSH连接失败,请检查输入的ip、用户名、密码信息和网络");
          return;
      }
      else
      {
          qDebug()<<"在远端服务器上删除目录 "<<const_ServerShFilePath<<" 成功";
      }
}

void CLS_SshManager::InitSshFun()
{
    int ret;
    if(libssh2_init(0) != 0)
    {
        qDebug()<<"libssh2 initialization failed : "<<ret;
    }
}

void CLS_SshManager::SetServerIp(const QString &ip)
{
    m_serverip = ip;
}

void CLS_SshManager::SetServerPort(int port)
{
    m_port = port;
}

void CLS_SshManager::SetUserName(const QString & username)
{
    m_username = username;
}

void CLS_SshManager::SetPasswd(const QString &passwd)
{
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

//    if(libssh2_init(0) != 0)
//    {
//        qDebug()<<"libssh2 initialization failed : "<<ret;
//        return -1;
//    }

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
        QThread::msleep(const_iWaitTime);
        count++;
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

int CLS_SshManager::ExecCommand(QJsonObject _userdata)
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

    //执行命令
    while((ret = libssh2_channel_exec(m_pchannel, m_command.toStdString().data())) == LIBSSH2_ERROR_EAGAIN)
    {
        waitsocket(m_sock, m_psession);
    }
    if(ret != 0)
    {
        OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
        return -1;
    }


    //读取执行结果
    do
    {
          char buffer[102400] = {0};
          while((ret = libssh2_channel_read(m_pchannel, buffer, sizeof(buffer))) == LIBSSH2_ERROR_EAGAIN)
          {
              waitsocket(m_sock, m_psession);
          }

          //qDebug()<<__FUNCTION__<<" "<<__LINE__<<" ret = "<<ret<<" [执行结果] "<<buffer;

          if(ret < 0)
          {
              OutputErrReason(ret,__LINE__,__FUNCTION__,__FILE__);
              return -1;
          }
          else if(ret > 0)
          {
              if(!_userdata.isEmpty())
              {
                  QJsonObject json = GenerateJsonEchoRes(_userdata, buffer,EXEC_SCRIPT_STAT_GETRES);
                  emit SigEchoRes(m_id,json);
              }
          }

    }while(ret > 0);


    if(this->DisConnSsh() != 0)
    {
        qDebug()<<"与主机"<<m_serverip<<" 连接的断开失败";
        return -1;
    }

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


void CLS_SshManager::AddSelRule(QString _id)
{
    bool contain = false;
    for(int i = 0; i < m_vecrule.size(); ++i)
    {
        if(m_vecrule[i] == _id)
        {
            contain = true;
            break;
        }
    }
    if(contain == false)
    {
        m_vecrule.push_back(_id);
    }
}

void CLS_SshManager::AddSelBatRule(QString _id)
{
    bool contain = false;
    for(int i = 0; i < m_vecbatrule.size(); ++i)
    {
        if(m_vecbatrule[i] == _id)
        {
            contain = true;
            break;
        }
    }
    if(contain == false)
    {
        m_vecbatrule.push_back(_id);
    }
}

void CLS_SshManager::DelSelRule(QString _id)
{
    for(int i = 0; i < m_vecrule.size(); ++i)
    {
        if(m_vecrule[i] == _id)
        {
            m_vecrule.remove(i);
            break;
        }
    }
}

void CLS_SshManager::DelSelBatRule(QString _id)
{
    for(int i = 0; i < m_vecbatrule.size(); ++i)
    {
        if(m_vecbatrule[i] == _id)
        {
            m_vecbatrule.remove(i);
            break;
        }
    }
}

void CLS_SshManager::SetSshId(int _id)
{
    m_id = _id;
}

QVector<QString> CLS_SshManager::GetSelRule()
{
    return m_vecrule;
}

QVector<QString> CLS_SshManager::GetSelBatRule()
{
    return m_vecbatrule;
}


void CLS_SshManager::ClearSelRule()
{
    m_vecrule.clear();
}

void CLS_SshManager::ClearSelBatRule()
{
    m_vecbatrule.clear();
}

QJsonObject CLS_SshManager::GenerateJsonEchoRes(QJsonObject _json, QString _res, EXEC_SCRIPT_STAT _stat)
{
    QJsonObject qjson;

    qjson["userdata"] = _json;
    qjson["res"]  = _res;
    qjson["stat"] = _stat;

    return qjson;
}
