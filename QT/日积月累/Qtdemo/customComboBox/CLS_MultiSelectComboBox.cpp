#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_MultiSelectComboBox.h"

MultiSelectComboBox::MultiSelectComboBox(QWidget *parent)
    : QComboBox(parent)
    , m_iCheckBoxNextIndex(0)
    , m_iCheckBoxStartIndex(0)
{

    //创建显示文本框界面
    m_plineEdt  = new QLineEdit(this);
    m_plineEdt->setReadOnly(true);
    m_plineEdt->installEventFilter(this);

    //创建搜索文本框界面
    m_plineSer  = new QLineEdit(this);
    m_plineSer->setPlaceholderText(QStringLiteral("Search........."));
    m_plineSer->setClearButtonEnabled(true);
    m_plineSer->installEventFilter(this);


    //创建下拉列表界面
    m_plistWgt  = new QListWidget(this);

    /*设置搜索框*/
    QListWidgetItem* currentItem = new QListWidgetItem(m_plistWgt);
    m_plistWgt->addItem(currentItem);
    m_plistWgt->setItemWidget(currentItem, m_plineSer);

    this->setModel(m_plistWgt->model());
    this->setView(m_plistWgt);
    this->setLineEdit(m_plineEdt);
    this->setMaxVisibleItems(10);  //设置可显示项的最大数量
    this->installEventFilter(this);

    connect(m_plineSer, SIGNAL(textChanged(const QString&)), this, SLOT(onSearch(const QString&)));
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MultiSelectComboBox::itemClicked);

    //设置样式表
    QFile file(QStringLiteral("./style.qss"));
    file.open(QIODevice::ReadOnly);
    this->setStyleSheet(QString::fromLatin1(file.readAll()));

    if(m_plineSer)
    {
        m_iCheckBoxNextIndex++;
        m_iCheckBoxStartIndex++;
    }
}

MultiSelectComboBox::~MultiSelectComboBox()
{

}

void MultiSelectComboBox::hidePopup()
{
     if(CanHide())
     {
          QComboBox::hidePopup();
     }
}

void MultiSelectComboBox::showPopup()
{
    QComboBox::showPopup();
}

void MultiSelectComboBox::addItem(const QString& _text, const QVariant& _variant)
{
    if(m_plistWgt == nullptr)
    {
        return;
    }

    //新增的列表项对象
    QListWidgetItem* item = nullptr;

    //获取当前的列表项数
    int count = m_plistWgt->count();
    if(count == m_iCheckBoxNextIndex)
    {//判断下索引和个数的关系是否匹配，注意此时新的checkbox还有添加到列表中

        item = new QListWidgetItem(m_plistWgt);
        STRUCT_CK_USER_DATA userdata = _variant.value<STRUCT_CK_USER_DATA>();
        item->setData(Qt::UserRole + m_iCheckBoxNextIndex,_variant);
    }

    QCheckBox * checkbox = new QCheckBox(this);
    checkbox->setText(_text);
    checkbox->installEventFilter(this);

    m_plistWgt->addItem(item);
    m_plistWgt->setItemWidget(item, checkbox);

    m_iCheckBoxNextIndex++;

    connect(checkbox, &QCheckBox::stateChanged, this, &MultiSelectComboBox::stateChange);
}

QStringList MultiSelectComboBox::currentText()
{
    QStringList text_list;

    if(m_plineEdt == nullptr)
    {
        return text_list;
    }

    if (!m_plineEdt->text().isEmpty())
    {
        //以;为分隔符分割字符串
        text_list = m_plineEdt->text().split(';');
    }
    return text_list;
}

int MultiSelectComboBox::count() const
{
    if(m_plistWgt == nullptr)
    {
        return 0;
    }

    int count = m_plistWgt->count() - m_iCheckBoxStartIndex;
    if (count < 0)
    {
        count = 0;
    }
    return count;
}

void MultiSelectComboBox::setSearchBarPlaceHolderText(const QString & _text)
{
    if(m_plineSer == nullptr)
    {
        return;
    }
    m_plineSer->setPlaceholderText(_text);
}

void MultiSelectComboBox::setPlaceHolderText(const QString & _text)
{
    if(m_plineEdt == nullptr)
    {
        return;
    }
    m_plineEdt->setPlaceholderText(_text);
}

void MultiSelectComboBox::resetSelection()
{
    if(m_plistWgt == nullptr || m_plineEdt == nullptr)
    {
        return;
    }

    m_plineEdt->clear();

    int count = m_plistWgt->count();
    for (int i = m_iCheckBoxStartIndex; i < count; i++)
    {
        QListWidgetItem * itemwgt = m_plistWgt->item(i);

        if(itemwgt)
        {
            //设置用户数据为空
            itemwgt->setData(Qt::UserRole,QVariant());

            QWidget *widget = m_plistWgt->itemWidget(itemwgt);

            if(widget)
            {
                QCheckBox *check_box = static_cast<QCheckBox*>(widget);
                if(check_box)
                {
                    check_box->setChecked(false);
                }
            }
        }
    }
}

