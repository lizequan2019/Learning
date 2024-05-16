//网络客户端

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
using namespace std;

int main(){

    //创建socket变量
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if(socketfd < 0){
        cout <<"创建链接套接字失败 返回值 : "<<socketfd<<endl;
    }

    //连接服务端
    sockaddr_in serv_addr;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    int res = connect(socketfd,(const sockaddr *)&serv_addr,sizeof(sockaddr_in));
    if(res != 0){
        cout <<"链接服务端失败 返回值 : "<<res<<endl;
    }


    char data[1024] = {0};//buffer全部置为0是必须的
    while (1){
       cout<<"请输入要发送的内容 : ";
       cin >> data;

       send(socketfd,data,1,0);
       send(socketfd,data+1,strlen(data)-1,0);
       cout <<"向服务器发送"<<strlen(data)<<"个字节数据 "<<data<<endl;

       memset(data,0,sizeof(data));

       int readnum = recv(socketfd,data,sizeof(data),0);
       if(readnum == 0){
          cout<<"服务端断开连接"<<endl;
          break;
       }
       else if(readnum < 0){
          cout<<"接收服务端数据错误"<<endl;
          break;
       }
       else{
          cout <<"从服务端获取"<<readnum<<"个字节数据 "<<data<<endl;
       }
       
       memset(data,0,sizeof(data));
    }

    shutdown(socketfd,SHUT_RDWR);

    cout<<"客户端关闭"<<endl;

    return 0;
}