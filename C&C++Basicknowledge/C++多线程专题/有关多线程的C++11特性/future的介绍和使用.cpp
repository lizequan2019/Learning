/*

    ****来自cpluplus的说明****

    A future is an object that can retrieve a value from some provider object or function, properly synchronizing this access if in different threads.

    future是一个可以从某个提供程序对象或函数中检索数据的对象，如果在不同的线程中，则需要正确地同步此访问。





    "Valid" futures are future objects associated to a shared state, and are constructed by calling one of the following functions:

    一个关联着共享状态的future被视作是一个有效的future对象，并且可以通过调用以下几个函数进行构造future
    (1) async
    (2) promise::get_future
    (3) packaged_task::get_future





    future objects are only useful when they are valid. Default-constructed future objects are not valid (unless move-assigned a valid future).

    future对象仅在有效时能被使用。使用future默认构造函数创建的对象是无效的(除非通过移动赋值操作将一个有效future对象赋值给无效的future对象)。






    Calling future::get on a valid future blocks the thread until the provider makes the shared state ready (either by setting a value or an exception to it).This way, two threads can be synchronized by one waiting for the other to set a value.

    有效的futrue对象调用future::get函数会阻塞当前线程直到数据提供者使共享状态就绪(这个共享状态可以是一个数据也可能是一个异常)
    通过这种方式，两个线程可以通过一个线程等待另一个线程设置值进行同步。




    The lifetime of the shared state lasts at least until the last object with which it is associated releases it or is destroyed.Therefore, if associated to a future, the shared state can survive the object from which it was obtained in the first place (if any).

    共享状态的生存期至少持续到与其关联的最后一个对象释放或销毁为止。因此，如果共享状态与一个future对象相关联，它可以在最初获得future对象中存活下来。


    ****共享future和独占future****
    (1) shared_future<bool>  fut;  共享性future  多次get,future也不会失效，依旧可以获取关联到线程的数据
    (2) future<bool> fut;          独享性future  只能get一次，之后future就会失效，因此要注意判断future.valid()


    ****注意事项**** 
    这里可以看出其实future是一个模板，当模板参数是void时，说明此时并没有关联的数据。
    还需要注意的是future本身需要互斥量或其他同步方式进行保护
*/



#include <iostream>       
#include <future>         
#include <chrono>        
#include <unistd.h>

using namespace std;

//共享future对象
future<bool>  fut;  
                  
//互斥量
mutex m;

//线程函数 判断入参是否是质数
bool thread_is_prime (int x) {

    sleep(2); //延时阻塞一下

    cout<<"开始计算"<<endl;
    if(x < 0){//不符合条件  抛出异常
        throw("[thread_is_prime] param is < 0");
    }

    for (int i=2; i<x; ++i) {
        if (x%i==0) {
            return false;
        }
    }
    
    cout<<"计算完毕"<<endl;
    return true;
}


void thread_fun(int _value){

    lock_guard<mutex> lk(m);
    cout<<__LINE__<<endl;    
    
    if(fut.valid()){ // 这里没有阻塞，下面的fut.get()会阻塞直至异步函数thread_is_prime执行完毕
                     // 只要future关联一个共享状态就视为有效

        cout<<__LINE__<<endl;
        try{
           //这里使用 try catch 捕获future对象携带的抛出异常
           cout <<__FUNCTION__<<" "<<_value << (fut.get()?" is":" is not") << " prime"<<endl;
        }
        catch(const char * a){
            cout <<"!!!!!!!get exception!!!!!!!!!!!"<<endl;
            cout <<"a = "<<a<<endl;
        }
    }
    cout<<__LINE__<<endl;
}


int main (){

    int value = 8;
    //异步函数执行使用async将函数的执行结果与fut绑定
    fut = std::async (thread_is_prime,value); 
    
    //模拟其他耗时操作 在此时 thread_is_prime 正在异步的执行中
    for(int i = 0; i < 5; ++i){
        cout<<"耗时操作"<<endl;
        sleep(1);
    }

    //创建线程
    thread th3(thread_fun,value);
    th3.join();

    cout <<"=====end======"<<endl;
    return 0;
}