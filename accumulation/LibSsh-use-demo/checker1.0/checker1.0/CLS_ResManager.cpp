#include "CLS_ResManager.h"

std::mutex CLS_ResManager::m_mutex;
std::once_flag CLS_ResManager::flag_new;
CLS_ResManager * CLS_ResManager::pInstance = nullptr;
CLS_ResManager * CLS_ResManager::getInstance()
{
    std::call_once (flag_new,[&]()
    {
        if(!pInstance)
        {
           pInstance = new CLS_ResManager();
        }
    });

    return pInstance;
}

void CLS_ResManager::Release()
{
    std::lock_guard<std::mutex> lock_(m_mutex);
    if(pInstance)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

void CLS_ResManager::OutputMsg()
{
    auto iterbasemsg = m_mapSshBasicMsg.begin();
    while(iterbasemsg != m_mapSshBasicMsg.end())
    {
        qDebug()<<"SSH ID     = "<<iterbasemsg.key();
        qDebug()<<"USER       = "<<iterbasemsg.value().m_qstrUser;
        qDebug()<<"IP         = "<<iterbasemsg.value().m_qstrTargetIp;
        qDebug()<<"START TIME = "<<iterbasemsg.value().m_qstrStartTime;
        qDebug()<<"END TIME   = "<<iterbasemsg.value().m_qstrEndTime;
        qDebug()<<"TAKE TIME  = "<<iterbasemsg.value().m_qstrTakeTime;
//        qDebug()<<"ERR NUM    = "<<iterbasemsg.value().m_iErrNum;
        qDebug()<<"FATAL NUM  = "<<iterbasemsg.value().m_iFatalNum;
        qDebug()<<"SER NUM    = "<<iterbasemsg.value().m_iSeriousNum;
        qDebug()<<"SLI NUM    = "<<iterbasemsg.value().m_iSlightNum;
        qDebug()<<"NOR NUM    = "<<iterbasemsg.value().m_iNormalNum;

        QVector<QString> vecbatruleid = m_mapSshBatRule[iterbasemsg.key()];
        for(int i = 0; i < vecbatruleid.size(); ++i)
        {
           QString batruleid = vecbatruleid[i];
           STRUCT_BATRULE strtbatrule = CLS_LoadRuleManager::getInstance()->GetBatRule()[batruleid];
           qDebug()<<"BAT RULE NAME = "<<strtbatrule.m_name<<" BAT RULE ID = "<< batruleid;
           QVector<QString> vecrulesid = strtbatrule.GetMergeRules();

           for(int j = 0; j < vecrulesid.size(); ++j)
           {
               QString ruleid = vecrulesid[j];
               STRUCT_RULE strrule = CLS_LoadRuleManager::getInstance()->GetRule()[ruleid];
               qDebug()<<"             RULE NAME = "<<strrule.m_name<<" RULE ID = "<< ruleid;
               qDebug()<<"             RULE RES = "<< m_mapRuleRes[batruleid+ruleid];
           }
        }
        iterbasemsg++;
    }
}

void CLS_ResManager::Clear()
{
    m_mapSshBasicMsg.clear();
    m_mapSshBatRule.clear();
    m_mapRuleRes.clear();
}

int CLS_ResManager::GetSshIdFromSerIp(QString _serip)
{
     if(!m_mapSshBasicMsg.isEmpty())
     {
         auto iter = m_mapSshBasicMsg.begin();
         while(iter != m_mapSshBasicMsg.end())
         {
             if(iter.value().m_qstrTargetIp == _serip)
             {
                 return iter.key();
             }
             ++iter;
         }
     }
     return -1;
}

QStringList CLS_ResManager::GetBatRuleNameFromSshId(int _sshid)
{
     QStringList qlistRet;
     if(!m_mapSshBatRule.isEmpty() && m_mapSshBatRule.contains(_sshid))
     {
         QVector<QString> vecbatruleId = m_mapSshBatRule[_sshid];
         for(int i = 0; i < vecbatruleId.size(); ++i)
         {
             STRUCT_BATRULE strbatrule = (CLS_LoadRuleManager::getInstance()->GetBatRule())[vecbatruleId[i]];
             qlistRet.push_back(strbatrule.m_name);
         }
     }
     return qlistRet;
}
