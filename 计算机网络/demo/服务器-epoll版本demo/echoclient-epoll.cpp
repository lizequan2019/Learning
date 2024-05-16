//epoll机制实现的聊天室网络客户端
//功能:(1)从标准输入终端读取用户数据,并将用户数据发送至服务器
//    (2)往标准输出终端打印服务器发送给它的数据


//客户端和服务端默认发送和接收MAX_DATASIZE大小的数据


// 用于socketfd的读取写入 用 recv/send
// 其他文件描述符的就用 read/write

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <iostream>
using namespace std;

#define MAX_EVENTNUM 2
#define MAX_DATASIZE 64

int main(){

    //创建socket变量
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if(socketfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" socket()失败 返回值 : "<<socketfd<<endl;
        return -1;
    }

    //连接服务端
    sockaddr_in serv_addr;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    int res = connect(socketfd,(const sockaddr *)&serv_addr,sizeof(sockaddr_in));
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" connect()失败 返回值 : "<<res<<endl;
        shutdown(socketfd,SHUT_RDWR);
        return -1;
    }


    //需要监测的文件描述符,客户端只需要监测标准输入和连接socketfd即可
    epoll_event events[MAX_EVENTNUM];
    events[0].data.fd = STDIN_FILENO;
    events[0].events = EPOLLIN;

    events[1].data.fd = socketfd;
    events[1].events = EPOLLIN;

    //在内核中创建事件表并获取其标识
    int epollfd = epoll_create(MAX_EVENTNUM);
    if(epollfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" epoll_create() 失败，返回值 : "<<epollfd<< endl;
        shutdown(socketfd,SHUT_RDWR);
        return -1;
    }

    //添加要监测的文件描述符
    for(int i = 0; i < MAX_EVENTNUM; ++i){
      res = epoll_ctl(epollfd, EPOLL_CTL_ADD, events[i].data.fd, &events[i]);
      if(res < 0){
         cout <<__FILE__<<" "<<__LINE__<<" epoll_ctl(EPOLL_CTL_ADD)失败" << endl;
         shutdown(socketfd,SHUT_RDWR);
         return -1;
      }
    }

    while (1){

      char data[MAX_DATASIZE] = {0};

      int count = epoll_wait(epollfd, events, MAX_EVENTNUM, -1);

      for(int i = 0; i < count; ++i){
         if(events[i].data.fd == STDIN_FILENO){//标准输入描述符就绪 读取的不是网络socketfd所以使用read/write进行读写
               if(events[i].events & EPOLLIN){
                  int readnum = read(events[i].data.fd, data, MAX_DATASIZE); //从标准输入中读取数据

                  int writenum = write(socketfd, data, MAX_DATASIZE);         //发送给服务端
                  if(writenum <= 0){//写入数据时发生错误，或者对方断开连接
                       cout <<__FILE__<<" "<<__LINE__<<"向服务器写入数据错误，或者服务器已经断开连接"<<endl;
                       return -1;
                  }
               }
         }else if(events[i].data.fd == socketfd){//连接socket就绪

               if(events[i].events & EPOLLIN){
                  int readnum = recv(events[i].data.fd, data, MAX_DATASIZE, 0);  //接收服务端发送的数据
                  if(readnum <= 0){//读取数据时发生错误，或者对方断开连接
                       cout <<__FILE__<<" "<<__LINE__<<"向服务器读取数据错误，或者服务器已经断开连接"<<endl;
                       shutdown(socketfd,SHUT_RDWR);
                       return -1;
                  }
                  cout<<data<<endl;
               }              
         }
      }
    }

    shutdown(socketfd,SHUT_RDWR);

    cout<<"客户端关闭"<<endl;

    return 0;
}