#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../customdatasize.h"
using namespace std;

void sighandler(int _sig){
    while(1){
        int pid = waitpid(-1,NULL,WNOHANG);
        if(pid <= 0){
            break;
        }
    }
}


int main(){

    signal(SIGCHLD,sighandler);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        cout <<"创建监听socket失败 返回值 : "<<listenfd<<endl;
    }

    sockaddr_in servaddr;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //开启socket选项
    int on = 1;
    int res;
    res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(res != 0){
        cout <<"设置监听socket失败 返回值 : "<<res<<endl;
    }

    //绑定socket
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    res =  bind(listenfd, (const sockaddr *)&servaddr, sizeof(servaddr));
    if(res != 0){
        cout <<"绑定监听socket失败 返回值 : "<<res<<endl;
    }
    
    //监听socket
    res = listen(listenfd,SOMAXCONN);
    if(res != 0){
        cout <<"listen() 调用失败，返回值 : "<<res<<endl;
    }

    vector<int> vecfd;         //套接字集合
    vecfd.push_back(listenfd); //现将监听套接字添加到 vecfd
    fd_set rset;               //读事件集合
    FD_ZERO(&rset);


    while(1){

        //每次调用select前都要重新设置文件描述符集合，因为事件发生后，集合都将被内核设置
        int maxfd = -1;
        for(int i = 0; i < vecfd.size(); i++){
            FD_SET(vecfd[i],&rset);
            if(vecfd[i] > maxfd){
                maxfd = vecfd[i];
            }
        }

        int count = select(maxfd+1,&rset,NULL,NULL,NULL);
        // cout<<"就绪文件描述符个数 count = "<<count<<endl;

        if(count == 0){
            continue;
        }else if(count == -1){
            cout <<__FILE__<<" "<<__LINE__<<" select() 调用失败"<<endl;
        }

        //处理监听socket上的可读事件
        if(FD_ISSET(listenfd,&rset)){

            if(vecfd.size() + 1 > 2){//代码逻辑限制，最多只能处理一个监听socket+1个连接socket
                /*
                     注意在已经有一个监听socket+1个连接socket时，再有一个客户端连接服务端则，监听socket会一直处于就绪状态
                     那么执行select得到的就绪文件描述符个数就会一直为1
                */
                // cout <<__FILE__<<" "<<__LINE__<<" 最多只能处理5个客户端连接,不再处理新的客户端连接"<<endl;
            }else{

                sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(sockaddr_in);

                //获取与客户端连接的socket
                int socketfd = accept(vecfd[0], (sockaddr *)&clientaddr, &clientlen);
                if(socketfd < 0){
                    cout <<__FILE__<<" "<<__LINE__<<" accept() 调用失败，返回值 : "<<socketfd<<endl;
                }else{
                    cout <<__FILE__<<" "<<__LINE__<<" 与新的客户端进行连接 socketfd = "<<socketfd<<endl;
                    vecfd.push_back(socketfd);//保存新连接的socket
                }

                if(count == 1){//只有一个文件描述符且是监听socket时，不需要再遍历vecfd
                   continue;
                }
            }
        }


        //for循环遍历剩余与客户端连接的socket
        for(int i = 1; i < vecfd.size(); i++){

            int socketfd = vecfd[i];

            if(FD_ISSET(socketfd,&rset)){//连接socket

                //首先接收数据字节个数的数据
                msgpackage msgpack; //信息包结构体
                int readnum = readex(socketfd,(char *)&msgpack.len,sizeof(msgpack.len));
                if(readnum <= 0){

                    cout <<__FILE__<<" "<<__LINE__<<" 从客户端获取数据长度错误 readnum = "<<readnum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = vecfd.begin();
                    while(iter != vecfd.end()){//删除断开连接的套接字
                        if(*iter == socketfd){
                            FD_CLR(vecfd[i],&rset);
                            vecfd.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<__FILE__<<" "<<__LINE__<<" 即将接收"<<ntohl(msgpack.len)<<"个字节数据"<<endl;
                }

                //然后是接收真正的数据
                int datasize = ntohl(msgpack.len);//注意整数需要转换字节序
                readnum = readex(socketfd,msgpack.data,datasize);
                if(readnum <= 0){

                    cout <<__FILE__<<" "<<__LINE__<<" 从客户端获取数据错误 readnum = "<<readnum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = vecfd.begin();
                    while(iter != vecfd.end()){//删除断开连接的套接字
                        if(*iter == socketfd){
                            FD_CLR(vecfd[i],&rset);
                            vecfd.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<__FILE__<<" "<<__LINE__<<" 从客户端接收数据 : "<<msgpack.data<<endl;
                }

                
                //即将回射数据给客户端


                //下面是将接收的数据再回射给客户端  先发送数据长度
                msgpack.len = strlen(msgpack.data);
                datasize = htonl(msgpack.len);
                int writenum = writeex(socketfd,(char *)&datasize,sizeof(msgpack.len));

                if(writenum <= 0){

                    cout <<__FILE__<<" "<<__LINE__<<" 向客户端发送数据长度错误 writenum = "<<writenum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = vecfd.begin();
                    while(iter != vecfd.end()){//删除断开连接的套接字
                        if(*iter == socketfd){
                            FD_CLR(vecfd[i],&rset);
                            vecfd.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;
                }else{
                    cout <<__FILE__<<" "<<__LINE__<<" 即将发送"<<msgpack.len<<"个字节数据 : "<<msgpack.data<<endl;
                }

                //向客户端发送数据
                writenum = writeex(socketfd,msgpack.data,msgpack.len);
                if(writenum <= 0){

                    cout <<__FILE__<<" "<<__LINE__<<" 向客户端发送数据错误"<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = vecfd.begin();
                    while(iter != vecfd.end()){//删除断开连接的套接字
                        if(*iter == socketfd){
                            FD_CLR(vecfd[i],&rset);
                            vecfd.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<__FILE__<<" "<<__LINE__<<" 向客户端发送数据 : "<<msgpack.data<<endl;
                }
            }
        }
    }

    for(int i = 0; i < vecfd.size(); i++){//关闭所有套接字
        FD_CLR(vecfd[i],&rset);
        shutdown(vecfd[i],SHUT_RDWR);
    }


    return 0;
}