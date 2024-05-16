/*
 * 此类为自定义下拉框控件，目前不使用搜索框
 * 如果要在下拉框界面增加额外的控件(不是QCheckBox的)，则需要注意索引与控件的对应关系
 * 【TODO 这里类需要优化  最好是使用模板类去实现 否则大部分逻辑代码都会写两遍】
 */

#pragma once

#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QEvent>
#include <QMap>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include "PublicData.h"
#include "CLS_LoadRuleManager.h"

class CLS_MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit CLS_MultiSelectComboBox(QWidget *parent = Q_NULLPTR);
    ~CLS_MultiSelectComboBox();

    /*************************************************
    函数名称:  setCheckBoxChecked
    功能描述:  设置QCheckBox的勾选与否
    参数传入:  _ruleId  规则(组)id号   _isChecked true表示勾选 false表示不勾选
    参数传出:  无
    返回值:   无
    其他: 无
    *************************************************/
    void setCheckBoxChecked(const QString & _ruleId, const bool & _isChecked);

    /*************************************************
    函数名称:  addItem
    功能描述:  添加一条checkbox选项
    参数传入:  _text  checkbox项显示的文字   _variant用户数据
    参数传出:
    返回值:
    其他:
    *************************************************/
    void addItem(const QString& _text, const QVariant& _variant = QVariant());

    /*************************************************
    函数名称:  currentText
    功能描述:  返回当前选中的checkbox项对应的文本
    参数传入:
    参数传出:
    返回值:    QStringList 选中项对应的本文
    其他:
    *************************************************/
    QStringList currentText();

    /*************************************************
    函数名称:  count
    功能描述:  返回当前选中的checkbox项的总数
    参数传入:
    参数传出:
    返回值:    int 选中的checkbox项的总数
    其他:
    *************************************************/
    int count()const;

    /*************************************************
    函数名称:  SetSearchBarPlaceHolderText
    功能描述:  设置搜索文本框默认的显示内容
    参数传入:  _text  默认显示的文本
    参数传出:
    返回值:
    其他:
    *************************************************/
    void setSearchBarPlaceHolderText(const QString &_text);

    /*************************************************
    函数名称:  SetPlaceHolderText
    功能描述:  设置文本框默认显示的内容
    参数传入:  _text  默认显示的文本
    参数传出:
    返回值:
    其他:
    *************************************************/
    void setPlaceHolderText(const QString& _text);

    /*************************************************
    函数名称:  ResetSelection
    功能描述:  下拉框中的所有checkbox恢复至不选择状态 而且所对应的用户数据设置为空,同时清空文本显示框
    参数传入:
    参数传出:
    返回值:
    其他:
    *************************************************/
    void resetSelection();

    /*************************************************
    函数名称:  clear
    功能描述:  清空下拉框中的所有checkbox控件
    参数传入:
    参数传出:
    返回值:
    其他:
    *************************************************/
    void clearCheckBox();

    void SetShowDataType(SHOW_DATA_TYPE _type);

    SHOW_DATA_TYPE GetShowDataType();

    void SetPlaceHolderText(const QString& _text);

    int GetSelItemCount();


private:
    /*************************************************
    函数名称: hidePopup
    功能描述: 隐藏CLS_MultiSelectComboBox的下拉框界面
    参数传入:
    参数传出:
    返回值:
    其他:
    *************************************************/
    virtual void hidePopup();

    /*************************************************
    函数名称: CanHide
    功能描述: 根据鼠标当前的坐标位置判断下拉框界面是否可隐藏,如果鼠标处在下拉框界面的内部则不能隐藏，反之可以隐藏
    参数传入:
    参数传出:
    返回值:  不能隐藏(false) 可以隐藏(true)
    其他:
    *************************************************/
    bool CanHide();

protected:

    //事件过滤器
    virtual bool eventFilter(QObject *watched,QEvent *event);

    //滚轮事件
    virtual void wheelEvent(QWheelEvent *event);

    //按键事件
    virtual void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:

    /*************************************************
    函数名称:  stateChange
    功能描述:  响应checkbox项的状态变化
    参数传入:  _state checkbox的状态值
    参数传出:
    返回值:
    其他:
    *************************************************/
    void stateChange(int _state);

    /*************************************************
    函数名称:  onSearch
    功能描述:  响应搜索框内容的变化
    参数传入:  _text搜索框的内容
    参数传出:
    返回值:
    其他:
    *************************************************/
    void onSearch(const QString& _text);


    //槽函数：点击下拉框选项
    /*************************************************
    函数名称:  itemClicked
    功能描述:  响应下拉框选项点击
    参数传入:  _index 被点击项的索引
    参数传出:
    返回值:
    其他:
    *************************************************/
    void itemClicked(int _index);

Q_SIGNALS:

    /*************************************************
    函数名称:  SigSelChange
    功能描述:  信号：向外界发送数据
    参数传入:  SELECT_TYPE 勾选框操作类型(是勾选还是去除勾选)
              QString     用户数据 这个也可以定结构体
    参数传出:
    返回值:
    其他:
    *************************************************/
    void SigSelChange(SELECT_TYPE,QString);


    /*************************************************
    函数名称:  SigClearSelRule
    功能描述:  向外界发送清除 存储的已选择的规则(组)
    参数传入:
    参数传出:
    返回值:
    其他:
    *************************************************/
    void SigClearSelRule();


private:

    //即将要添加的勾选框的索引值  因为下拉界面中可能会增加其他控件，这会对勾选框的索引值产生影响
    int m_iCheckBoxNextIndex;

    //勾选框起始索引
    int m_iCheckBoxStartIndex;

    //列表部件(下拉框部件)
    QListWidget * m_plistWgt;

    //文本框
    QLineEdit * m_plineEdt;

    //搜索框
    QLineEdit * m_plineSer;

    //下拉框界面显示标志  true显示 false不显示
    bool m_blDropdownShow;

    //按照点击顺序存储checkbox对象
    QVector<QCheckBox *> m_vecSelItem;

    //记录当前已经选中的规则id号集合
    QVector<QString> m_vecSelRuleId;

    SHOW_DATA_TYPE  m_showDataType;
};

