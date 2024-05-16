#!/bin/bash

#原来以为tty7就是桌面终端，后面发现银河麒麟是这样的，而uos则是tty1
#tty1-tty6是本机上面的登入者程序,若为pts/0等等的,则表示为由网络连接进主机的程序。

num=$(who | wc |  awk '{print $1}') #who获取已经登录的用户 wc获取输出的行数、列数、字数  awk输出wc中的行数
username=" "

#依据行数遍历
for((i=1; i<=${num}; i++))
do
   temp=$(who | sed -n "${i}p")  #获取who输出中的第i行内容  【for中使用sed】
   content=$( echo ${temp} | cut -d ' ' -f 2)  #获取第2列内容

   if [[ ${content} == "tty"* ]] #如果第2列内容是tty(桌面终端)
   then
      username=$( echo ${temp} | cut -d ' ' -f 1) #获取第1列内容(用户名)
      break
   fi
done


echo "username=${username}"