#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_WgtSystemSetting.h"
#include "ui_CLS_WgtSystemSetting.h"


CLS_WgtSystemSetting::CLS_WgtSystemSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLS_WgtSystemSetting)
{
    ui->setupUi(this);

    m_pTreeWgt = new QTreeWidget(ui->wgt_viewrule);

    //树状控件 设置为3列 分别为名称  版本 描述
    m_pTreeWgt->setColumnCount(3);
    m_pTreeWgt->setHeaderLabels(QStringList()<<"名称"<<"版本"<<"描述");
    m_pTreeWgt->header()->setDefaultAlignment(Qt::AlignCenter);
    m_pTreeWgt->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_pTreeWgt->setAlternatingRowColors(true); //交替背景颜色
    m_pTreeWgt->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);


    SetTreeMinSize();

    //读取并设置样式表
    QFile file("./style-system.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        QString qstrStyle = file.readAll();
        this->setStyleSheet(qstrStyle);
        file.close();
    }
}

CLS_WgtSystemSetting::~CLS_WgtSystemSetting()
{
    delete ui;
}

void CLS_WgtSystemSetting::SetTreeMinSize()
{
    if(m_pTreeWgt == nullptr)
    {
        return;
    }

    ui->wgt_viewrule->setMinimumSize(const_SysTemTreeMinWidthSize,const_SysTemTreeMinHighSize);
    m_pTreeWgt->setMinimumSize(const_SysTemTreeMinWidthSize,const_SysTemTreeMinHighSize);

    ui->wgt_viewrule->setMaximumSize(const_SysTemTreeMinWidthSize,const_SysTemTreeMinHighSize);
    m_pTreeWgt->setMaximumSize(const_SysTemTreeMinWidthSize,const_SysTemTreeMinHighSize);
}

void CLS_WgtSystemSetting::SetTreeMaxSize()
{
    if(m_pTreeWgt == nullptr)
    {
        return;
    }

    ui->wgt_viewrule->setMinimumSize(const_SysTemTreeMaxWidthSize,const_SysTemTreeMaxHighSize);
    m_pTreeWgt->setMinimumSize(const_SysTemTreeMaxWidthSize,const_SysTemTreeMaxHighSize);

    ui->wgt_viewrule->setMaximumSize(const_SysTemTreeMaxWidthSize,const_SysTemTreeMaxHighSize);
    m_pTreeWgt->setMaximumSize(const_SysTemTreeMaxWidthSize,const_SysTemTreeMaxHighSize);
}

QString CLS_WgtSystemSetting::GetOperatorName()
{
    return ui->edt_username->text();
}

void CLS_WgtSystemSetting::SaveUserInfo()
{
    if(ui->edt_username->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","用户登录界面-用户名不能为空");
        return;
    }

    if(ui->edt_passwd->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","用户登录界面-密码不能为空");
        return;
    }

    if(ui->edt_serverport->text().isEmpty())
    {
        QMessageBox::warning(this,"提示","用户登录界面-服务器端口不能为空");
        return;
    }

    int checkres = CheckIpFormat(ui->edt_serverip->text());
    if(checkres == CHECK_IP_RES_EMPTY)
    {
        QMessageBox::warning(this,"提示","用户登录界面-服务器Ip不能为空");
        return;
    }
    else if(checkres == CHECK_IP_RES_ERR)
    {
        QMessageBox::information(this, "提示", "用户登录界面-服务器Ip格式错误");
        return;
    }


    STRUCT_USER userinfo;
    bool blReset = false;
    if(userinfo.m_name != ui->edt_username->text())
    {
        userinfo.m_name = ui->edt_username->text();
        blReset = true;
    }

    if(userinfo.m_passwd != ui->edt_passwd->text())
    {
        userinfo.m_passwd = ui->edt_passwd->text();
        blReset = true;
    }

    if(userinfo.m_serverIp != ui->edt_serverip->text())
    {
        userinfo.m_serverIp = ui->edt_serverip->text();
        blReset = true;
    }

    if(userinfo.m_port != ui->edt_serverport->text())
    {
        userinfo.m_port = ui->edt_serverport->text();
        blReset = true;
    }

    if(blReset)
    {
        RemoveRuleCfgJson();
        RemovePdfDir();
        RemoveShellDir();
        CLS_LoadRuleManager::getInstance()->ClearProgramCache();
    }


    QVariant qvar;
    qvar.setValue(userinfo);
    emit SigSaveUserInfo(qvar);
}

void CLS_WgtSystemSetting::SlotViewRule()
{
    if(m_pTreeWgt == nullptr)
    {
        return;
    }

    m_pTreeWgt->clear();

    QMap<QString,STRUCT_BATRULE> qmapBatRule = CLS_LoadRuleManager::getInstance()->GetBatRule();
    auto iter = qmapBatRule.begin();

    //显示基本信息
    while(iter != qmapBatRule.end())
    {
        STRUCT_BATRULE strbatrule =  iter.value();
        if(strbatrule.m_isDel == true)
        {
            continue;
        }

        QTreeWidgetItem * pItemOne = new QTreeWidgetItem(m_pTreeWgt);
        pItemOne->setText(0,strbatrule.m_name);
        pItemOne->setText(1,strbatrule.m_version);
        pItemOne->setText(2,strbatrule.m_description);

        pItemOne->setToolTip(0,strbatrule.m_name);
        pItemOne->setToolTip(1,strbatrule.m_version);
        pItemOne->setToolTip(2,strbatrule.m_description);

        QVector<QString>  vecrulesid = strbatrule.GetMergeRules();
        for(int j = 0; j < vecrulesid.size(); ++j)
        {//二级节点  显示规则信息

            STRUCT_RULE strrule = CLS_LoadRuleManager::getInstance()->GetRule()[vecrulesid[j]];

            if(strrule.m_isDel == true)
            {
                continue;
            }

            QTreeWidgetItem * pItemtwo = new QTreeWidgetItem(pItemOne);
            pItemtwo->setText(0,strrule.m_name);
            pItemtwo->setText(1,strrule.m_version);
            pItemtwo->setText(2,strrule.m_description);

            pItemtwo->setToolTip(0,strrule.m_name);
            pItemtwo->setToolTip(1,strrule.m_version);
            pItemtwo->setToolTip(2,strrule.m_description);
        }

        ++iter;
    }

    //处理更新时间
    ui->lab_updateTime->setText("规则更新时间 : "+  TimeStampToDate(CLS_LoadRuleManager::getInstance()->GetUpdateTime()));
}

void CLS_WgtSystemSetting::SlotUpdateErr()
{
    QMessageBox::warning(this,"警告","访问服务器失败，请检查用户登录信息、网络或联系服务器管理员");
}


void CLS_WgtSystemSetting::on_btn_allupdate_clicked()
{
    SaveUserInfo();
    emit SigAllUpdate();
}


void CLS_WgtSystemSetting::on_btn_addupdate_clicked()
{
    SaveUserInfo();
    emit SigAddUpdate();
}

