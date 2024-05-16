# Qt属性系统

&emsp;&emsp;Q_PROPERTY()是一个宏，用来在一个类中声明一个属性property，由于该宏是qt特有的，需要用moc进行编译，所以使用的类必须继承于QObject类，还应使用Q_OBJECT宏。**Qt宏多用于QML、QtScript、Designer插件等场景。**

&emsp;&emsp;在Qt中属性和数据成员是两个不同的概念，他们可以相关联也可以没有联系，比如名为a的属性，与数据成员 a，虽然他们名称相同，若他们之间没有产生关联，则数据成员a与属性a是完全不相关的。

&emsp;&emsp;属性值可使用以下方式进行存取，属性值可以关联 存取函数和类成员变量
* 可使用 QObject::property 和 QObject::setProperty 函数进行存取
* 若属性有相关联的存取函数，则可使用存取函数进行存取
* 属性还可通过元对象系统的 QMetaObject 类进行存取。
* 若属性与某个数据成员相关联，则可通过存取普通数据成员的值来间接存取属性的
* **注意：Qt 中的类，只有属性没有数据成员，因此只能通过前面三种方式对属性值进行修改。**


# Q_PROPERTY 宏定义的解析
&emsp;&emsp; 宏定义中 | 符号两侧的变量必须要有一个,[]中的变量是可选项

```c++
Q_PROPERTY
(
    //属性的类型 可以是QVariant支持的类型或用户自定义类型(自定义的类型还需要注册)
    type

    //属性的名称
    name  

    //读取函数 设置函数 读取 设置属性的值    用于把指定的成员变量 memberName 设置为具有可读和可写性质
    (READ getFunction [WRITE setFunction] | MEMBER memberName [(READ getFunction | WRITE setFunction)])

    //用于把属性重置为默认值，该函数不能有参数，且返回值必须为 void。resetFunction用于指定函数名称。
    [RESET resetFunction]

    //表示给属性关联一个信号。如果设置该项，则应指定该类中已经存在的信号，每当属性值发生变化时就会发出该信号。
    //如果使用MEMBER变量，则NOTIFY信号必须为零或一个参数，且参数必须与属性的类型相同，该参数将接受该属性的新值。？？？
    [NOTIFY notifySignal]

    //设置版本号，默认为0
    [REVISION int]

    //用于设置属性在GUI设计工具的属性编辑器(例如Qt设计师)中是否可见,多数属性是可见的,默认值为ture,即可见。
    //该变量也可以指定一个返回布尔值的成员函数替代 true 或 false。
    [DESIGNABLE bool]

    //设置属性是否可被脚本引擎访问,默认为true
    [SCRIPTABLE bool]

    //设置在保存对象的状态时是否必须保存属性的值。大多数属性此值为true
    [STORED bool]

    //设置属性是否为可编辑的属性，每一个类只有一个USER属性(默认值为false)
    [USER bool]

    //表明属性的值是常量，常量属性不能有WRITE函数和NOTIFY信号。
    //对于同一个对象实例，每一次使用常量属性的READ函数都必须得到相同的值,但对于类的不同实例,这个值可以不同。
    [CONSTANT]

    //表示属性不能被派生类重写
    [FINAL]
)
```       

## 举例
```c++

//头文件
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QDebug>

class myclass : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int StudentNumber READ readNumber WRITE setNumber) //这样需要对应的成员变量去关联
    Q_PROPERTY(int TeacherNumber MEMBER m_iTeacherNumer)          //TeacherNumber 关联成员变量 m_iTeacherNumer
    Q_PROPERTY(int ClassLevel READ getClassLevel)

public:
    myclass();

    int readNumber();
    void setNumber(int _iNum);

    int readTeacherNumer();
    void setTeacherNumer(int _iNum);

    int getClassLevel();

    void myprint();

private:
    int m_iTeacherNumer;
    int m_iStudentNumber;
    int m_iClassLevel;
};

#endif // MYCLASS_H


//源文件
myclass::myclass()
{
    myprint();
}

int myclass::readNumber()
{
    return m_iStudentNumber;  //上面第一个Q_PROPERTY(int StudentNumber READ readNumber WRITE setNumber) 这就算关联了
}

void myclass::setNumber(int _iNum)
{
    m_iStudentNumber = _iNum;
}



int myclass::getClassLevel()
{
    return m_iClassLevel;
}



int myclass::readTeacherNumer()
{
    return m_iTeacherNumer;
}

void myclass::setTeacherNumer(int _iNum)
{
    m_iTeacherNumer = _iNum;
}



void myclass::myprint()
{
    //都是输出1000
    m_iTeacherNumer = 1000;
    qDebug()<<" m_iTeacherNumer = "<<m_iTeacherNumer
            <<" QObject::property(\"TeacherNumber\") = "<<QObject::property("TeacherNumber")
            <<" readTeacherNumer= "<<readTeacherNumer();

    //都是输出2000
    m_iStudentNumber = 2000;
    qDebug()<<" m_iStudentNumber = "<<m_iStudentNumber
            <<" QObject::property(\"StudentNumber\") = "<<QObject::property("StudentNumber")
            <<" readNumber = "<<readNumber();

    //都是输出3000
    m_iClassLevel = 3000;
    qDebug()<<" m_iClassLevel = "<<m_iClassLevel
            <<" QObject::property(\"ClassLevel\") = "<<QObject::property("ClassLevel")
            <<" getClassLevel = "<<getClassLevel();
}
```

## NOTIFY 信号项
&emsp;&emsp;这里要说一下这个项的使用，Q_PROPERTY中使用NOTIFY,后面跟的是类中已经出现的信号
```c++
    Q_PROPERTY(int StudentNumber READ readNumber WRITE setNumber NOTIFY SignStudent)
    Q_PROPERTY(int TeacherNumber MEMBER m_iTeacherNumer NOTIFY SignTeacher)

    signals:
    void SignStudent(int _iNum);  //StudentNumber属性关联的信号
    void SignTeacher(int _iNum);  //TeacherNumber属性关联的信号
```

&emsp;&emsp;上面代码中StudentNumber的信号，需要自己在更改属性值的时候去emit 信号和原来的写法相同。

&emsp;&emsp;如果想让属性值更改时自动emit 信号，可以按照以下方法去做，这里以TeacherNumber属性为例。

```c++
    Q_PROPERTY(int TeacherNumber MEMBER m_iTeacherNumer NOTIFY SignTeacher) //需要关联一个成员变量

    this->setProperty("TeacherNumber", QVariant(4)); //更改属性值的时候只有使用这个方法能自动emit 信号
```

## 参考网址

&emsp;&emsp;http://blog.sina.com.cn/s/blog_c3c7614d0102wdm2.html