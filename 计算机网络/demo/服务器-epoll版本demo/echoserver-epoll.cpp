//使用epoll 方式实现一个聊天室服务端
//[功能] 接收客户端数据，并把客户数据发送给每一个登录到该服务器上的客户端(数据发送者除外)

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>
#include <algorithm>
#include <string>
#include "../customdatasize.h"
using namespace std;

//聊天室中同时存在用户的最大值
#define MAX_CLIENTNUM 5

//存储活跃的文件描述符
epoll_event events[MAX_CLIENTNUM];

//应用缓存区最大字节数
#define MAX_BUFFSIZE 64

//在线用户的标识集合,标识就是客户端对应的连接socket
vector<int> clients;

int main()
{
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" socket()失败 返回值 : "<<listenfd<<endl;
    }

    sockaddr_in servaddr;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //开启socket选项
    int on = 1;
    int res;
    res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" setsockopt()失败 返回值 : "<<res<<endl;
    }

    //绑定套接字
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    res =  bind(listenfd, (const sockaddr *)&servaddr, sizeof(servaddr));
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" bind()失败 返回值 : "<<res<<endl;
    }
    
    //将listenfd套接字转换成监听状态,变成被动套接字,用于调用accept
    res = listen(listenfd,SOMAXCONN);
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" listen() 失败，返回值 : "<<res<<endl;
    }

    //在内核中创建事件表并获取其标识
    int epollfd = epoll_create(MAX_CLIENTNUM);
    if(epollfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" epoll_create() 失败，返回值 : "<<epollfd<< endl;
        return -1;
    }


    //添加监听socket
    epoll_event epoll_listen;
    epoll_listen.data.fd = listenfd;
    epoll_listen.events = EPOLLIN;
    res = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &epoll_listen);
    if(res < 0){
        cout <<__FILE__<<" "<<__LINE__<<" epoll_ctl(EPOLL_CTL_ADD)失败" << endl;
        return -1;
    }

    while(1){
        cout <<"======================================================================="<<endl;

        int count = epoll_wait(epollfd, events, MAX_CLIENTNUM, -1);
        cout <<__FILE__<<" "<<__LINE__<<"就绪文件描述符个数 = "<<count<<endl;

        for(int i = 0; i < count; i++){//这里遍历的是已经就绪的文件描述符，而不是遍历所有文件描述符

            if(events[i].data.fd == listenfd && events[i].events & EPOLLIN){//如果是监听socket就绪，说明有新的连接申请

                sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(sockaddr_in);

                int socketfd = accept(events[0].data.fd, (sockaddr *)&clientaddr, &clientlen);

                epoll_event epoll_listen;
                epoll_listen.data.fd = socketfd;
                epoll_listen.events = EPOLLIN;
                int res =  epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &epoll_listen);

                clients.push_back(socketfd);
                cout<<"有新用户上线，当前在线人数为:"<<clients.size()<<endl;
                
                continue;
            }
            
            if(events[i].events & EPOLLIN) {//其他连接socket就绪

                char data[MAX_BUFFSIZE] = {0};

                int socketfd = events[i].data.fd;//当前处理的连接对应的socketfd

                int readnum = readex(socketfd,data,MAX_BUFFSIZE);
                if(readnum <= 0){//读取数据时发生错误，或者对方断开连接

                    //断开socket连接
                    shutdown(socketfd, SHUT_RDWR);

                    //epoll不再监听
                    int res = epoll_ctl(epollfd, EPOLL_CTL_DEL, socketfd, &events[i]);
                    if(res < 0){
                        cout <<__FILE__<<" "<<__LINE__<<" epoll_ctl(EPOLL_CTL_DEL)失败" << endl;
                        return -1;
                    }

                    //在线连接socket中删除对应的描述符
                    auto iter = clients.begin();
                    while(iter != clients.end()){
                       if(*iter == socketfd){
                          clients.erase(iter);
                          break;
                       }
                       ++iter;
                    }

                    continue;
                }

                cout <<__FILE__<<" "<<__LINE__<<" 从客户端【"<<socketfd<<"】接收数据 : "<<data<<endl;
  
                //给数据增加新内容，标记是谁发送的
                string msg = "【" +to_string(socketfd) + "】";
                msg += data;

                for(int i = 0; i < clients.size(); ++i){

                    if(clients[i] == socketfd){//如果遍历的fd是发送者的，就不发消息给他
                        continue;
                    }else{
                        int writenum = writeex(clients[i],(char *)msg.data(),MAX_BUFFSIZE);

                        if(writenum <= 0){//写入数据时发生错误，或者对方断开连接

                            //断开socket连接
                            shutdown(clients[i], SHUT_RDWR);

                            //epoll不再监听
                            int res = epoll_ctl(epollfd, EPOLL_CTL_DEL, clients[i], &events[i]);
                            if(res < 0){
                                cout <<__FILE__<<" "<<__LINE__<<" epoll_ctl(EPOLL_CTL_DEL)失败" << endl;
                                return -1;
                            }

                            //在线连接socket中删除对应的描述符
                            auto iter = clients.begin();
                            while(iter != clients.end()){
                                if(*iter == clients[i]){
                                    clients.erase(iter);
                                    break;
                                }
                                ++iter;
                            }

                            continue;
                        }

                        cout <<__FILE__<<" "<<__LINE__<<" 向客户端【"<<clients[i]<<"】发送数据 : "<<data<<endl;
                    } 
                }
            }
        }
    }


    for(int i = 0; i < MAX_CLIENTNUM; i++){//关闭所有套接字
        shutdown(events[i].data.fd,SHUT_RDWR);
    }
    
    return 0;
}