#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <cstring>

#include <thread>
#include <iostream>

using namespace std;


//自定义结构体，用于向libevent回调函数传递的参数
typedef struct CALL_ARG{
	 struct bufferevent* _bev;  
}call_arg;


//读取缓冲区时需要调用的回调函数
void client_read_cb(struct bufferevent * _bev, void * _arg){
    printf("进入 client_read_cb\n");
    char _buf[1024] = {0}; 
    bufferevent_read(_bev, _buf, sizeof(_buf));
    printf("[从服务端接收数据]: %s\n", _buf);
	printf("离开 client_read_cb\n\n");
}

//向缓冲区写数据时需要调用的回调函数【目前不使用】
void client_write_cb(struct bufferevent * _bev, void * _arg){
	printf("进入 client_write_cb\n");
	printf("离开 client_write_cb\n\n");
}

//struct bufferevent对应的fd发生事件时会调用的回调函数
void client_event_cb(struct bufferevent * _bev, short _events, void * _arg){
    if(_events & BEV_EVENT_CONNECTED){
        printf("已经连接服务器...\\(^o^)/...\n");
    }
}


//event_new创建的事件对应的回调函数
void event_new_cb(evutil_socket_t _fd, short _what, void * _arg){
	//读数据
    char _buf[1024] = {0};
    int  _len = read(_fd, _buf, sizeof(_buf));
	
    //发送数据
	call_arg _strarg = *(call_arg *)_arg;
	struct bufferevent * _bev = _strarg._bev;
	bufferevent_write(_bev, _buf, _len);
}
 
 
 
int main(int argc, const char* argv[])
{
	printf("this is learn-8 client\n"); 
	
    if(argc < 2){
        printf("请输入服务端IP\n");
        return 0;
    }

    //创建libevent上下文对象
    struct event_base* base = nullptr;
    base = event_base_new();
 
    //创建bufferevent对象 
    struct bufferevent* bev;
    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	
    //连接服务器
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9999);
    evutil_inet_pton(AF_INET, argv[1], &serv.sin_addr.s_addr);
	
	bufferevent_socket_connect(bev, (struct sockaddr*)&serv, sizeof(serv));

	//设置回调函数   
    bufferevent_setcb(bev, client_read_cb, client_write_cb, client_event_cb, NULL);
	
    //使能bufferevent的回调函数
    int ret = bufferevent_enable(bev, EV_READ | EV_WRITE);

    //创建一个事件,绑定标准输入,获取发送数据
	call_arg _arg;
	_arg._bev = bev;
    struct event* ev = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, event_new_cb, &_arg);
    
    //添加一个事件
    event_add(ev, NULL);
	  
    //开启事件循环
    event_base_dispatch(base);
	
    return 0;
}
