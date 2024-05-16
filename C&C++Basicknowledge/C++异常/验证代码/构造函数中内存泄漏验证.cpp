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
                    cout<<"Outer(int value) \n";
                    inner2 = new Inner(2);
                    throw 1;
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
    void* operator new(size_t size)    // 构造Outer执行重载的new运算符
    Inner()                            // 执行Inner构造函数
    Outer(int value)                   // 执行Outer构造函数
    Inner(int value)                   // 执行inner2 = new Inner(2);
    ~Inner1()                          // 抛出异常 清除栈上的对象 Inner1
    void* operator delete(void* ptr)   // 析构Outer对象
    got it                             // 接收异常

    Inner* inner2; 对象并没有被析构，导致内存泄漏


    ==26935== Memcheck, a memory error detector
    ==26935== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
    ==26935== Using Valgrind-3.14.0 and LibVEX; rerun with -h for copyright info
    ==26935== Command: ./a.out
    ==26935==
    void* operator new(size_t size)
    Inner()
    Outer(int value)
    Inner(int value)
    ~Inner1()
    void* operator delete(void* ptr)
    got it
    ==26935==
    ==26935== HEAP SUMMARY:
    ==26935==     in use at exit: 4 bytes in 1 blocks
    ==26935==   total heap usage: 5 allocs, 4 frees, 73,880 bytes allocated
    ==26935==
    ==26935== LEAK SUMMARY:
    ==26935==    definitely lost: 4 bytes in 1 blocks
    ==26935==    indirectly lost: 0 bytes in 0 blocks
    ==26935==      possibly lost: 0 bytes in 0 blocks
    ==26935==    still reachable: 0 bytes in 0 blocks
    ==26935==         suppressed: 0 bytes in 0 blocks
    ==26935== Rerun with --leak-check=full to see details of leaked memory
    ==26935==
    ==26935== For counts of detected and suppressed errors, rerun with: -v
    ==26935== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
 */