#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "../customdatasize.h"
using namespace std;

int main(){

    int socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if(socketfd < 0){
        cout <<__FILE__<<" "<<__LINE__<<" 创建连接套接字失败 返回值 : "<<socketfd<<endl;
    }


    sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    int res = connect(socketfd,(const sockaddr *)&addr,sizeof(sockaddr_in));
    if(res != 0){
        cout <<__FILE__<<" "<<__LINE__<<" 连接服务端失败 返回值 : "<<res<<endl;
    }


    fd_set rset; //可读文件描述符数组
    fd_set wset; //可写文件描述符数组
    bool bltips = true; //是否输出写入内容提醒标志

    int maxfd = max(fileno(stdout),socketfd);
    int datasize = 0;
    msgpackage msgpack; //信息包结构体


    while (1){
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        //每次调用select前都要重新设置文件描述符集合，因为事件发生后，集合都将被内核设置
        FD_SET(fileno(stdin),&rset);
        FD_SET(fileno(stdout),&wset);
        FD_SET(socketfd,&rset);

        //这里不设置带外数组，并且设置select为阻塞
        int count = select(maxfd+1,&rset,&wset,NULL,NULL); 

        if(count == 0){
            continue;
        }
        else if(count == -1){
            cout <<__FILE__<<" "<<__LINE__<<" select() 调用错误"<<endl;
        }

        //处理socketfd上的可读事件
        if(FD_ISSET(socketfd,&rset)){
            cout <<__FILE__<<" "<<__LINE__<<"=============获取服务端发送的数据============="<<endl;

            //接收服务端回射数据
            int readnum = readex(socketfd,(char *)&msgpack.len,sizeof(msgpack.len));
            if(readnum <= 0){
                cout <<__FILE__<<" "<<__LINE__<<" 从服务端获取数据长度错误 readnum = "<<readnum<<endl;
                break;
            }else{
                cout <<__FILE__<<" "<<__LINE__<<" 即将接收"<<ntohl(msgpack.len)<<"个字节数据"<<endl;
            }

            //然后是接收真正的数据
            datasize = ntohl(msgpack.len);//注意整数需要转换字节序
            readnum = readex(socketfd,msgpack.data,datasize);
            if(readnum <= 0){
                cout <<__FILE__<<" "<<__LINE__<<" 从服务端获取数据错误 readnum = "<<readnum<<endl;
                break;
            }else{
                cout <<__FILE__<<" "<<__LINE__<<" 从服务端接收数据 : "<<msgpack.data<<endl;
            }

            memset(&msgpack,0,sizeof(msgpack));
            bltips = true;
        }

        if(bltips){//处理标准输出可写事件,主要就是为了输出提示内容
            if(FD_ISSET(fileno(stdout),&wset)){      
                cout <<__FILE__<<" "<<__LINE__<<" 请写入内容:"<<endl;
                bltips = false;   
            }
        }

        //处理标准输入可读事件
        if(FD_ISSET(fileno(stdin),&rset)){

            cin >> msgpack.data;

            //首先是发送数据长度到服务端
            msgpack.len = strlen(msgpack.data);
            datasize = htonl(msgpack.len);
            int writenum = writeex(socketfd,(char *)&datasize,sizeof(msgpack.len));
            if(writenum <= 0){
                cout <<__FILE__<<" "<<__LINE__<<" 向服务端发送数据长度错误 writenum = "<<writenum<<endl;
                break;
            }else{
                cout <<__FILE__<<" "<<__LINE__<<" 即将发送"<<msgpack.len<<"个字节数据 : "<<msgpack.data<<endl;
            }

            writenum = writeex(socketfd,msgpack.data,msgpack.len);
            if(writenum <= 0){
                cout <<__FILE__<<" "<<__LINE__<<" 向服务端发送数据错误 writenum ="<<writenum<<endl;
                break;
            }else{
                cout <<__FILE__<<" "<<__LINE__<<" 向服务端发送数据 : "<<msgpack.data<<endl;
            }

            memset(&msgpack,0,sizeof(msgpack));
        }
    }

    FD_ZERO(&rset);
    shutdown(socketfd,SHUT_RDWR);
    
    return 0;
}