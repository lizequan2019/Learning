## 拖拽事件学习
&emsp;&emsp;代码直接看工程项目QT_Drag就好了，我这里只是说一下在测试中发现的一些细节

```c++
Qt::DropAction aa = dg->exec(Qt::CopyAction|Qt::MoveAction|Qt::LinkAction,Qt::LinkAction);

```
&emsp;&emsp;上面这段代码，是启动拖拽功能，主要说一下后面参数的作用，先看一下官方的介绍


```c++
    /*
      Starts the drag and drop operation and returns a value indicating the requested drop action when it is completed. 
      The drop actions that the user can choose from are specified in supportedActions.
      The defaultDropAction determines which action will be proposed when the user performs a drag without using modifier keys.*/

Qt::DropAction QDrag::exec(Qt::DropActions supportedActions, Qt::DropAction defaultDropAction)
```
&emsp;&emsp;从介绍上看supportedActions参数表示用户可以选择拖拽动作的范围，defaultDropAction这个参数表示当用户没有使用修饰键时程序会使用defaultDropAction作为拖拽动作。我当时很疑惑，为什么会扯到修饰键(ctrl、shift、alt)。
&emsp;&emsp;这里还涉及到另一个事件处理函数**void dragMoveEvent(QDragMoveEvent * e)**，修饰键就是和它有关系。

```c++
/*
  This event handler is called if a drag is in progress, and when any of the following conditions occur: 
  the cursor enters this widget, the cursor moves within this widget, or a modifier key is pressed on the keyboard while this widget has the focus. The event is passed in the event parameter.
  如果拖动正在进行，并且出现以下任何情况时，将调用此事件处理程序：光标进入此小部件，光标在该小部件内移动，或者在该小部件具有焦点时按下键盘上的修改器键。事件在事件参数中传递。
*/
[virtual protected] void QWidget::dragMoveEvent(QDragMoveEvent *event)
```

&emsp;&emsp;由此可见上面提到的修饰键是在dragMoveEvent中进行使用的，所以dragMoveEvent内部逻辑可以这样实现。
&emsp;&emsp;**在下面的代码中你会看到注释，没有使用修饰键则设置拖拽动作不会生效**，那么如何确定呢？这里就要结合到**defaultDropAction**参数，上文说到

> defaultDropAction这个参数表示当用户没有使用修饰键时程序会使用defaultDropAction作为拖拽动作

所以现在没有使用修饰键的分支逻辑的拖拽动作就是defaultDropAction，所以这一个分支其实是不需要写的

```c++
     void dragMoveEvent(QDragMoveEvent * e)
     {
         if(e->keyboardModifiers()==Qt::CTRL)
         {
            e->setDropAction(Qt::CopyAction);
         }
         else if(e->keyboardModifiers()==Qt::SHIFT)
         {
            e->setDropAction(Qt::MoveAction);
         }
         else if(e->keyboardModifiers()==Qt::ALT)
         {
            e->setDropAction(Qt::LinkAction);
         }
         else
         {//注意这一个分支虽然会在不使用修饰键时执行，但是设置的拖拽动作并不会是Qt::CopyAction
             e->setDropAction(Qt::CopyAction);  
         }

         e->accept();
     }
```

&emsp;&emsp;在后续的测试中也发现了**void dropEvent(QDropEvent * e)**中也可以进行拖拽动作的设置(感觉是QDragMoveEvent继承自QDropEvent的吧)，而且可以覆盖掉dragMoveEvent的设置(应该是dropEvent触发的时间更靠后)

## 拖放文件

```c++
class C:public QPushButton
{
    Q_OBJECT
public: 
    C(QString t="",QWidget *p=0):QPushButton(t,p)
    {

    }

    void dragEnterEvent(QDragEnterEvent * e)
    {//若拖动的数据包含一个 URL 则接受该事件，否则忽略该事件。
 
      if(e->mimeData()->hasUrls())
      {
           e->accept(); 
      } 
      else 
      {
          e->ignore();
      } 
    }

    void dropEvent(QDropEvent * e)
    {
      const QMimeData *pm=e->mimeData();

      QList<QUrl> u=pm->urls(); //读取出 URL 的地址列表。
 
      QString pth=u.at(0).toLocalFile(); //将地址转换为 QString
 
      if(!pth.isEmpty())
      {//判断地址 pth 是否为空

            QFile file(pth); //创建文件 file

            if(!file.open(QIODevice::ReadOnly)); //以只读方式打开文件

            QTextStream in(&file); //创建流用于读取文件的内容。

            setText(in.readAll()); 
      } 
    } //读出文件的内容，并设置为该部件的文本
};
```

---
---

## 剪贴板
&emsp;&emsp;这里主要说明一下剪贴板的分类，主要是三种类型如下:
1. 全局鼠标选择:是从全局鼠标选择中存储和检索数据，其好处是可以在以后使用鼠标中键来粘贴其数据，但仅在具有全局鼠标选择的系统上提供支持(比如 X11)，Window 和 macOS 都不支持这种剪贴板。
2. 查找(或搜索)缓冲区:表示从查找缓冲区中存储和检索数据，该方式用于macOS上保存搜索的字符串。
3. 全局剪贴板:Windows只支持全局剪贴板，这种剪贴板是完全的全局资源。
&emsp;&emsp;以上三种剪贴板使用 QClipboard::Mode 枚举进行描述
* QClipboard::Clipboard  0 全局剪贴板
* QClipboard::Selection  1 全局鼠标选择
* QClipboard::FindBuffer 2 查找缓冲区