#include<iostream>
#include<mutex>
#include<thread>

using namespace std;

//单例类
class Singleton {
    public :
             static Singleton * instance();
             static Singleton * pInstance ;
             static mutex m_mutex;
};


//单例类的部分实现
Singleton * Singleton :: pInstance = 0 ;
mutex Singleton::m_mutex;

Singleton * Singleton :: instance() 
{
    //通过新增加的判断来减少加锁操作
    if ( pInstance == 0 ) {

        //原本的写法
        lock_guard<mutex> lock(m_mutex);
        if ( pInstance == 0 ) {
            pInstance = new Singleton ;
        }

    }
    return pInstance ;
}

void thread_fun1(){
     Singleton * p  = Singleton::instance();
     cout <<"fun1 p = "<<p<<endl;
}

void thread_fun2(){
     Singleton * p  = Singleton::instance();
     cout <<"fun2 p = "<<p<<endl;
}

int main(){

    thread th1(thread_fun1);
    thread th2(thread_fun2);

    th1.join();
    th2.join();
    return 0;
}