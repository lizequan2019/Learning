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
	int sockListener,nMsgLen;
	char szBuf[1024];
	struct sockaddr_in addrListener;
	socklen_t addrLen;
	addrLen = sizeof(addrListener);
	addrListener.sin_family = AF_INET;
	addrListener.sin_port = htons(8000);
    addrListener.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockListener = socket(AF_INET,SOCK_DGRAM,0);
    bind(sockListener,(struct sockaddr *)&addrListener,addrLen);

	struct sockaddr_in addrClient;
	while(1)
	{
		memset(szBuf,'\0',1024);
        cout <<"==================="<<endl;
		nMsgLen = recvfrom(sockListener,szBuf,1024,0,(struct sockaddr *)&addrClient,&addrLen);
		cout <<"data form client = "<<szBuf<<endl;
		sendto(sockListener,szBuf,1024,0,(struct sockaddr *)&addrClient,addrLen);
	}
}
