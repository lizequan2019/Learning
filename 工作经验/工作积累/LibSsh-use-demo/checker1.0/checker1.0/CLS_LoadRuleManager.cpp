#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_LoadRuleManager.h"

std::mutex CLS_LoadRuleManager::m_mutex;
std::once_flag CLS_LoadRuleManager::flag_new;
CLS_LoadRuleManager * CLS_LoadRuleManager::pInstance = nullptr;
CLS_LoadRuleManager * CLS_LoadRuleManager::getInstance()
{
    std::call_once (flag_new,[&]()
    {
        if(!pInstance)
        {
           pInstance = new CLS_LoadRuleManager();
        }
    });

    return pInstance;
}

void CLS_LoadRuleManager::Release()
{
    std::lock_guard<std::mutex> lock_(m_mutex);
    if(pInstance)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

QMap<QString,STRUCT_RULE> CLS_LoadRuleManager::GetRule()
{
    return m_mapRule;
}

QMap<QString,STRUCT_BATRULE> CLS_LoadRuleManager::GetBatRule()
{
     return m_mapBatRule;
}

void CLS_LoadRuleManager::JsonTransToStrRule(QJsonObject _json, QMap<QString, STRUCT_BATRULE> & _mapbatrule, QMap<QString, STRUCT_RULE> & _maprule)
{
    //获取规则组json数组
    QJsonArray jsonAryBatRule  = _json["content"].toArray();
    for(int i = 0; i < jsonAryBatRule.size(); ++i)
    {//遍历每一个规则组

        QJsonObject jsonObjBatRule = jsonAryBatRule[i].toObject();
        STRUCT_BATRULE strBatRule;

        //读取一个规则组信息
        strBatRule.m_sid   = jsonObjBatRule["id"].toString();
        strBatRule.m_name  = jsonObjBatRule["name"].toString();
        strBatRule.m_isDel = jsonObjBatRule["delFlg"].toInt();
        strBatRule.m_createTime  = jsonObjBatRule["createTime"].toString();
        strBatRule.m_createUser  = jsonObjBatRule["createUser"].toString();
        strBatRule.m_description = jsonObjBatRule["description"].toString();
        strBatRule.m_updateTime  = jsonObjBatRule["updateTime"].toString();
        strBatRule.m_updateUser  = jsonObjBatRule["updateUser"].toString();
        strBatRule.m_version     = jsonObjBatRule["version"].toString();

        QJsonArray jsonAryRule;
        if(jsonObjBatRule["ruleVOS"].isArray())
        {
            jsonAryRule = jsonObjBatRule["ruleVOS"].toArray();
            for(int j = 0; j < jsonAryRule.size(); ++j)
            {//遍历规则组中每个规则

                QJsonObject jsonObjRule = jsonAryRule[j].toObject();
                STRUCT_RULE strRule;

                //读取一个规则信息
                strRule.m_sid   = jsonObjRule["id"].toString();
                strRule.m_name  = jsonObjRule["name"].toString();
                strRule.m_url   = jsonObjRule["scriptUrl"].toString();
                strRule.m_isDel = jsonObjRule["delFlg"].toInt();
                strRule.m_version     = jsonObjRule["version"].toString();
                strRule.m_description = jsonObjRule["description"].toString();
                strRule.m_updateUser  = jsonObjRule["updateUser"].toString();
                strRule.m_updateTime  = jsonObjRule["updateTime"].toString();
                strRule.m_createTime  = jsonObjRule["createTime"].toString();
                strRule.m_createUser  = jsonObjRule["createUser"].toString();
                strRule.m_md5         = jsonObjRule["scriptMd5"].toString();
                strRule.m_scriptfilename = strRule.m_md5 + "-" + GetShFileName(strRule.m_url);

                //向 _maprule 中插入不存在并且是没有被弃用的规则
                if(!_maprule.contains(strRule.m_sid) && (strRule.m_isDel != const_Deleted))
                {
                    _maprule.insert(strRule.m_sid, strRule);
                }

                strBatRule.m_containrulesid.push_back(strRule.m_sid); //保存规则组中的规则id
            }
        }


        if(jsonObjBatRule["parentRuleVOS"].isArray())
        {
            jsonAryRule  =  jsonObjBatRule["parentRuleVOS"].toArray();
            for(int j = 0; j < jsonAryRule.size(); ++j)
            {//遍历规则组中每个规则

                QJsonObject jsonObjRule = jsonAryRule[j].toObject();
                STRUCT_RULE strRule;

                //读取一个规则信息
                strRule.m_sid   = jsonObjRule["id"].toString();
                strRule.m_name  = jsonObjRule["name"].toString();
                strRule.m_url   = jsonObjRule["scriptUrl"].toString();
                strRule.m_isDel = jsonObjRule["delFlg"].toInt();
                strRule.m_version     = jsonObjRule["version"].toString();
                strRule.m_description = jsonObjRule["description"].toString();
                strRule.m_updateUser  = jsonObjRule["updateUser"].toString();
                strRule.m_updateTime  = jsonObjRule["updateTime"].toString();
                strRule.m_createTime  = jsonObjRule["createTime"].toString();
                strRule.m_createUser  = jsonObjRule["createUser"].toString();
                strRule.m_md5         = jsonObjRule["scriptMd5"].toString();
                strRule.m_scriptfilename = strRule.m_md5 + "-" + GetShFileName(strRule.m_url);

                //向 _maprule 中插入不存在并且是没有被弃用的规则
                if(!_maprule.contains(strRule.m_sid) && (strRule.m_isDel != const_Deleted))
                {
                    _maprule.insert(strRule.m_sid, strRule);
                }

                strBatRule.m_inheritrulesid.push_back(strRule.m_sid); //保存规则组继承其他规则组规则的id
            }
        }

        //向 _mapbatrule 中插入不存在并且是没有被弃用的规则组
        if(!_mapbatrule.contains(strBatRule.m_sid) && (strBatRule.m_isDel != const_Deleted))
        {
            _mapbatrule.insert(strBatRule.m_sid, strBatRule);
        }
    }
}



QJsonObject CLS_LoadRuleManager::StrRuleTransToJson(QMap<QString, STRUCT_BATRULE> _mapbatrule, QMap<QString, STRUCT_RULE> _maprule)
{
    QJsonObject jsonObj;  //最终要输出的json对象

    QJsonArray jsonArybatrule; //第一层的规则组数组对象

    auto iterbatrule = _mapbatrule.begin();
    while(iterbatrule != _mapbatrule.end()) //遍历_mapbatrule元素
    {
        QJsonObject jsonObjbatrule;
        jsonObjbatrule["id"]   = iterbatrule.value().m_sid;
        jsonObjbatrule["name"] = iterbatrule.value().m_name;
        jsonObjbatrule["createTime"]  = iterbatrule.value().m_createTime;
        jsonObjbatrule["createUser"]  = iterbatrule.value().m_createUser;
        jsonObjbatrule["description"] = iterbatrule.value().m_description;
        jsonObjbatrule["updateTime"]  = iterbatrule.value().m_updateTime;
        jsonObjbatrule["updateUser"]  = iterbatrule.value().m_updateUser;
        jsonObjbatrule["version"] = iterbatrule.value().m_version;
        jsonObjbatrule["delFlg"]  = iterbatrule.value().m_isDel;

        QJsonArray jsonAryrule;
        for(int i = 0; i < iterbatrule.value().m_containrulesid.size(); ++i) //遍历 m_containrules 获取规则组对应的规则数据
        {
            QJsonObject jsonObjrule;
            STRUCT_RULE rule = m_mapRule[iterbatrule.value().m_containrulesid[i]];
            jsonObjrule["id"]   = rule.m_sid;
            jsonObjrule["name"] = rule.m_name;
            jsonObjrule["scriptUrl"]   = rule.m_url;
            jsonObjrule["createTime"]  = rule.m_createTime;
            jsonObjrule["createUser"]  = rule.m_createUser;
            jsonObjrule["description"] = rule.m_description;
            jsonObjrule["updateTime"]  = rule.m_updateTime;
            jsonObjrule["updateUser"]  = rule.m_updateUser;
            jsonObjrule["version"]     = rule.m_version;
            jsonObjrule["delFlg"]      = rule.m_isDel;
            jsonObjrule["scriptMd5"]   = rule.m_md5;

            jsonAryrule.append(jsonObjrule);
        }
        jsonObjbatrule["ruleVOS"] = jsonAryrule;



        jsonAryrule = QJsonArray();
        for(int i = 0; i < iterbatrule.value().m_inheritrulesid.size(); ++i) //遍历 m_inheritrule 获取规则组继承其他规则组规则
        {
            QJsonObject jsonObjrule;
            STRUCT_RULE rule = m_mapRule[iterbatrule.value().m_inheritrulesid[i]];
            jsonObjrule["id"]   = rule.m_sid;
            jsonObjrule["name"] = rule.m_name;
            jsonObjrule["scriptUrl"]   = rule.m_url;
            jsonObjrule["createTime"]  = rule.m_createTime;
            jsonObjrule["createUser"]  = rule.m_createUser;
            jsonObjrule["description"] = rule.m_description;
            jsonObjrule["updateTime"]  = rule.m_updateTime;
            jsonObjrule["updateUser"]  = rule.m_updateUser;
            jsonObjrule["version"]     = rule.m_version;
            jsonObjrule["scriptMd5"]   = rule.m_md5;

            jsonAryrule.append(jsonObjrule);
        }
        jsonObjbatrule["parentRuleVOS"] = jsonAryrule;


        jsonArybatrule.append(jsonObjbatrule);
        ++iterbatrule;
    }

    //加一个更新时间字段
    jsonObj["timestamp"] = m_updateTime;

    jsonObj["content"] = jsonArybatrule;
    return jsonObj;
}

void CLS_LoadRuleManager::UpdateRule(QJsonObject & _json)
{
    QMap<QString,STRUCT_BATRULE> mapBatRule;  //这个不需要使用
    QMap<QString,STRUCT_RULE> mapRule;

    //将json解析到mapRule，然后向m_mapRule中合并
    JsonTransToStrRule(_json,mapBatRule,mapRule);

    auto iterrule = mapRule.begin();
    while(iterrule != mapRule.end())
    {
        if(iterrule.value().m_isDel == const_Deleted)
        {//如果规则是已弃用的,并且当前缓存的规则集合中包含此规则，那么就删除掉数据
            if(m_mapRule.contains(iterrule.key()))
            {
               m_mapRule.remove(iterrule.key());
            }
        }
        else
        {//修改或者更新规则

            //如果已经有key相同的，那么直接覆盖，这属于更新数据
            //如果没有key相同的，那么就加上，属于新增数据

            //下载脚本文件
            //http://10.30.23.231:9689/checker/file/f9d86955-f615-4649-840a-2f75eef7a6ad/aaa.sh 脚本文件下载路径样例
            QString qstrurl = QString("http://%1:%2/checker%3").arg(m_qstrSerIp).arg(m_qstrSerPort).arg(iterrule.value().m_url); //拼接下载路径

            QString scriptfilename = iterrule.value().m_md5 +"-"+ GetShFileName(qstrurl); //注意这里下载下来的文件名要加md5码防止有文件重名
            iterrule.value().m_scriptfilename = scriptfilename;
            QString qstrlocalfile = const_LocalShFilePath + scriptfilename;

            m_downloadMgr.SetDownloadCfg(qstrurl, qstrlocalfile);

            //存储规则信息
            m_mapRule[iterrule.key()] = iterrule.value();
        }
        ++iterrule;
    }
}

void CLS_LoadRuleManager::UpdateBatRule(QJsonObject & _json)
{
    QMap<QString,STRUCT_BATRULE> mapBatRule;
    QMap<QString,STRUCT_RULE> mapRule;    //这个不需要使用

    JsonTransToStrRule(_json,mapBatRule,mapRule);

    auto iterbatrule = mapBatRule.begin();

    while(iterbatrule != mapBatRule.end())
    {
        if(iterbatrule.value().m_isDel == const_Deleted)
        {//删除规则组
            if(m_mapBatRule.contains(iterbatrule.key()))
            {
               m_mapBatRule.remove(iterbatrule.key());
            }
        }
        else
        {//修改或者增加规则组
            m_mapBatRule[iterbatrule.key()] = iterbatrule.value();
        }

        ++iterbatrule;
    }
}

void CLS_LoadRuleManager::SetUpdateTime(QString _qstrTime)
{
    m_updateTime = _qstrTime;
}

QString CLS_LoadRuleManager::GetUpdateTime()
{
    return m_updateTime;
}


void CLS_LoadRuleManager::LoadLoaclRules()
{
    QFile file;
    file.setFileName(const_CacheFilePath);

    if(file.exists())
    {//如果文件存在
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonDocument qjsonDoc = QJsonDocument::fromJson(file.readAll());
            if(qjsonDoc.isObject())
            {
               QJsonObject qjson = qjsonDoc.object();
               if(qjson.contains("timestamp"))
               {
                   SetUpdateTime(qjson["timestamp"].toString());
               }
               JsonTransToStrRule(qjsonDoc.object(),m_mapBatRule,m_mapRule);
            }
            file.close();
        }
    }
}

void CLS_LoadRuleManager::SaveRuleToLoacl()
{
    QJsonObject jsonObj =  StrRuleTransToJson(m_mapBatRule , m_mapRule);

    QFile file;
    file.setFileName(const_CacheFilePath);

    if(file.open(QIODevice::WriteOnly))
    {
        QJsonDocument qjsonDoc;
        qjsonDoc.setObject(jsonObj);
        QString qstrcontent = qjsonDoc.toJson();
        file.write(qstrcontent.toStdString().data());
        file.close();
    }
    else
    {
        QMessageBox::warning(nullptr,"警告","将数据保存到本地文件失败!");
    }
}

void CLS_LoadRuleManager::SetSerIp(QString _qstrip)
{
    m_qstrSerIp = _qstrip;
}

void CLS_LoadRuleManager::SetSerPort(QString _qstrport)
{
    m_qstrSerPort = _qstrport;
}

void CLS_LoadRuleManager::ClearProgramCache()
{
    m_mapRule.clear();
    m_mapBatRule.clear();
    m_updateTime.clear();
    m_qstrSerIp.clear();
    m_qstrSerPort.clear();
}



