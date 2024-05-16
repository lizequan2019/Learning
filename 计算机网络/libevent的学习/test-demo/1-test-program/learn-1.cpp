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
void (*LISTENER_CB) (struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);

//回调函数
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg){
     cout<<__FILE__<<"  "<<__LINE__<<endl;
}

int main(){
   
   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return 1;
   }

   cout<<"this is learn-1.cpp"<<endl;

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
   
   LISTENER_CB = listener_cb;
   evconnlistener * ev = evconnlistener_new_bind(base, 
                                                 LISTENER_CB, 
                                                 base, 
                                                 LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_THREADSAFE, 
                                                 1024, 
                                                 (struct sockaddr *)&addr, 
                                                 sizeof(struct sockaddr_in));
   
   if(ev){
      cout <<"evconnlistener_new_bind   call  ok"<<endl;
   }else{
      cout <<"evconnlistener_new_bind   call  error"<<endl;
   }

   //事件分发
   if(base){
       event_base_dispatch(base);
   }

   if(ev){
      evconnlistener_free(ev);
   }

   if(base){
       event_base_free(base);
   }
   return 0;
}
