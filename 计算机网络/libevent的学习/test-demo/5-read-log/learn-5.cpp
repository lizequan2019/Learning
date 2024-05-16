#include <event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

//读取文件 回调函数
void readfile_cb(int fd, short ev_waht, void * arg){    
    char buf[1024] = {0};

    //读取文件
    int res = read(fd,buf,sizeof(buf)-1);
    if(res > 0){
        cout<<"[read log] "<<buf<<endl;
    }else{
        usleep(100 * 1000);
    }
}


int main(){
   
   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return 1;
   }

   cout<<"this is learn-5.cpp"<<endl;

   event_base * base = NULL;

   //创建上下文配置对象
   event_config *conf = event_config_new();
   event_config_require_features(conf,EV_FEATURE_FDS);

   //创建上下文对象
   base = event_base_new_with_config(conf);
   
   //及时删除上下文配置对象 conf
   event_config_free(conf);

   //打开文件
   int readfd = -1;
   readfd = open("/var/log/auth.log",O_RDONLY|O_NONBLOCK,0);
   
   //调整读取文件位置
   lseek(readfd, 0, SEEK_END);

   //创建事件对象
   event * ev = event_new(base,readfd,EV_READ|EV_PERSIST,readfile_cb,NULL);
   event_add(ev,NULL);

   //回收资源
   event_base_dispatch(base);
   event_del(ev);
   event_free(ev);
   event_base_free(base);
   close(readfd);
  
   return 0;
}
