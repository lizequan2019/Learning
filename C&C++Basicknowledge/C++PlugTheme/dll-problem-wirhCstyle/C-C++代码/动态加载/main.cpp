#include <iostream>
#include "plug.h"
#include "plug2.h"
#include <dlfcn.h>  //动态链接

using namespace std;

typedef int (*myfun)();

int main()
{
    //动态加载库文件
    void * phandle = dlopen("./libplug.so",RTLD_NOW);
    void * phandle2 = dlopen("./libplug2.so",RTLD_NOW);

    //解析导出符号
    myfun add_func = (myfun)dlsym(phandle, "fun");
    add_func();
    
    myfun add_func2 = (myfun)dlsym(phandle2, "fun");
    add_func2();

    //关闭
    dlclose(phandle);
    dlclose(phandle2);
    return 0;
}