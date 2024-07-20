#ifndef CLS_WGTSYSTEMSETTING_H
#define CLS_WGTSYSTEMSETTING_H

#include <QDebug>
#include <QWidget>
#include <QVariant>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QMessageBox>

#include "PublicData.h"
#include "CLS_PublicFun.h"
#include "CLS_LoadRuleManager.h"

namespace Ui {
class CLS_WgtSystemSetting;
}

class CLS_WgtSystemSetting : public QWidget
{
    Q_OBJECT

public:
    explicit CLS_WgtSystemSetting(QWidget *parent = nullptr);
    ~CLS_WgtSystemSetting();

    /******************************************************
    函数 : SetTreeMinSize
    功能描述 : 设置树控件缩小显示
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 修改尺寸的控件包括  树控件  树控件所在的父界面  修改的尺寸数值是硬编码
    *******************************************************/
    void SetTreeMinSize();

    /******************************************************
    函数 : SetTreeMaxSize
    功能描述 : 设置树控件放大显示
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 修改尺寸的控件包括  树控件  树控件所在的父界面  修改的尺寸数值是硬编码
    *******************************************************/
    void SetTreeMaxSize();

    /******************************************************
    函数 : GetOperatorName
    功能描述 : 获取tab页面中登陆账号的用户名
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    QString GetOperatorName();


private:
    /******************************************************
    函数 : SaveUserInfo
    功能描述 : 保存用户登录界面填写的信息，并将数据发送到外界
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SaveUserInfo();


private slots:

    void on_btn_allupdate_clicked(); //全量更新

    void on_btn_addupdate_clicked(); //增量更新

public slots:

    /******************************************************
    函数 : SlotViewRule
    功能描述 : 预览本地缓存的规则数据
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 从程序缓存的信息中预览规则数据，注意调用此函数前程序必须要更新缓存数据才能显示最新的数据
    *******************************************************/
    void SlotViewRule();

    /******************************************************
    函数 : SlotUpdateErr
    功能描述 : 响应更新规则失败的信号，弹窗提示用户
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 :
    *******************************************************/
    void SlotUpdateErr();

signals:

    void SigAllUpdate();   //全量更新信号

    void SigAddUpdate();   //增量更新信号

    void SigSaveUserInfo(QVariant);//保存用户信息的信号

private:
    Ui::CLS_WgtSystemSetting *ui;
    QTreeWidget * m_pTreeWgt;
};

#endif // CLS_WGTSYSTEMSETTING_H