void MultiSelectComboBox::clearCheckBox()
{
    if(m_plineEdt == nullptr || m_plistWgt == nullptr)
    {
        return;
    }

    //清空文本显示框内容
    m_plineEdt->clear();

    m_vecSelItem.clear();

    //清空下拉框中的checkbox控件
    int count = m_plistWgt->count();
    while (count != m_iCheckBoxStartIndex)
    {
        for (int i = m_iCheckBoxStartIndex; i < count; i++)
        {
            QListWidgetItem * itemwgt = m_plistWgt->item(i);

            QWidget * widget = m_plistWgt->itemWidget(m_plistWgt->item(i));

            if(widget)
            {
                QCheckBox * check_box = static_cast<QCheckBox*>(widget);
                disconnect(check_box, &QCheckBox::stateChanged, this, &MultiSelectComboBox::stateChange);
                delete check_box;
                check_box = nullptr;
            }

            if(itemwgt)
            {
               m_plistWgt->removeItemWidget(itemwgt);

               delete  itemwgt;
               itemwgt = nullptr;
            }
        }

        count = m_plistWgt->count();
    }


    m_iCheckBoxNextIndex = 0;
    m_iCheckBoxStartIndex = 0;

    if(m_plineSer)
    {
        m_iCheckBoxNextIndex++;
        m_iCheckBoxStartIndex++;
    }
}


bool MultiSelectComboBox::eventFilter(QObject * watched, QEvent * event)
{
    QFocusEvent * pfocusevent = (QFocusEvent *)event;
    if(watched == this && pfocusevent->reason() == Qt::PopupFocusReason)
    {//控件抓取/释放键盘焦点。
        if(event->type() == QEvent::FocusAboutToChange)
        {//打开下拉框
            showPopup();
        }
        else if(event->type() == QEvent::FocusIn)
        {//关闭下拉框
            hidePopup();
        }
        return true;
    }
    else if(event->type() == QEvent::ContextMenu)
    {//弹出菜单的事件不予理会
        return true;
    }
    else if(((QMouseEvent *)event)->button() == Qt::RightButton)
    {//弹出菜单的事件不予理会
        return true;
    }

    return QComboBox::eventFilter(watched, event);
}

void MultiSelectComboBox::wheelEvent(QWheelEvent *event)
{
    //禁用QComboBox默认的滚轮事件
    Q_UNUSED(event);
}

void MultiSelectComboBox::keyPressEvent(QKeyEvent *event)
{
    QComboBox::keyPressEvent(event);
}

void MultiSelectComboBox::stateChange(int state)
{
    //获取信号的发送者
    QObject* obj = sender();
    QCheckBox * psender = dynamic_cast<QCheckBox*>(obj);

    if(psender == nullptr || m_plistWgt == nullptr || m_plineEdt == nullptr)
    {
        return;
    }

    //获取用户数据
    int count = m_plistWgt->count();
    for (int i = m_iCheckBoxStartIndex; i < count; i++)
    {
        QWidget *widget = m_plistWgt->itemWidget(m_plistWgt->item(i));

        QVariant qvardata = m_plistWgt->item(i)->data(Qt::UserRole + i);

        STRUCT_CK_USER_DATA userdata = qvardata.value<STRUCT_CK_USER_DATA>();

        QCheckBox *check_box = static_cast<QCheckBox*>(widget);

        if (check_box && (psender == check_box))
        {
            if(state == Qt::Checked)
            {
                Q_EMIT SigSelChange(SELECT_TYPE_SEL,userdata.m_userdata);
            }
            else if(state == Qt::Unchecked)
            {
                Q_EMIT SigSelChange(SELECT_TYPE_REMOVESEL,userdata.m_userdata);
            }
            break;
        }
    }


    //根据选择操作，增加或删除checkbox对象
    if(psender)
    {
        if(state == Qt::Checked)
        {
            m_vecSelItem.push_back(psender);
        }
        else if(state == Qt::Unchecked)
        {
            for(int i = 0; i < m_vecSelItem.size(); ++i)
            {
                if(m_vecSelItem[i] && (m_vecSelItem[i] == psender))
                {
                    m_vecSelItem.remove(i); //删除对应的checkbox对象
                }
            }
        }
    }

    //每次都清空一下文本框
    m_plineEdt->clear();

    //重新显示文本框内容
    QString qstr;
    for(int i = 0; i < m_vecSelItem.size(); ++i)
    {
        qstr += m_vecSelItem[i]->text() + ";";
    }

    m_plineEdt->setText(qstr);
    m_plineEdt->setToolTip(m_plineEdt->text());
}

void MultiSelectComboBox::onSearch(const QString& _text)
{
    if(m_plistWgt == nullptr)
    {
        return;
    }

    for (int i = 1; i < m_plistWgt->count(); i++)
    {
        QCheckBox *check_box = static_cast<QCheckBox*>(m_plistWgt->itemWidget(m_plistWgt->item(i)));
        //文本匹配则显示，反之隐藏
        //Qt::CaseInsensitive模糊查询
        if (check_box->text().contains(_text, Qt::CaseInsensitive)) {
            m_plistWgt->item(i)->setHidden(false);
        }
        else
        {
            m_plistWgt->item(i)->setHidden(true);
        }
    }
}

void MultiSelectComboBox::itemClicked(int _index)
{
    if(m_plistWgt == nullptr)
    {
        return;
    }

    if (_index != 0)
    {
        QCheckBox *check_box = static_cast<QCheckBox*>(m_plistWgt->itemWidget(m_plistWgt->item(_index)));
        check_box->setChecked(!check_box->isChecked());
    }
}

bool MultiSelectComboBox::CanHide()
{
    if(m_plistWgt == nullptr)
    {
        return false;
    }

    if(m_plistWgt)
    {
        QPoint topleftpos = this->mapToGlobal(m_plistWgt->geometry().topLeft());
        int topleftx = topleftpos.x();
        int toplefty = topleftpos.y() + m_plineEdt->height();
        int bottomrightx = topleftx + m_plistWgt->width();
        int bottomrighty = toplefty + m_plistWgt->height();

        QPoint mousepos = QCursor::pos();

        if(mousepos.x() < topleftx  || mousepos.x() > bottomrightx || mousepos.y() < toplefty  || mousepos.y() > bottomrighty)
        {//鼠标在下拉框界面外面
            return true;
        }
    }
    return false;
}

