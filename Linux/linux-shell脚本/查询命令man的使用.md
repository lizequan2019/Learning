&emsp;&emsp;man命令用来查询系统中自带的各种参考手册，但是手册分为好几个部分


1. commands普通命令

2. system calls是系统调用，如open，write之类的(通过这个，至少可以很方便的查到调用这个函数，需要加什么头文件)

3. libray calls是库函数，如printf,fread

4. special files是特殊文件，也就是/dev下的各种设备文件

5. file formats and convertions是指文件的格式，比如passwd,就会说明这个文件中各个字段的含义

6. games for linux是给游戏留的，又各游戏自己定义

7. macro packages and conventions是附件还有一些变量，比如向environ这种全局变量在这里就有说明

8. system management commands是系统管理用的命令，这些命令只能由root使用，如ifconing

9. 其他