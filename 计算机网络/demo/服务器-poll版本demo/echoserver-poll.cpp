//网络服务端 
//网络客户端可以使用select版本的

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>
#include <poll.h>
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

//poll()函数需要使用的结构体数组
vector<pollfd> pollfdary;  

int main()
{
    signal(SIGCHLD,sighandler);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" socket() 失败 返回值 : "<<listenfd<<endl;
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
        cout <<__FILE__<<" "<<__LINE__<<" bind() 失败 返回值 : "<<res<<endl;
    }
    
    //将listenfd套接字转换成监听状态,变成被动套接字,用于调用accept
    res = listen(listenfd,SOMAXCONN);
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" listen() 失败，返回值 : "<<res<<endl;
    }

    
    //首先将监听socketfd添加到数组中
    pollfd listenpollfd;
    listenpollfd.fd = listenfd;
    listenpollfd.events = POLL_IN;
    listenpollfd.revents = 0;
    pollfdary.push_back(listenpollfd); 



    while(1){
        //执行poll函数检测 每次都需要重新设置要检测的文件描述符
        int count = poll(&pollfdary[0],pollfdary.size(),-1);

        if(count == -1){
            cout <<__FILE__<<" "<<__LINE__<<" poll() 失败"<<endl;
            continue;
        }

        //判断监听socketfd是否可读
        if(pollfdary[0].revents & POLLIN){

            sockaddr_in clientaddr;
            socklen_t clientlen = sizeof(sockaddr_in);

            int socketfd = accept(pollfdary[0].fd, (sockaddr *)&clientaddr, &clientlen);

            if(socketfd < 0){

                cout <<__FILE__<<" "<<__LINE__<<" accept() 失败，返回值 : "<<socketfd<<endl;
            }else{

                cout <<__FILE__<<" "<<__LINE__<<" 添加新的连接socketfd"<<endl;
                pollfd pollfdtemp;
                memset(&pollfdtemp, 0, sizeof(pollfd));
                pollfdtemp.fd = socketfd;
                pollfdtemp.events = POLL_IN;
                pollfdary.push_back(pollfdtemp);   
            }
        }


        //检测所有的连接socketfd是否可读
        for(int i = 1; i < pollfdary.size(); i++){
            msgpackage msgpack; //信息包结构体

            if(pollfdary[i].revents & POLL_IN){

                cout<<"与连接socketfd = "<<pollfdary[i].fd<<"进行通讯"<<endl;
                int socketfd = pollfdary[i].fd;
                
                //首先接收数据字节个数的数据
                int readnum = readex(socketfd,(char *)&msgpack.len,sizeof(msgpack.len));
                if(readnum <= 0){
                    
                    cout <<__FILE__<<" "<<__LINE__<<" 从客户端获取数据长度错误 readnum = "<<readnum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = pollfdary.begin();
                    while(iter != pollfdary.end()){//删除断开连接的套接字
                        if((*iter).fd == socketfd){
                            pollfdary.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<"即将从客户端接收"<<ntohl(msgpack.len)<<"个字节数据"<<endl;
                }



                //然后是接收真正的数据
                int datasize = ntohl(msgpack.len);//注意整数需要转换字节序
                readnum = readex(socketfd,msgpack.data,datasize);
                if(readnum <= 0){
                    cout <<__FILE__<<" "<<__LINE__<<" 从客户端获取数据长度错误 readnum = "<<readnum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = pollfdary.begin();
                    while(iter != pollfdary.end()){//删除断开连接的套接字
                        if((*iter).fd == socketfd){
                            pollfdary.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<"从客户端接收数据 : "<<msgpack.data<<endl;
                }

                
                cout<<"====即将回射数据给客户端===="<<endl;


                //下面是将接收的数据再回射给客户端  先发送数据长度
                msgpack.len = strlen(msgpack.data);
                datasize = htonl(msgpack.len);
                int writenum = writeex(socketfd,(char *)&datasize,sizeof(msgpack.len));
                if(writenum <= 0){
                    cout <<__FILE__<<" "<<__LINE__<<" 向客户端发送数据长度错误 writenum = "<<writenum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = pollfdary.begin();
                    while(iter != pollfdary.end()){//删除断开连接的套接字
                        if((*iter).fd == socketfd){
                            pollfdary.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<"即将向客户端发送"<<msgpack.len<<"个字节数据 : "<<msgpack.data<<endl;
                }

                //向客户端发送数据
                writenum = writeex(socketfd,msgpack.data,msgpack.len);
                if(writenum <= 0){
                    cout <<__FILE__<<" "<<__LINE__<<" 向客户端发送数据长度错误 writenum = "<<writenum<<endl;

                    int res = shutdown(socketfd,SHUT_RDWR);
                    cout <<__FILE__<<" "<<__LINE__<<" 关闭与客户端的连接 res = "<<res<<endl;

                    auto iter = pollfdary.begin();
                    while(iter != pollfdary.end()){//删除断开连接的套接字
                        if((*iter).fd == socketfd){
                            pollfdary.erase(iter);
                            break;
                        }
                        iter++;
                    }
                    continue;

                }else{
                    cout <<"向客户端发送数据 : "<<msgpack.data<<endl;
                }
            }
        }
    }

    for(int i = 0; i < pollfdary.size(); i++)
    {//关闭所有套接字
        shutdown,(pollfdary[i].fd,SHUT_RDWR);
    }
    
    return 0;
}