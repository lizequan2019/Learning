#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_RuleUpdateManager.h"

std::mutex CLS_RuleUpdateManager::m_mutex;
std::once_flag CLS_RuleUpdateManager::flag_new;
CLS_RuleUpdateManager * CLS_RuleUpdateManager::pInstance = nullptr;
CLS_RuleUpdateManager * CLS_RuleUpdateManager::getInstance()
{
    std::call_once (flag_new,[&]()
    {
        if(!pInstance)
        {
           pInstance = new CLS_RuleUpdateManager();
           pInstance->setAutoDelete(false);
        }
    });
    return pInstance;
}

void CLS_RuleUpdateManager::Release()
{
    std::lock_guard<std::mutex> lock_(m_mutex);
    if(pInstance)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

void CLS_RuleUpdateManager::run()
{
   if(m_opType == OP_TYPE_UPDATE_ALL || m_opType == OP_TYPE_UPDATE_ADD)
   {//访问服务器数据库进行全量更新或者增量更新
       GetToken();
       UpdateRule();
       emit SigUpdateOver();
   }
}

void CLS_RuleUpdateManager::GetToken()
{
    //拼接url
    QString qStrUrl = QString("http://%1:%2/checker/rulemanage/user/login")
                      .arg(m_userinfo.m_serverIp)
                      .arg(m_userinfo.m_port);

    //拼接报文内容
    QJsonObject jsonContent;
    QJsonObject jsonRet;
    jsonContent["name"] = m_userinfo.m_name;
    jsonContent["password"] = m_userinfo.m_passwd;

    //发送报文
    if(CLS_WebServiceManager::getInstance()->SendPost(qStrUrl,jsonContent,jsonRet) == ACCESS_WEBSER_RES_OK)
    {
          if(!jsonRet.isEmpty())
          {
              if(jsonRet["statusCode"].toString() == const_ServerRetCodeSuccess)
              {//判断返回值正确，存储获得的凭证
                  m_token = jsonRet["content"].toString();
              }
              else
              {//如果返回值不成功，则需要清空 m_token ，表示此次获取token失败
                  m_token.clear();
              }
          }
    }
}

void CLS_RuleUpdateManager::UpdateRule()
{
    //拼接url
    QString qStrUrl = QString("http://%1:%2/checker/rulemanage/ruleGroup/search-for-client")
                      .arg(m_userinfo.m_serverIp)
                      .arg(m_userinfo.m_port);


    //拼接报文内容
    QJsonObject jsonContent;
    QJsonObject jsonRet;

    //全量更新都是不需要传递updateTime的
    //增量更新则需要判断有没有规则缓存文件，有缓存文件(说明以前有过更新)则需要传递updateTime，没有(说明从未进行更新)则不传递updateTime
    if(m_opType == OP_TYPE_UPDATE_ADD)
    {
        jsonContent["updateTime"] = CLS_LoadRuleManager::getInstance()->GetUpdateTime();
    }
    else
    {
        jsonContent["updateTime"] = "";
    }

    //发送报文
    if(CLS_WebServiceManager::getInstance()->SendPost(qStrUrl,jsonContent,jsonRet,m_token) == ACCESS_WEBSER_RES_OK)
    {
          qDebug()<<"[数据服务器返回更新数据]:"<<jsonRet;

          if(!jsonRet.isEmpty())
          {
              if(jsonRet["statusCode"].toString() == const_ServerRetCodeSuccess)
              {

                CLS_LoadRuleManager::getInstance()->SetSerIp(m_userinfo.m_serverIp);
                CLS_LoadRuleManager::getInstance()->SetSerPort(m_userinfo.m_port);

                //更新规则
                CLS_LoadRuleManager::getInstance()->UpdateRule(jsonRet);

                //更新规则组
                CLS_LoadRuleManager::getInstance()->UpdateBatRule(jsonRet);

                if(jsonRet.contains("timestamp"))
                {//记录下更新时间(此时间是基于数据服务器系统时间)

                    QString qstrtime = jsonRet["timestamp"].toString();
                    CLS_LoadRuleManager::getInstance()->SetUpdateTime(qstrtime);
                }
                else
                {
                    qDebug()<<"[ERROR] 没有数据更新时间信息";
                }

                //更新本地缓存文件
                CLS_LoadRuleManager::getInstance()->SaveRuleToLoacl();
              }
          }
    }
    else
    {
        emit SigUpdateErr();
    }
}

void CLS_RuleUpdateManager::SetUserInfo(STRUCT_USER & _userinfo)
{
    m_userinfo = _userinfo;
}

void CLS_RuleUpdateManager::SetOperaType(OP_TYPE _type)
{
    m_opType =  _type;
}
