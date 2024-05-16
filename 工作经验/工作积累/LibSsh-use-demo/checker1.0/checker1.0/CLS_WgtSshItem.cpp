#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_WgtSshItem.h"

//输入框默认显示内容
const QString CONST_LINE_EDIT_QSTR[6] =
{
    "服务器IP",
    "服务器端口",
    "用户名",
    "密码",
    "规则",
    "规则组"
};


CLS_WgtSshItem::CLS_WgtSshItem(int _iId, QWidget *parent) : QWidget(parent)
{

    m_iId = _iId;

    //Ssh管理类的创建
    m_sshManager = new CLS_SshManager(this);
    connect(m_sshManager,SIGNAL(SigEchoRes(int,QJsonObject)),this,SIGNAL(SigEchoRes(int,QJsonObject)),Qt::QueuedConnection);
    connect(m_sshManager,SIGNAL(SigSshErrMsg(QString)),this,SIGNAL(SigSshErrMsg(QString)),Qt::QueuedConnection);
    //创建水平布局
    QHBoxLayout *pHMain = new QHBoxLayout;
    pHMain->setContentsMargins(0,0,0,0);

    //创建勾选框控件默认不选择
    m_pCheckBox = new QCheckBox(this);
    m_pCheckBox->setChecked(false);
    connect(m_pCheckBox,SIGNAL(stateChanged(int)),this,SLOT(SlotStateChange(int)));

    //向水平布局中加入勾选框控件
    pHMain->addWidget(m_pCheckBox);

    //创建 ip 端口 用户名 密码的输入框控件 并加入到水平布局中
    for(int i = 0; i < ENUM_TABLE_COL_PWD + 1; i++)
    {
        QLineEdit *pEdit = nullptr;
        pEdit = new QLineEdit(this);
        if(i == ENUM_TABLE_COL_PWD)  //todo 远程服务器密码设置
        {
            pEdit->setEchoMode(QLineEdit::Password);
        }
        pEdit->setObjectName(QString::number(i));
        pEdit->setPlaceholderText(CONST_LINE_EDIT_QSTR[i]); //设置输入框中提示字符
        m_pLineEdit[i] = pEdit;
        pHMain->addWidget(pEdit);
    }

    //创建规则组和规则选择下拉框控件
    m_pComboBoxRule    = new CLS_MultiSelectComboBox(this);
    m_pComboBoxRule->SetShowDataType(SHOW_DATA_TYPE_RULE);
    m_pComboBoxRule->setObjectName(QString::number(ENUM_TABLE_COL_RULE));
    m_pComboBoxRule->SetPlaceHolderText(CONST_LINE_EDIT_QSTR[4]);
    m_pComboBoxRule->hide();

    m_pComboBoxBatRule = new CLS_MultiSelectComboBox(this);
    m_pComboBoxBatRule->SetShowDataType(SHOW_DATA_TYPE_BATRULE);
    m_pComboBoxBatRule->setObjectName(QString::number(ENUM_TABLE_COL_BATRULE));
    m_pComboBoxBatRule->SetPlaceHolderText(CONST_LINE_EDIT_QSTR[5]);

    connect(m_pComboBoxRule,SIGNAL(SigSelChange(SELECT_TYPE,QString)),this,SLOT(SlotSelChange(SELECT_TYPE,QString)));
    connect(m_pComboBoxBatRule,SIGNAL(SigSelChange(SELECT_TYPE,QString)),this,SLOT(SlotSelChange(SELECT_TYPE,QString)));

    connect(m_pComboBoxRule,SIGNAL(SigClearSelRule()),this,SLOT(SlotClearSelRule()));
    connect(m_pComboBoxBatRule,SIGNAL(SigClearSelRule()),this,SLOT(SlotClearSelRule()));

    //规则组和规则选择下拉框控件加入到水平布局
    pHMain->addWidget(m_pComboBoxRule);
    pHMain->addWidget(m_pComboBoxBatRule);


    //删除按钮创建加入到水平布局
    m_pBtnDel = new QPushButton(this);
    connect(m_pBtnDel, SIGNAL(clicked()), this, SLOT(SlotClickDelBtn()));
    m_pBtnDel->setObjectName("btndel");
    pHMain->addWidget(m_pBtnDel);

    //增加按钮创建 (这个按钮需要隐藏) 加入到水平布局
    m_pBtnAdd = new QPushButton(this);
    connect(m_pBtnAdd, SIGNAL(clicked()), this, SIGNAL(SignAddItem()));
    m_pBtnAdd->setObjectName("btnadd");
    pHMain->addWidget(m_pBtnAdd);


    //增加弹簧
    pHMain->addStretch();

    pHMain->setSpacing(9);

    //设置界面到水平区域
    this->setLayout(pHMain);
}

