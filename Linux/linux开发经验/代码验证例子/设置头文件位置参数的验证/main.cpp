/*
     此例子主要是为了验证 g++ -I 参数的使用
     在验证的过程中还是发现了一些需要注意的细节

    
     -I ./include/  -I ./include2/  这次表示的是在两个目录中查找,

     注意不要使用 -I ./include/ ./include2/ 连续路径，会出现不能识别的情况


     验证例子  
             运行成功(include2 中也有 myheader.h文件)
             g++ main.cpp ./include/myheader.cpp ./include2/football.cpp  -I ./include2/


             运行成功 因为设置的是在两个目录中查找
             g++ main.cpp ./include/myheader.cpp ./include2/football.cpp  -I ./include/ -I ./include2/


             运行失败 编译时只在 include 中查找了头文件 而没有在 include2中查头文件
             g++ main.cpp ./include/myheader.cpp ./include2/football.cpp  -I ./include/ ./include2/

 */


#include "myheader.h"
#include "football.h"


int main()
{
    showmsg();
    showfootball();
    return 0;
}