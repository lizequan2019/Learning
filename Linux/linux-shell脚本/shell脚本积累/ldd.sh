#! /bin/bash
 
#执行脚本时在终端输入　./copy.sh ./test ./copy
#其中./test为ldd所要查看的可执行程序的路径，./copy为依赖文件最终拷贝到的文件夹路径
 
#以下三个参数为终端输入
# $0 为 ./copy.sh
# $1 为 ./test
# $2 为 ./copy
 
 
# awk 后　$0,$1,$2为终端日志的字符串，以空格为分隔符，只在awk本行起作用
#将字符串重定向到dependList
dependList=$( ldd $1 | awk '{if (match($3,"/")){ print $3}}' )
 
 
#输出　./test
#echo $1
 
#拷贝依赖
cp $dependList $2
#拷贝可执行程序
cp $1 $2