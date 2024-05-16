#include <event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

//生成随机 用于随机耗时 时间范围[4-7]
unsigned getrand(){
    srand((unsigned int)time(NULL));
    unsigned randnum = rand()%3;
    return randnum+4;
}

//获取当前时间
void gettime(char * _buf, int _size){
    memset(_buf, 0, _size);

    time_t t;
    time(&t);
    ctime_r(&t,_buf);   
}


//定时器回调函数
void timer1_cb(evutil_socket_t _fd, short _what, void * _arg){
   
    //获取当前时间
    char buf[1024];
    gettime(buf,sizeof(buf));
   
    cout<<"进入 timer 1 回调函数  当前时间:"<<buf<<endl;

}

void timer2_cb(evutil_socket_t _fd, short _what, void * _arg){
   
    //获取当前时间
    char buf[1024];
    gettime(buf,sizeof(buf));
   
    cout<<"进入 timer 2 回调函数  当前时间:"<<buf<<endl;

}

void timer3_cb(evutil_socket_t _fd, short _what, void * _arg){
   
    //获取当前时间
    char buf[1024];
    gettime(buf,sizeof(buf));
   
    cout<<"进入 timer 3 回调函数  当前时间:"<<buf<<endl;
    cout<<"======================================================="<<endl;
}



int main(){
   
   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return 1;
   }

   cout<<"this is learn-4.cpp"<<endl;

   //监听端口
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(9999);

   //创建libevent的上下文
   event_base * base = event_base_new();
   if(base){
	    cout <<"event_base_new success"<<endl;
   }
   

   //使用event_new创建定时器
   event * ev_timer1 = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, timer1_cb, event_self_cbarg());
   timeval tim1 = {1,0};
   event_add(ev_timer1,&tim1);
      
   //evtimer_new是在event_new的基础上进行的封装
   event * ev_timer2 = evtimer_new(base, timer2_cb, event_self_cbarg());
   timeval tim2 = {3,0};
   evtimer_add(ev_timer2, &tim2);

   //定时器性能优化  默认event使用二叉堆存储 优化到使用双向队列
   event * ev_timer3 = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, timer3_cb, event_self_cbarg());
   static timeval tv_in = {4,0};
   const timeval * tim3;
   tim3 = event_base_init_common_timeout(base, &tv_in);
   event_add(ev_timer3, tim3);




   //事件分发
   if(base){
       event_base_dispatch(base);
   }

   if(base){
       event_base_free(base);
   }

   return 0;
}