void CLS_WgtSshItem::SetSsh(STRUCT_SSH _info)
{
    if( m_pLineEdit[ENUM_TABLE_COL_IP])
    {
        m_pLineEdit[ENUM_TABLE_COL_IP]->setText(_info.m_qstrIp);
    }
    if( m_pLineEdit[ENUM_TABLE_COL_PORT])
    {
        m_pLineEdit[ENUM_TABLE_COL_PORT]->setText(QString("%1").arg(_info.m_iPort));
    }
    if( m_pLineEdit[ENUM_TABLE_COL_USERNAME])
    {
        m_pLineEdit[ENUM_TABLE_COL_USERNAME]->setText(_info.m_qstrUsername);
    }
    if( m_pLineEdit[ENUM_TABLE_COL_PWD])
    {
        m_pLineEdit[ENUM_TABLE_COL_PWD]->setText(_info.m_qstrPassword);
    }
}

STRUCT_SSH CLS_WgtSshItem::GetSsh()
{
    STRUCT_SSH info;
    for(int i = 0; i < ENUM_TABLE_COL_PWD + 1; i++)
    {
        if(!m_pLineEdit[i])
        {
            return info;
        }
    }

    if(m_pLineEdit[ENUM_TABLE_COL_IP])
    {
        info.m_qstrIp = m_pLineEdit[ENUM_TABLE_COL_IP]->text();
    }

    if(m_pLineEdit[ENUM_TABLE_COL_PORT])
    {
        info.m_iPort =  m_pLineEdit[ENUM_TABLE_COL_PORT]->text().toInt();
    }

    if(m_pLineEdit[ENUM_TABLE_COL_USERNAME])
    {
        info.m_qstrUsername = m_pLineEdit[ENUM_TABLE_COL_USERNAME]->text();
    }

    if(m_pLineEdit[ENUM_TABLE_COL_PWD])
    {
        info.m_qstrPassword = m_pLineEdit[ENUM_TABLE_COL_PWD]->text();
    }

    return info;
}


void CLS_WgtSshItem::SetId(int _id)
{
    m_iId = _id;
}

int CLS_WgtSshItem::GetId()
{
    return m_iId;
}


bool CLS_WgtSshItem::IsSelect()
{
    if(m_pCheckBox == nullptr)
    {
        return false;
    }
    return m_pCheckBox->isChecked();
}


void CLS_WgtSshItem::SetSelect()
{
    if(m_pCheckBox == nullptr)
    {
        return;
    }
    m_pCheckBox->setChecked(true);
}

QCheckBox *CLS_WgtSshItem::GetCheckBoxPoint()
{
    return m_pCheckBox;
}

QPushButton *CLS_WgtSshItem::GetAddBtnPoint()
{
    return m_pBtnAdd;
}

QPushButton *CLS_WgtSshItem::GetDelBtnPoint()
{
    return m_pBtnDel;
}

void CLS_WgtSshItem::SlotClickDelBtn()
{
    emit SignDelItem(this->m_iId);
}

