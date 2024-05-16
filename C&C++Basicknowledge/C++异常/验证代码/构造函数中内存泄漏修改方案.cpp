#include <iostream>
using namespace std;

class Inner
{
    private:
                int m_Value;
    public:
                Inner()
                {
                    m_Value=1;
                    cout<<"Inner() \n";
                }

                Inner(int value):m_Value(value)
                {
                    cout<<"Inner(int value) \n";
                }

                ~Inner()
                {
                    cout<<"~Inner"<<m_Value<<"() \n";
                }
};

 

class Outer
{
    private:
                int m_Value;
                Inner inner1;
                Inner* inner2;
    public:
                Outer()
                {
                    cout<<"Outer() \n";
                }

                Outer(int value):m_Value(value)
                {
                    try{
                        cout<<"Outer(int value) \n";
                        inner2 = new Inner(2);       //在构造函数中就使用try catch，方式内存泄漏
                        throw 1;
                    }
                    catch(...)
                    {
                        delete inner2;
                        throw; 
                    }
                }

                ~Outer()
                {
                    cout<<"~Outer() \n";
                }

                void* operator new(size_t size)
                {
                    cout<<"void* operator new(size_t size) \n";
                    unsigned char* tmp = ::new unsigned char[size];
                    return tmp;
                }

                void operator delete(void* ptr)
                {
                    cout<<"void* operator delete(void* ptr) \n";
                    ::delete [] (unsigned char*)ptr;
                }
};

 
int main()
{
    try
    {
        Outer* d = new Outer(1);
    }

    catch(...)
    {
        cout<<"got it \n";
    }

    return 0;
}

/*
        void* operator new(size_t size)
        Inner()
        Outer(int value)
        Inner(int value)
        ~Inner2()         //抛出异常时被析构
        ~Inner1()
        void* operator delete(void* ptr)
        got it  i =1      //接收到抛出异常的信息
 */