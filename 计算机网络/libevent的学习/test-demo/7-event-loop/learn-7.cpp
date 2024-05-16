#include <event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

bool blbreak = false;

//处理ctrl-c 信号的回调函数
void DealCtrl_C(int fd, short ev_waht, void * arg){
   cout << "捕获到 Ctrl C 事件" << endl;
  
   event_base * _base = (event_base * )arg;
   
   //验证停止循环接口
   if(blbreak == true){
	   //执行完当前激活事件的回调函数后就会退出循环
	   event_base_loopbreak(_base);
   }else{
	   
	   /*
	      (1) 没有激活事件时不会退出循环，至少等待一次事件激活并处理完回调函数后再退出
		  (2) 当等待一次事件激活并处理完回调函数后，如果还没有到超时事件，此时还会继续等待
		  (3) 如果有连续的事件被激活，则会一直等待处理完事件的回调函数再退出，即使花费的时间超过了超时时间
	   */
	   timeval _t = {2,0};
	   event_base_loopexit(_base, &_t);
   }
   
   //这里延时4秒，间断延时使得事件循环继续捕获后面的Ctrl C事件
   for(int i = 0; i < 400; ++i){
	   usleep(1000 * 10);
   }
   cout << "处理捕获 Ctrl C 事件完毕" << endl;
}

//处理kill信号的回调函数 也就是kill进程可以进入此回调函数 但是kill -9 还是会杀死进程
void DealKill(int fd, short ev_waht, void * arg){
   cout << " Kill " << endl;

   event * _sig = (event *)arg;
   if(!event_pending(_sig, EV_SIGNAL, NULL)){//当信号事件处于未决状态时,
       if(event_add(_sig,NULL) != 0){//将事件加入到任务表中，这样算作根据条件来决定事件是否持久化，而非单纯使用EV_PERSIST实现持久化
          cout<<"event_add error"<<endl;
       }else{
          cout<<"kill事件被加入"<<endl;
       }
   }
}

int main(int argc, char * argv[]){
   
   cout<<"libevent循环接口设置 :"<<endl;
   cout<<" ONCE     for EVLOOP_ONCE"<<endl;
   cout<<" NONBLOCK for EVLOOP_NONBLOCK"<<endl;
   cout<<" ON_EMPTY for EVLOOP_NO_EXIT_ON_EMPTY"<<endl;
   cout<<" DEF      for 0"<<endl;

   cout<<"是否注册事件 :"<<endl;
   cout<<" yes      for reg event"<<endl;
   cout<<" no       for not reg event"<<endl;
   
   cout<<"libevent停止循环接口设置 :"<<endl;
   cout<<"LOOPBREAK for event_base_loopbreak"<<endl;
   cout<<"LOOPEXIT  for event_base_loopexit"<<endl;
   
   if(argc != 4){
      cout <<"param num != 4"<<endl;
	  return -1;
   }
   
   if(strcmp(argv[3],"LOOPBREAK") == 0){
	   blbreak = true;
   }else if(strcmp(argv[3],"LOOPEXIT") == 0){
	   blbreak = false;
   }

   if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){//忽略管道信号
	   return -1;
   }

   cout<<"this is learn-7.cpp"<<endl;

   //监听端口
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(9999);

   //创建libevent的上下文
   event_base * base = event_base_new();
   if(base){
	    cout <<"event_base_new success"<<endl;
   }
   
   event * ev_sig = NULL;
   event * ksig   = NULL;
   
   //第一个参数如果是yes 就注册事件
   if(strcmp(argv[2],"yes") == 0){
      //control+c
      ev_sig = evsignal_new(base,SIGINT,DealCtrl_C,base);
      if(!ev_sig){
         cout<<"evsignal_new error"<<endl;
      }else{
         if(event_add(ev_sig,NULL)  != 0){
               cout<<"event_add error"<<endl;
         }else{
			   cout<<"注册DealCtrl_C"<<endl;
		 }
      }

      ksig = event_new(base,SIGTERM,EV_SIGNAL,DealKill,event_self_cbarg());
      if(!ksig){
         cout<<"evsignal_new error"<<endl;
      }else{
         if(event_add(ksig,NULL)  != 0){
               cout<<"event_add error"<<endl;
         }else{
			   cout<<"注册DealKill"<<endl;
		 }
      }
   }else{ 
	  cout<<"根据入参，不注册事件"<<endl;
   }

   //事件分发
   int res = -1;
   if(base){
       //event_base_dispatch(base);
	   
       if(strcmp(argv[1],"ONCE") == 0){
         res = event_base_loop(base,EVLOOP_ONCE); 
		 cout<<" event_base_loop res = "<<res<<endl;
       }else if(strcmp(argv[1],"NONBLOCK") == 0){
		 cout<<"选择 NOBLOCK 循环30次等待"<<endl;
		 for(int i = 0; i < 30; ++i){
			cout<<"等待 "<<i+1<<endl;
			sleep(1);
			res = event_base_loop(base,EVLOOP_NONBLOCK); 
			cout<<"event_base_loop res = "<<res<<endl;
		 }
       }else if(strcmp(argv[1],"ON_EMPTY") == 0){
         res = event_base_loop(base,EVLOOP_NO_EXIT_ON_EMPTY); 
		 cout<<" event_base_loop res = "<<res<<endl;
       }else if(strcmp(argv[1],"DEF") == 0){
         res = event_base_loop(base,0); 
		 cout<<" event_base_loop res = "<<res<<endl;
       }
   }

   if(base){
       event_base_free(base);
   }

   
   if(ev_sig){
      if(event_del(ev_sig) == 0){
         event_free(ev_sig);
      }else{
         cout <<"event_del ev_sig error"<< endl;
      }
   }

   if(ksig){
      if(event_del(ksig) == 0){
         event_free(ksig);
      }else{
         cout <<"event_del ksig error"<< endl;
      }
   }

   return 0;
}
