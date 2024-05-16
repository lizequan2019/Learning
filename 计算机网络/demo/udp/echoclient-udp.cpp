#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <fcntl.h>
#include <iostream>
using namespace std;


int main()
{
    socklen_t addrLen=sizeof(struct sockaddr_in);
    struct sockaddr_in addrServer;
    char szMsg[1024];
    int sockClient;

    addrServer.sin_family=AF_INET;
    //addrServer.sin_addr.s_addr=inet_addr("127.0.0.1");
    addrServer.sin_addr.s_addr=htonl(INADDR_ANY);
    addrServer.sin_port=htons(8000);

    sockClient=socket(AF_INET,SOCK_DGRAM,0);
    while(true)
    {
        static int id=0;
        snprintf(szMsg,sizeof(szMsg),"this is %d",id++);
        cout <<"szMsg = "<<szMsg<<endl;
        sendto(sockClient,szMsg,strlen(szMsg),0,(struct sockaddr*)&addrServer,sizeof(addrServer));
        sleep(1);
    }
}