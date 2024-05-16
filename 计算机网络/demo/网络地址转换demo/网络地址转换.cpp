#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

int main()
{
   char chip_addr[]="192.168.111.111";
   int res = 0;
   struct in_addr dst; //IPv4地址结构体
   char dstip[20] = {0};


   in_addr_t addr = inet_addr(chip_addr);
   cout<<" inet_addr 将点分十进制ip转换成网络字节序整数 "<<addr<<endl;

   res = inet_aton(chip_addr, &dst);
   cout<<" inet_aton 将点分十进制ip转换成网络字节序整数 res = "<<res<<"  dst = "<<dst.s_addr<<endl;

   char * ntoa_ipch = inet_ntoa(dst);
   cout<<" inet_ntoa 将网络字节序整数转换成点分十进制ip ntoa_ipch = "<<ntoa_ipch<<endl;

   //================================================================================================

   res = inet_pton(AF_INET, chip_addr, (void* )&dst);
   cout<<" inet_pton 将点分十进制ip转换成网络字节序整数 res = "<<res<<"  dst = "<<dst.s_addr<<endl;

   const char * ntop_ipch = inet_ntop(AF_INET, (const void *)&dst, dstip, sizeof(dstip));
   cout<<" inet_pton 将网络字节序整数转换成点分十进制ip ntop_ipch = "<<ntop_ipch<<"  dstip = "<<dstip<<endl;

   return 0;
}