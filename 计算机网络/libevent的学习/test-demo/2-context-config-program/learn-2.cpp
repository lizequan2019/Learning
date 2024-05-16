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

   cout<<"this is learn-2.cpp"<<endl;

   //监听端口
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(9999);

   //以上属于是固定部分

   
   event_base * base = NULL;
   //创建上下文配置对象
   event_config *conf = event_config_new();
   if(!conf){
      cout<<"event_config_new error"<<endl;
   }

   //设置选择使用的内核消息通知机制，这里是反向选择
   event_config_avoid_method(conf,"epoll");
   event_config_avoid_method(conf,"poll");

   //获取支持的消息通知机制
   const char ** methods = event_get_supported_methods();
   for(int i = 0; methods[i] != NULL; i++){
      cout<<"supported method["<<i+1<<"] = "<<methods[i]<<endl;
   }

   //设置功能特征  EV_FEATURE_FDS 还可以设置其他的
   //设置完        要确认一下其他特征是否受到影响
   //系统不一样    支持的特征也不一样 
   if(event_config_require_features(conf,EV_FEATURE_FDS) == 0){
        cout<<"event_config_require_features  success"<<endl;
   }


   if(conf){
      //创建libevent的上下文
      base = event_base_new_with_config(conf);
   }

   //及时删除上下文配置对象 conf
   event_config_free(conf);

   if(base){
	    cout <<"event_base_new_with_config success"<<endl;
   }else{
       cout <<"event_base_new_with_config error"<<endl;

       //自己配置的选项可能会出错，此时使用默认配置创建上下文对象补救一下
       base = event_base_new();
       if(!base){
         cout <<"event_base_new error"<<endl;
         return -1;
       }
   }

   cout<<"get current method = "<< event_base_get_method(base)<<endl;

   int feature_code = event_base_get_features(base);
   if(feature_code & EV_FEATURE_ET){
      cout<<"EV_FEATURE_ET is supported"<<endl;
   }else{
      cout<<"EV_FEATURE_ET is not supported"<<endl;
   }
   if(feature_code & EV_FEATURE_O1){
      cout<<"EV_FEATURE_O1 is supported"<<endl;
   }else{
      cout<<"EV_FEATURE_O1 is not supported"<<endl;
   }
   if(feature_code & EV_FEATURE_FDS){
      cout<<"EV_FEATURE_FDS is supported"<<endl;
   }else{
      cout<<"EV_FEATURE_FDS is not supported"<<endl;
   }
   if(feature_code & EV_FEATURE_EARLY_CLOSE){
      cout<<"EV_FEATURE_EARLY_CLOSE is supported"<<endl;
   }else{
      cout<<"EV_FEATURE_EARLY_CLOSE is not supported"<<endl;
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
