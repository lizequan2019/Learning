/*
 * CLS_WgtSshItem
 * 作用: 这个类是封装了程序中需要的 ip port user pwd 规则选则 规则组选择 删除按钮 增加按钮 组件，
 * 成为了一个大的组件 方便主界面程序调用
 *
 */

#ifndef CLS_WGTSSHITEM_H
#define CLS_WGTSSHITEM_H

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QComboBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QMessageBox>
#include "PublicData.h"
#include "CLS_SshManager.h"
#include "CLS_MultiSelectComboBox.h"

class CLS_WgtSshItem : public QWidget
{
    Q_OBJECT

    //枚举 一行需要的组件
    enum ENUM_TABLE_COL_INDEX
    {
        ENUM_TABLE_COL_IP,       //输入框
        ENUM_TABLE_COL_PORT,     //输入框
        ENUM_TABLE_COL_USERNAME, //输入框
        ENUM_TABLE_COL_PWD,      //输入框

        ENUM_TABLE_COL_RULE,     //规则选择下拉框
        ENUM_TABLE_COL_BATRULE,  //规则组选择下拉框

        ENUM_TABLE_COL_DEL,      //按钮
        ENUM_TABLE_COL_ADD,      //按钮

        ENUM_TABLE_COL_NUM,
    };

public:

    /******************************************************
    函数 : CLS_WgtSshItem
    功能描述 : 初始化Ssh项控件
    输入参数 : _iId是此记录当前的序号   parent是父窗口
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    explicit CLS_WgtSshItem(int _iId, QWidget * parent = nullptr);

    /******************************************************
    函数 : SetSsh
    功能描述 : 设置Ssh连接信息
    输入参数 : STRUCT_SSH  自定义数据结构存储ssh连接信息
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSsh(STRUCT_SSH);

    /******************************************************
    函数 : GetSsh
    功能描述 : 获取Ssh连接信息
    输入参数 : 无
    输出参数 : 无
    返回值 : STRUCT_SSH  自定义数据结构存储ssh连接信息
    其它 : 无
    *******************************************************/
    STRUCT_SSH GetSsh();

    /******************************************************
    函数 : SetId
    功能描述 : 设置该控件对应的id号
    输入参数 : int id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetId(int);

    /******************************************************
    函数 : GetId
    功能描述 : 获取该控件对应的id号
    输入参数 : 无
    输出参数 : 无
    返回值 : int id号
    其它 : 无
    *******************************************************/
    int  GetId();

    /******************************************************
    函数 : IsSelect
    功能描述 : 获取该控件是否被选择
    输入参数 : 无
    输出参数 : 无
    返回值 : bool true选择  false未选择
    其它 : 无
    *******************************************************/
    bool IsSelect();

    /******************************************************
    函数 : SetSelect
    功能描述 : 设置该控件选择
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetSelect();

    //以下三条均属于获取子控件指针
    QCheckBox   * GetCheckBoxPoint();

    QPushButton * GetAddBtnPoint();

    QPushButton * GetDelBtnPoint();

    //Ssh管理类的指针
    CLS_SshManager * m_sshManager;

public slots:

    void SlotClickDelBtn();

    /******************************************************
    函数 : SlotSelChange
    功能描述 : 响应 CLS_MultiSelectComboBox 对象下拉框勾选/取消勾选 信号
    输入参数 : SELECT_TYPE 选择类型(选择/未选择)   QString 规则/规则组id
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotSelChange(SELECT_TYPE,QString);

    /******************************************************
    函数 : SlotClearSelRule
    功能描述 : 响应 CLS_MultiSelectComboBox 对象清除 存储的已选择规则(组)信号
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotClearSelRule();

    /******************************************************
    函数 : SlotStateChange
    功能描述 : 响应本类中CheckBox控件的信号
    输入参数 : int  CheckBox改变后的值
    输出参数 : 无
    返回值 : 无
    其它 : 在CheckBox被勾选时判断需要填写的项是否正确，否则不能勾选
    *******************************************************/
    void SlotStateChange(int);

private:

    //表示此类对象对外部表示的一个id号，方便区分查找
    int m_iId;

    //选择按钮控件
    QCheckBox * m_pCheckBox;

    //输入框控件组
    QLineEdit * m_pLineEdit[ENUM_TABLE_COL_PWD + 1];

    //规则选择组件
    CLS_MultiSelectComboBox * m_pComboBoxRule;

    //规则组选择组件
    CLS_MultiSelectComboBox * m_pComboBoxBatRule;

    //增加按钮
    QPushButton * m_pBtnAdd;

    //删除按钮
    QPushButton * m_pBtnDel;

signals:
     void SignAddItem();
     void SignDelItem(int);
     void SigEchoRes(int,QJsonObject);
     void SigSshErrMsg(QString);
};

#endif // CLS_WGTSSHITEM_H
