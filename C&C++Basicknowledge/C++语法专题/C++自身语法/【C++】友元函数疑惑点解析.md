### 关于友元知识点

&emsp;&emsp;主要是为了面试才看的，平时工作都用不到友元的知识点。


### 友元声明
&emsp;&emsp;A类中声明B类是它的友元类，那么B类中就可以使用A类对象访问A类中所有的成员(共有私有权限的都可以)。注意被friend关键字修饰的类或函数就叫做友元类(友元函数)。

```c
//举例说明
            #include <cstdarg>
            #include <iostream>
            using namespace std;
            
            class TestA{
                friend class TestB;     //此处声明 TestB 是 TestA的友元类

                private:
                    int a;
                    void fun(){
                        cout <<"hello , i am TestA"<<endl;
                    }       

                public:
                    void setValue(int _value){
                        a = _value;
                    }      
            };
            
            class TestB{
                public:	
                        //以下几个函数都可以使用A类对象访问 私有或共有成员变量和成员函数
                        void SetData(TestA & _testA, int _value){
                            _testA.a = _value;
                        }

                        void UseASetValue(TestA & _testA, int _value){
                            _testA.setValue(_value);
                        }

                        int  GetData(TestA & _testA){
                            return _testA.a;
                        }

                        void introduction(TestA & _testA){
                            _testA.fun();
                        }
            };
            
            int main(){
                TestB B;
                TestA A;

                B.SetData(A, 10);
                cout <<"a =  "<<B.GetData(A)<<endl;

                B.introduction(A);

                B.UseASetValue(A, 20);
                cout <<"a = "<<B.GetData(A)<<endl;
                return 0;
            }

```

### 友元类和原始类的声明顺序:

&emsp;&emsp;一个类充当其他的类的友元的情况 : 没有固定顺序，友元类和原始类谁先谁后都行。

&emsp;&emsp;**一个类的成员函数充当其他类的友元的情况 : 先前置声明原始类，然后声明友元类，接着声明和实现原始类，最后实现友元类。**


### 友元类横向和纵向的关系

&emsp;&emsp;横向关系是指友元关系的传递，朋友的朋友不是我的朋友。纵向关系是指派生类是否能继承友元关系，我的朋友不是我子女的朋友。


### 习题

#### 例题1
```c

// 友元函数需要通过对象或指针调用 这个说法对不对？？？

// 代码验证
            #include <climits>
            #include <iostream>
            #include <vector>
            #include <limits.h>
            using namespace std;

            class A
            {
                friend void fun(A & a);

                public:
                        void setvalue(int value)
                        {
                            _a = value;
                        }
                private:
                        int _a;
            };

            void fun(A & a)
            {//友元函数
                cout<<" this is fun() a ="<<a._a<<endl;
            }

            int main()
            {
                A a;
                a.setvalue(10);
                
                fun(a);    //友元函数可以直接被调用，不需要对象和指针，上述说法错误

                return 0;
            }
```
