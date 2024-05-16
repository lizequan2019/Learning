//网络服务端

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(){

    int res;

    //创建一个socket变量
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    if(listenfd < 0){
        cout <<"创建监听套接字失败 返回值 : "<<listenfd<<endl;
    }

    //设置socket选项
    int on = 1;
    res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(res != 0){
        cout <<"设置套接字失败 返回值 : "<<res<<endl;
    }

    //设置监听的ip和端口
    sockaddr_in servaddr;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);

    //将监听socket与ip和端口绑定
    res =  bind(listenfd, (const sockaddr *)&servaddr, sizeof(servaddr));
    if(res != 0){
        cout <<"绑定监听套接字失败 返回值 : "<<res<<endl;
    }
    
    //创建一个监听队列以存放待处理的客户连接，将listenfd套接字转换成监听状态,变成被动套接字
    res = listen(listenfd,SOMAXCONN);
    if(res != 0){
        cout <<"listen 失败，返回值 : "<<res<<endl;
    }

    cout<<"暂停30s等待客户端退出"<<endl;
    for(int i = 0; i < 30; ++i){
         cout<<"等待"<<i<<"秒"<<endl;
         sleep(1);
    }
    

    //从listen监听队列中接收一个连接
    sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(sockaddr_in);
    //accept只是从监听队列中取出连接，而不关心任何网络状况的变化
    int socketfd = accept(listenfd, (sockaddr *)&clientaddr, &clientlen);//注意socketfd 是主动套接字
    if(socketfd < 0){
        cout <<"accept 失败，返回值 : "<<socketfd<<endl;
    }


    char data[1024] = {0}; //buffer全部置为0是必须的
    while (1){

       int readnum = recv(socketfd,data,sizeof(data),0);

       if(readnum <= 0){
          cout<<" 退出 "<<endl;
          break;
       }

       cout <<"从客户端获取"<<readnum<<"个字节数据 "<<data<<endl;

       send(socketfd,data,strlen(data),0);

       cout <<"向客户端发送"<<strlen(data)<<"个字节数据 "<<data<<endl;

       memset(data,0,sizeof(data));
    }

    //可以终止socket连接而非像close一样减少引用计数
    shutdown(socketfd,SHUT_RDWR);
    shutdown(listenfd,SHUT_RDWR);

    cout<<"服务器关闭"<<endl;

    return 0;
}