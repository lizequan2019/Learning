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


//资源回收函数
void Recycling(event * _ev, evutil_socket_t _socketfd){

   if(_ev == NULL){
      return;
   }
   event_free(_ev);//回收事件对象资源
   evutil_closesocket(_socketfd);//关闭客户端fd
}


//接收客户端消息回调函数 
void client_cb(evutil_socket_t _sockfd, short _what, void * _arg){

   //获取事件对象
   event * _ev = (event *)_arg;

   //判断是否为超时时间
   if(_what & EV_TIMEOUT){
       printf("[TimeOut]\n");
       Recycling(_ev,_sockfd);
       return;
   }

   //从客户端读取消息
   char buf[1024];
   memset(buf,0,sizeof(buf));  
   int readnum = recv(_sockfd, buf, sizeof(buf)-1, 0);
   
   if(readnum > 0){
       printf("[read msg] %s\n",buf);
   }else if(readnum == 0){//断开连接
       printf("[Disconnect]\n");
       Recycling(_ev,_sockfd);
   }else if(readnum < 0){//连接出现错误
       printf("[recv data error]\n");
       Recycling(_ev,_sockfd);
   }

   //向客户端发送消息
   int writenum = send(_sockfd, buf, strlen(buf)+1, 0);
   if(writenum > 0){
       printf("[send msg] %s\n",buf);
   }else if(readnum == 0){//断开连接
       printf("[Disconnect]\n");
       Recycling(_ev,_sockfd);
   }else if(readnum < 0){//连接出现错误
       printf("[send data error]\n");
       Recycling(_ev,_sockfd);
   }
}

//回调函数
void listener_cb(evutil_socket_t _sockfd, short _waht, void * _arg){

   event_base * _base = (event_base *)_arg;

   //获取客户端的socket资源
   sockaddr_in clientaddr;
   socklen_t clientlen = sizeof(sockaddr_in);
   int socketfd = accept(_sockfd, (sockaddr *)&clientaddr, &clientlen);

   //解析客户端ip
   char ip[20];
   memset(ip,0,sizeof(ip));
   inet_ntop(AF_INET, &clientaddr.sin_addr, ip, clientlen);

   printf("[connect] ip : %s\n",ip);

   event * ev = event_new(_base, socketfd, EV_READ|EV_PERSIST|EV_TIMEOUT, client_cb, event_self_cbarg());

   timeval timeout = {10,0};

   //添加事件对象
   event_add(ev,&timeout);
}

int main(){
   
   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return 1;
   }

   cout<<"this is learn-6.cpp server"<<endl;

   //监听端口
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(9999);

   int listenfd = socket(PF_INET, SOCK_STREAM, 0);
   evutil_make_socket_nonblocking(listenfd);
   evutil_make_listen_socket_reuseable(listenfd);

   ::bind(listenfd, (const sockaddr *)&addr, sizeof(addr));

   //将listenfd套接字转换成监听状态,变成被动套接字,用于调用accept
   listen(listenfd,SOMAXCONN);

   //创建上下文对象
   event_base * base = NULL;
   base = event_base_new();
   
   //创建事件对象
   event * ev = event_new(base, listenfd, EV_READ|EV_PERSIST, listener_cb, base);
   event_add(ev,NULL);

   //事件分发
   event_base_dispatch(base);

   //回收资源
   event_free(ev);
   event_base_free(base);   
   evutil_closesocket(listenfd);//关闭客户端fd

   return 0;
}
