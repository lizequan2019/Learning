#include  <iostream>

using namespace std;


class A
{
    public:

    A()
    {
        cout <<"执行A的构造函数"<<endl;
    }

    ~A()
    {
        cout <<"执行A的析构函数"<<endl;
    }
};


A GetA()
{
    A a;
    return a;
}



int main()
{
   cout <<"不使用常量引用接住临时变量======"<<endl;
   GetA();//执行A的构造函数和析构函数
   cout <<"使用常量引用接住临时变量========"<<endl;
   const A & a = GetA();//执行A的构造函数
   cout <<"==========end=========="<<endl;
   return 0;
}

/*
执行结果:
            不使用常量引用接住临时变量======
            执行A的构造函数
            执行A的析构函数

            使用常量引用接住临时变量========
            执行A的构造函数
            
            ==========end==========
            执行A的析构函数   //引用销毁时 临时对象才销毁
*/