//网络服务端 基于echoserver.cpp优化

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
using namespace std;


int main()
{

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    if(listenfd < 0)
    {
        cout <<"创建监听套接字失败 返回值 : "<<listenfd<<endl;
    }

    sockaddr_in servaddr;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //inet_aton("127.0.0.1",&servaddr.sin_addr);  设置ip的第二种写法

    int on = 1;
    int res;
    res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(res != 0)
    {
        cout <<"设置套接字失败 返回值 : "<<res<<endl;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    res =  bind(listenfd, (const sockaddr *)&servaddr, sizeof(servaddr));
    if(res != 0)
    {
        cout <<"绑定监听套接字失败 返回值 : "<<res<<endl;
    }
    
    //将listenfd套接字转换成监听状态,变成被动套接字,用于调用accept
    res = listen(listenfd,SOMAXCONN);
    if(res != 0)
    {
        cout <<"listen 失败，返回值 : "<<res<<endl;
    }

    int socketfd;

    while (1)
    {
        sockaddr_in clientaddr;
        socklen_t clientlen = sizeof(sockaddr_in);
        //注意socketfd 是主动套接字
        socketfd = accept(listenfd, (sockaddr *)&clientaddr, &clientlen);
        if(socketfd < 0)
        {
            cout <<"accept 失败，返回值 : "<<socketfd<<endl;
        }

        //创建一个子进程用于处理与客户端交互，主进程继续等待其他客户端连接
        int processid = fork();
        if(processid == -1)
        {
            break;
        }
        else if(processid == 0)
        {//子进程

            close(listenfd);//处理客户端交付不需要再使用监听socekt

            while (1)
            {
                cout<<"子进程 id = "<<getpid()<<"***********************************"<<endl;

                char data[30] = {0}; //buffer全部置为0是必须的

                int readnum = recv(socketfd,data,sizeof(data),0);
                if(readnum <= 0)
                {
                    cout <<"从客户端获取数据错误"<<endl;
                }
                else
                {
                    cout <<"从客户端获取"<<readnum<<"个字节数据 "<<data<<endl;
                }
                
                int writenum = send(socketfd,data,strlen(data),0);
                if(writenum < 0)
                {
                    cout <<"向客户端发送数据错误: "<<data<<endl;
                }
                else
                {
                    cout <<"向客户端发送"<<writenum<<"个字节数据 "<<data<<endl;
                }

                memset(data,0,sizeof(data));
            }

            exit(EXIT_SUCCESS); //与客户端关闭连接后子进程要退出
        }
        else
        {//主进程
            close(socketfd); //等待客户端连接不需要通讯socket
        }
    }

    close(socketfd);
    close(listenfd);
    
    return 0;
}