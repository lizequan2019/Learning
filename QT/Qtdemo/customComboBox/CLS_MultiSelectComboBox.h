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


//用户数据结构体,是为了设置每个列表项的信息
struct STRUCT_CK_USER_DATA
{
     QString m_userdata;
};
Q_DECLARE_METATYPE(STRUCT_CK_USER_DATA)


//下拉框对勾选框的操作
enum SELECT_TYPE
{
    SELECT_TYPE_SEL,       //勾选
    SELECT_TYPE_REMOVESEL, //去除勾选
};




class MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit MultiSelectComboBox(QWidget *parent = Q_NULLPTR);
    ~MultiSelectComboBox();


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


private:
    /*************************************************
    函数名称: hidePopup
    功能描述: 隐藏MultiSelectComboBox的下拉框界面
    参数传入:
    参数传出:
    返回值:
    其他:
    *************************************************/
    virtual void hidePopup();


    virtual void showPopup();

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

    //按照点击顺序存储checkbox对象
    QVector<QCheckBox *> m_vecSelItem;
};
