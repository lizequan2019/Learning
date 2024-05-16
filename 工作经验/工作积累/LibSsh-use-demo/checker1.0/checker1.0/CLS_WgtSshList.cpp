#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include "CLS_WgtSshList.h"

//静态变量 在程序运行的过程中值只会增长
int CLS_WgtSshList::m_iCreateID = 0;

CLS_WgtSshList::CLS_WgtSshList(QWidget *parent) : QScrollArea(parent)
{
    this->setWidgetResizable(true);

    m_pWidget = new QWidget(this);
    m_pWidget->setObjectName("wgt_hostinfolistinside");
    m_pWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Maximum);

    m_pVboxMain = new QVBoxLayout;
    m_pVboxMain->setSpacing(7);
    m_pVboxMain->setContentsMargins(0,0,0,0);  //设置控件的外边距

    m_pWidget->setLayout(m_pVboxMain);

    this->setWidget(m_pWidget);
    this->setWidgetResizable(true);

    this->setMinimumHeight(50);
}

void CLS_WgtSshList::AddItem(STRUCT_SSH _info)
{
     if(m_pVboxMain == nullptr)
     {
         return;
     }

     //创建一个Ssh的Id号
     int _id = CreateId();

     //判断是否重复添加 根据id号查询
     if(m_qmapItem.find(_id) != m_qmapItem.end())
     {
         return;
     }

     //创建一个新的对象
     CLS_WgtSshItem * pItem = new CLS_WgtSshItem(_id);
     //CLS_WgtSshItem 对象设置ssh连接信息
     pItem->SetSsh(_info);

     //每次都添加到布局的末尾
     m_pVboxMain->insertWidget(-1,pItem);
     //存储控件id和控件的关系
     m_qmapItem[_id] = pItem;


     //信号的连接
     connect(pItem,SIGNAL(SignAddItem()),this,SLOT(SlotAddItem()));
     connect(pItem,SIGNAL(SignDelItem(int)),this,SLOT(SlotDelItem(int)));
     connect(pItem,SIGNAL(SigEchoRes(int,QJsonObject)),this,SIGNAL(SigEchoRes(int,QJsonObject)));
     connect(pItem,SIGNAL(SigSshErrMsg(QString)),this,SIGNAL(SigSshErrMsg(QString)));
     //显示增加按钮
     SetBtnAddShow();
}


void CLS_WgtSshList::DeleteSingleItem(int _Id)
{
    if(m_pVboxMain == nullptr)
    {
        return;
    }

    auto iter = m_qmapItem.begin();
    while(iter != m_qmapItem.end())
    {
        if(m_qmapItem.size() <= const_AllowMinExistNum)
        {//界面输入栏数量小于等于const_AllowMinExistNum时不再删除 为了界面美观
            break;
        }

        CLS_WgtSshItem *pItem = iter.value();
        if(pItem && pItem->GetId() == _Id)
        {//如果id号相同

            //取消信号&槽的连接
            disconnect(pItem,SIGNAL(SignAddItem()),this,SLOT(SlotAddItem()));
            disconnect(pItem,SIGNAL(SignDelItem(int)),this,SLOT(SlotDelItem(int)));
            disconnect(pItem,SIGNAL(SigEchoRes(int,QJsonObject)),this,SIGNAL(SigEchoRes(int,QJsonObject)));
            disconnect(pItem,SIGNAL(SigSshErrMsg(QString)),this,SIGNAL(SigSshErrMsg(QString)));
            //删除Ssh项控件
            m_pVboxMain->removeWidget(pItem);
            delete pItem;
            pItem = nullptr;

            m_qmapItem.erase(iter);
            break;
        }
        ++iter;
    }
    SetBtnAddShow();
}


void CLS_WgtSshList::DeleteSelectItem()
{
    if(m_pVboxMain == nullptr)
    {
        return;
    }

    auto iter = m_qmapItem.begin();
    while(iter != m_qmapItem.end())
    {
        if(m_qmapItem.size() <= const_AllowMinExistNum)
        {//界面输入栏小于等于const_AllowMinExistNum行时不再删除行 为了界面美观
            break;
        }

        CLS_WgtSshItem *pItem = iter.value();

        if(pItem && pItem->IsSelect())
        {//输入栏选中并且对应的界面不为空指针

            disconnect(pItem,SIGNAL(SignAddItem()),this,SIGNAL(SignAddItem()));
            disconnect(pItem,SIGNAL(SignDelItem(QPushButton *)),this,SIGNAL(SignDelItem(QPushButton *)));
            disconnect(pItem,SIGNAL(SigEchoRes(int,QJsonObject)),this,SIGNAL(SigEchoRes(int,QJsonObject)));
            disconnect(pItem,SIGNAL(SigSshErrMsg(QString)),this,SIGNAL(SigSshErrMsg(QString)));
            m_pVboxMain->removeWidget(pItem);
            delete pItem;
            pItem = nullptr;

            m_qmapItem.erase(iter);
            continue;
        }
        ++iter;
    }
    SetBtnAddShow();
}

void CLS_WgtSshList::DeleteAllItem()
{
    if(m_pVboxMain == nullptr)
    {
        return;
    }

    auto iter = m_qmapItem.begin();
    while(iter != m_qmapItem.end())
    {
        if(m_qmapItem.size() <= const_AllowMinExistNum)
        {//界面输入栏小于等于const_AllowMinExistNum行时不再删除行 为了界面美观
            return;
        }

        CLS_WgtSshItem *pItem = iter.value();
        if(pItem)
        {
            m_pVboxMain->removeWidget(pItem);
            delete pItem;
            pItem = nullptr;
        }
        m_qmapItem.erase(iter);
    }

    SetBtnAddShow();
}

void CLS_WgtSshList::AllSelect(bool _bSelect)
{
    STRUCT_SSH info;
    for(QMap<int, CLS_WgtSshItem *>::iterator it = m_qmapItem.begin(); it != m_qmapItem.end(); it++)
    {
        CLS_WgtSshItem *pItem = it.value();
        if(pItem && pItem->GetCheckBoxPoint())
        {
            pItem->GetCheckBoxPoint()->setChecked(_bSelect);
        }
    }
}

QMap<int, CLS_WgtSshItem *> CLS_WgtSshList::GetSshItems()
{
    return m_qmapItem;
}

void CLS_WgtSshList::SetBtnAddShow()
{
    if(m_qmapItem.isEmpty())
    {
        return;
    }

    //设置添加按钮的显隐, 只有最后一行的添加按钮是显示的
    auto iter = m_qmapItem.begin();
    while(iter != m_qmapItem.end())
    {
        if(iter.value()->GetAddBtnPoint())
        {
            iter.value()->GetAddBtnPoint()->show();
        }
        ++iter;
    }

    iter = m_qmapItem.begin();
    while(iter != m_qmapItem.end() - 1)
    {
        if(iter.value()->GetAddBtnPoint())
        {
            iter.value()->GetAddBtnPoint()->hide();
        }
        ++iter;
    }
}

int CLS_WgtSshList::CreateId()
{
    int ret = ++m_iCreateID;
    return ret;
}

void CLS_WgtSshList::SlotAddItem()
{
    STRUCT_SSH sshinfo;
    AddItem(sshinfo);
}

void CLS_WgtSshList::SlotDelItem(int _Id)
{
    DeleteSingleItem(_Id);
}


