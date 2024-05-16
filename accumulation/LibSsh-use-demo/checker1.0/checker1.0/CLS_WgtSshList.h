/*
 *  用于管理CLS_WgtSshItem多个对象的增删
 */
#ifndef CLS_WGTSSHLIST_H
#define CLS_WGTSSHLIST_H



#include <QBoxLayout>
#include <QMap>
#include <QScrollArea>
#include <QVector>
#include "CLS_WgtSshItem.h"

class CLS_WgtSshList : public QScrollArea
{
    Q_OBJECT
public:
    explicit CLS_WgtSshList(QWidget *parent = nullptr);

    /******************************************************
    函数 :  AddItem
    功能描述 : 在界面增加一个ssh项控件
    输入参数 : _info 有效的ssh信息 这是自定义的数据结构
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void AddItem(STRUCT_SSH _info);

    /******************************************************
    函数 :  DeleteSingleItem
    功能描述 : 在界面删除一个ssh项控件
    输入参数 : int ssh项对应的id号
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void DeleteSingleItem(int);

    /******************************************************
    函数 :  DeleteSelectItem
    功能描述 : 在界面删除被选中的ssh项控件
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void DeleteSelectItem();

    /******************************************************
    函数 :  DeleteAllItem
    功能描述 : 删除界面所有的ssh控件
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void DeleteAllItem();

    /******************************************************
    函数 :  AllSelect
    功能描述 : 全选/不全选 界面所有的ssh控件
    输入参数 : _bSelect true表示选择  fasle表示不选择  默认值是选择
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void AllSelect(bool _bSelect = true);

    /******************************************************
    函数 :  GetSshItems
    功能描述 : 获取界面上所有的ssh项集合
    输入参数 : 无
    输出参数 : 无
    返回值 : QMap<int, CLS_WgtSshItem *>  存储CLS_WgtSshItem对象(ssh项)和其id号的关系
    其它 : 无
    *******************************************************/
    QMap<int, CLS_WgtSshItem *> GetSshItems();


private:

    static int m_iCreateID; //此变量的作用是给每个ssh项一个唯一的id号，便于后期查找

    QVBoxLayout * m_pVboxMain; //m_pWidget界面对应的布局

    QWidget * m_pWidget; //存储所有ssh项控件的界面

    QMap<int, CLS_WgtSshItem *> m_qmapItem; //存储CLS_WgtSshItem对象(ssh项)和其id号的关系

    /******************************************************
    函数 :  SetBtnAddShow
    功能描述 :  显示ssh项控件中增加按钮
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SetBtnAddShow();

    /******************************************************
    函数 :  CreateId
    功能描述 :  为一个ssh项创建一个唯一的id号
    输入参数 :  无
    输出参数 :  int 创建的id号
    返回值 : 无
    其它 : 无
    *******************************************************/
    int CreateId();

public slots:
    /******************************************************
    函数 :  SlotAddItem
    功能描述 :  槽函数 增加一个ssh项控件
    输入参数 :  无
    输出参数 :  无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotAddItem();

    /******************************************************
    函数 :  SlotDelItem
    功能描述 :  槽函数 删除一个ssh项控件
    输入参数 :  int ssh项对应的id号
    输出参数 :  无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotDelItem(int);

signals:
    /******************************************************
    函数 : SigEchoRes
    功能描述 :  发射信号 通知主界面显示每个ssh项的执行结果
    输入参数 :  int ssh项的id号  QJsonObject 要显示的执行结果
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SigEchoRes(int,QJsonObject);

    void SigSshErrMsg(QString);
};

#endif // CLS_WGTSSHLIST_H
