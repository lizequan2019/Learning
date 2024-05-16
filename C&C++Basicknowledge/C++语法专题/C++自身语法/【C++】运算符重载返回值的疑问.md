&emsp;&emsp;关于为什么**有的运算符重载需要返回一个类的引用**的问题，可以参考下面这篇文章

> https://www.cnblogs.com/codingmengmeng/p/5871254.html


&emsp;&emsp;这里我大概的总结一下文章中的观点
#### 使用返回引用可以节省不必要的开销，提高效率
&emsp;&emsp;这里的开销指的就是**临时对象的创建和销毁**，返回引用的话，其实使用的还是原来的 *this 的对象。如果是返回对象的话，需要创建一个临时对象，比如下面这样
```c++
#include<iostream>
using namespace std;

class A{


public:

    A(int a):x(a){

       cout <<"运行构造函数"<<endl;
    }

    A(const A & a){

       cout <<"运行拷贝构造函数"<<endl;

       this->x = a.x;

    }

    A operator=(const A & a)
    {
        cout <<"运行等号操作符函数"<<endl;
        this->x = a.x;
        return *this;
    }

    int value()
    {
        cout <<"x = "<<this->x<<endl;

    }

private:

    int x;

};


int main(){

  A A1(10);
  A A2(20);

  //这里使用了等号运算符重载,返回一个临时对象，使用了拷贝构造函数
  //现在这个临时对象并没有使用,临时对象会被销毁掉，这就是不必要的开销
  A2 = A1;
  A2.value();

  return 0;
}

/* 运行结果

    运行构造函数
    运行构造函数
    运行等号操作符函数
    运行拷贝构造函数
    x = 10
*/


//如果将上面的代码中的等号重载函数替换成下面的代码,则会减少执行一次拷贝构造函数
    A & operator=(const A & a)
    {
        cout <<"运行等号操作符函数"<<endl;
        this->x = a.x;
        return *this;
    }

/*
    运行构造函数
    运行构造函数
    运行等号操作符函数
    x = 10
*/

```

#### 使用返回引用可以保证运算符原始语义的清晰
&emsp;&emsp;文中举例一个例子，这里我直接复用a = b = c，和 （a = b） = c，前者的语义是c给b赋值，b再给a赋值，后者的语义是b先给a赋值，c再给a赋值。使用返回引用这两者都可以清晰的实现。但是使用返回对象则后者不能保证语义清晰，**(a = b)操作完成后，整个表达式返回了一个临时对象tmp，最后是 tmp = c，并没有实现c给a赋值的语义**。

```c++
#include<iostream>
using namespace std;

//#define li 1
#define li2 1


class A
{
    public:

            A(int a):x(a)
            {
              cout <<"运行构造函数"<<endl;
            }

            A(const A & a)
            {
              cout <<"运行拷贝构造函数"<<endl;
              this->x = a.x;
            }

            #ifdef li
            A operator=(const A & a)
            {
                cout <<"运行等号操作符函数"<<endl;
                this->x = a.x;
                return *this;
            }
            #endif


            #ifdef li2
            A & operator=(const A & a)
            {
                cout <<"运行等号操作符函数"<<endl;
                this->x = a.x;
                return *this;
            }
            #endif

            int value()
            {
                cout <<"x = "<<this->x<<endl;
            }

    private:
            int x;
};


int main()
{
    A A1(10);
    A A2(20);
    A A3(30);

    cout <<"================="<<endl;
    A1 = A2 = A3;
    A1.value();
    cout <<"================="<<endl;

    A A4(10);
    A A5(20);
    A A6(30);
    cout <<"================="<<endl;
    (A4 = A5) = A6;
    A4.value();
    return 0;
}

/*
   没有使用引用的结果
                    运行构造函数
                    运行构造函数
                    运行构造函数
                    =================
                    运行等号操作符函数
                    运行拷贝构造函数
                    运行等号操作符函数
                    运行拷贝构造函数
                    x = 30
                    =================
                    运行构造函数
                    运行构造函数
                    运行构造函数
                    =================
                    运行等号操作符函数
                    运行拷贝构造函数
                    运行等号操作符函数
                    运行拷贝构造函数
                    x = 20


    使用引用的结果
                    运行构造函数
                    运行构造函数
                    运行构造函数
                    =================
                    运行等号操作符函数
                    运行等号操作符函数
                    x = 30
                    =================
                    运行构造函数
                    运行构造函数
                    运行构造函数
                    =================
                    运行等号操作符函数
                    运行等号操作符函数
                    x = 30
 */

```
     

#### 关于C++中 前置++ 和  后置++ 的内部区别
&emsp;&emsp;这个问题我也是在看++运算符重载的时候才开始思考的，以前也是有一些区别，但是只是知道前置++比后置++的效率高，别的就不了解了。
&emsp;&emsp;思考的源头，为什么重载前置++ 返回的是引用，而重载后置++返回的是对象。

```c++
     A& operator++() 
     {//前置++
          ++a;
          return *this;
     }   
     

     A  operator++(int) 
     {//后置++
         A a = *this;
         ++*this;
         returnn a;
     }
```

> 参考文章 https://www.cnblogs.com/yuxingfirst/p/3258712.html

&emsp;&emsp;总结一下文章中的观点
1. 重载自定义的类运算符，但是含义要和C++内置的类型一样，也就是说运算符原来什么作用，你重载后就应该是什么作用
2. C++中内置的前++ 和 后++ 的内部机制,这两种方式都会使得i跟j自增1，不同的地方在于其内部实现; ++i的实现原理是现将i自增1，然后返回i的引用(我们知道重载操作符也是可以有返回值的),而i++的实现原理是先定义一个j的副本，然后在将j自增1，最后返回之前定义个那个副本的值。所以++的运算符重载也要按照这个机制去实现了