/*
     此函数是配合future使用的，目的是方便操作


     Calls fn (with args as arguments) at some point, returning without waiting for the execution of fn to complete.
     在某个时刻调用Fn函数（以args作为参数），返回时无需等待Fn是否执行完成(会返回一个future对象)。

     
     调用async时可以选择启动Fn策略，不选择就是程序自动选择

     launch::async  启动一个新线程来调用Fn(就好像一个线程对象是用Fn和args作为参数构建的，并且访问返回的future的共享状态会加入它）。
      
      
     launch::deferred  对Fn的调用被推迟，直到访问返回的future的共享状态(使用wait或get)。
*/