void CLS_WgtSshItem::SlotSelChange(SELECT_TYPE _seltype, QString _id)
{//TODO这里如果用模板 代码量会小一些

    //获取发送信息的控件
    QObject * obj = sender();
    CLS_MultiSelectComboBox * psender = dynamic_cast<CLS_MultiSelectComboBox *>(obj);

    if(psender == nullptr || m_sshManager == nullptr)
    {
        return;
    }


    //获取程序内存中的规则和规则组数据
    QMap<QString,STRUCT_RULE> maprule = CLS_LoadRuleManager::getInstance()->GetRule();
    QMap<QString,STRUCT_BATRULE> mapbatrule = CLS_LoadRuleManager::getInstance()->GetBatRule();

    if(psender->GetShowDataType() == SHOW_DATA_TYPE_RULE) //区分控件显示的数据类型
    {//选择/不选择规则数据
        STRUCT_RULE strrule = maprule[_id];
        if(_seltype == SELECT_TYPE_SEL)
        {//增加规则
              m_sshManager->AddSelRule(strrule.m_sid);
        }
        else if(_seltype == SELECT_TYPE_REMOVESEL)
        {//删除规则
              m_sshManager->DelSelRule(strrule.m_sid);
        }
    }
    else if(psender->GetShowDataType() == SHOW_DATA_TYPE_BATRULE)
    {//选择/不选择规则组数据
        STRUCT_BATRULE strbatrule = mapbatrule[_id];
        if(_seltype == SELECT_TYPE_SEL)
        {//增加规则组
           m_sshManager->AddSelBatRule(strbatrule.m_sid);
        }
        else if(_seltype == SELECT_TYPE_REMOVESEL)
        {//删除规则组
           m_sshManager->DelSelBatRule(strbatrule.m_sid);
        }
    }
}

void CLS_WgtSshItem::SlotClearSelRule()
{
    //获取发送信息的控件
    QObject * obj = sender();
    CLS_MultiSelectComboBox * psender = dynamic_cast<CLS_MultiSelectComboBox *>(obj);

    if(psender == nullptr || m_sshManager == nullptr)
    {
        return;
    }

    if(psender->GetShowDataType() == SHOW_DATA_TYPE_RULE) //区分控件显示的数据类型
    {
        m_sshManager->ClearSelRule();
    }
    else if(psender->GetShowDataType() == SHOW_DATA_TYPE_BATRULE)
    {
        m_sshManager->ClearSelBatRule();
    }
}

void CLS_WgtSshItem::SlotStateChange(int _state)
{
   if(m_pCheckBox == nullptr || m_pComboBoxBatRule == nullptr)
   {
       return;
   }
   if(_state == Qt::Checked)
   {
       if(m_pLineEdit[ENUM_TABLE_COL_IP])
       {
           int checkres = CheckIpFormat(m_pLineEdit[ENUM_TABLE_COL_IP]->text());
           if(checkres == CHECK_IP_RES_EMPTY)
           {
               QMessageBox::warning(this,"提示","ssh配置-服务器Ip不能为空");
               m_pCheckBox->setCheckState(Qt::Unchecked);
               return;
           }
           else if(checkres == CHECK_IP_RES_ERR)
           {
               QMessageBox::information(this, "提示", "ssh配置-服务器Ip格式错误");
               m_pCheckBox->setCheckState(Qt::Unchecked);
               return;
           }
       }

       if(m_pLineEdit[ENUM_TABLE_COL_PORT])
       {
           if(m_pLineEdit[ENUM_TABLE_COL_PORT]->text().isEmpty())
           {
               QMessageBox::warning(this,"提示","ssh配置-服务器端口不能为空");
               m_pCheckBox->setCheckState(Qt::Unchecked);
               return;
           }
       }
       if(m_pLineEdit[ENUM_TABLE_COL_USERNAME])
       {
           if(m_pLineEdit[ENUM_TABLE_COL_USERNAME]->text().isEmpty())
           {
               QMessageBox::warning(this,"提示","ssh配置-服务器用户名不能为空");
               m_pCheckBox->setCheckState(Qt::Unchecked);
               return;
           }
       }
       if(m_pLineEdit[ENUM_TABLE_COL_PWD])
       {
           if(m_pLineEdit[ENUM_TABLE_COL_PWD]->text().isEmpty())
           {
               QMessageBox::warning(this,"提示","ssh配置-服务器密码不能为空");
               m_pCheckBox->setCheckState(Qt::Unchecked);
               return;
           }
       }

       if(m_pComboBoxBatRule->GetSelItemCount() == 0)
       {
           QMessageBox::warning(this,"提示","ssh配置-没有勾选规则组");
           m_pCheckBox->setCheckState(Qt::Unchecked);
           return;
       }
   }
}


