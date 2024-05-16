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

//函数指针
// void (*LISTENER_CB) (struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);

//回调函数
//void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg){
//     cout<<__FILE__<<"  "<<__LINE__<<endl;
//}

//处理ctrl-c 信号的回调函数
void DealCtrl_C(int fd, short ev_waht, void * arg){
   cout << " Ctrl C " << endl;
}

//处理kill信号的回调函数 也就是kill进程可以进入此回调函数 但是kill -9 还是会杀死进程
void DealKill(int fd, short ev_waht, void * arg){
   cout << " Kill " << endl;

   event * _sig = (event *)arg;
   if(!event_pending(_sig, EV_SIGNAL, NULL)){//当信号事件处于未决状态时,
       if(event_add(_sig,NULL) != 0){//将事件加入到任务表中，这样算作根据条件来决定事件是否持久化，而非单纯使用EV_PERSIST实现持久化
          cout<<"event_add error"<<endl;
       }else{
          cout<<"kill事件被加入"<<endl;
       }
   }
}

int main(){
   
   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return 1;
   }

   cout<<"this is learn-3.cpp"<<endl;

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
   
   // LISTENER_CB = listener_cb;
   // evconnlistener * ev = evconnlistener_new_bind(base, 
   //                                               LISTENER_CB, 
   //                                               base, 
   //                                               LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_THREADSAFE, 
   //                                               1024, 
   //                                               (struct sockaddr *)&addr, 
   //                                               sizeof(struct sockaddr_in));
   
   // if(ev){
   //    cout <<"evconnlistener_new_bind   call  ok"<<endl;
   // }else{
   //    cout <<"evconnlistener_new_bind   call  error"<<endl;
   // }


   //control+c
   event * ev_sig = evsignal_new(base,SIGINT,DealCtrl_C,base);
   if(!ev_sig){
        cout<<"evsignal_new error"<<endl;
   }else{
        if(event_add(ev_sig,NULL)  != 0){
            cout<<"event_add error"<<endl;
        }
   }

   event * ksig = event_new(base,SIGTERM,EV_SIGNAL,DealKill,event_self_cbarg());
   if(!ksig){
        cout<<"evsignal_new error"<<endl;
   }else{
        if(event_add(ksig,NULL)  != 0){
            cout<<"event_add error"<<endl;
        }
   }

   //事件分发
   if(base){
       event_base_dispatch(base);
   }

   // if(ev){
   //    evconnlistener_free(ev);
   // }

   if(base){
       event_base_free(base);
   }

   if(ev_sig){
      if(event_del(ev_sig)){
         event_free(ev_sig);
      }else{
         cout <<"event_del ev_sig error"<< endl;
      }
   }

   if(ksig){
      if(event_del(ksig)){
         event_free(ksig);
      }else{
         cout <<"event_del ksig error"<< endl;
      }
   }

   return 0;
}
