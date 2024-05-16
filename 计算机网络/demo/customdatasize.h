#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
using namespace std;


const int const_idatasize = 1024;

//数据包结构体
struct MSGPACKAGE
{
    int len; //数据长度
    char data[const_idatasize];

    MSGPACKAGE(){
        len = 0;
        memset(data,0,sizeof(data));
    }
};

typedef MSGPACKAGE msgpackage;


/*
   功能 : 从socketfd上读取bufsize字节数的数据

   socketfd是连接socket 
   buf是数据存储空间指针
   bufsize是此次要读取数据的字节数

   返回值 0表示读取成功，
         >0表示只读取部分数据，数值表示已经读取的字节数，
         -1表示读取发生错误
  
  【注意】如果没有读取够bufsize字节数的数据则会一直处于while循环中
*/
ulong readex(int socketfd, char * buf, ulong bufsize){

     char * buftemp = buf;
     ulong leftsize = bufsize; //剩余未读取的字节数

     while(1){

        if(leftsize == 0){//buf没有空间就不能再读取了
            break;
        }
        else if(leftsize < 0){
            return -1;
        }

        int readsize = recv(socketfd,buftemp,leftsize,0);

        if(readsize < 0){//读取数据错误

            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK){
                cout <<__FUNCTION__<<" "<<__LINE__<<" 收到系统信号【"<<errno<<"】忽略"<<endl;
                continue;
            }

            cout <<__FUNCTION__<<" "<<__LINE__<<" 读取数据错误"<<endl;
            return -1;

        }else if(readsize == 0){//recv返回值为0不是读到的数据大小为0，是对方断开了连接
            cout <<__FUNCTION__<<" "<<__LINE__<<" 对方断开通讯连接"<<endl;
            break;
        }

        cout <<__FUNCTION__<<" "<<__LINE__<<" readsize = "<<readsize<<endl;

        buftemp += readsize;  //偏移buf的指针
        leftsize -= readsize; //更新剩余未读取的字节数
     }

     return bufsize - leftsize; //计算出已经读取的字节数
}


/*
   功能 : 从socketfd上写入bufsize字节数的数据

   socketfd是连接socket 
   buf是数据存储空间指针
   bufsize是此次要写入数据的字节数

   返回值 0表示写入成功，
         >0表示只写入部分数据，数值表示已经写入的字节数，
         -1表示写入发生错误
  
  【注意】如果没有写入够bufsize字节数的数据则会一直处于while循环中
*/
ulong writeex(int socketfd, char * buf, ulong bufsize){

    char * buftemp = buf;
    ulong leftsize = bufsize;//剩余未写入的字节数

    while(1){

        if(leftsize == 0){//已经将buf中所有的数据都发送完毕了
            break;
        }else if(leftsize < 0){
            return -1;
        }

        //向对方发送数据
        int writesize = send(socketfd,buftemp,leftsize,0);

        if(writesize < 0){

            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK){
                cout <<__FUNCTION__<<" "<<__LINE__<<" 收到系统信号【"<<errno<<"】忽略"<<endl;
                continue;
            }

            cout <<__FUNCTION__<<" "<<__LINE__<<" 写入数据错误"<<endl;
            return -1;

        }else if(writesize == 0){//返回值为0，对方可能断开了连接
            cout <<__FUNCTION__<<" "<<__LINE__<<" 对方断开通讯连接"<<endl;
            break;
        }

        buftemp += writesize;
        leftsize -= writesize;
    }
    
    return bufsize - leftsize;//计算出已经写入的字节数
}