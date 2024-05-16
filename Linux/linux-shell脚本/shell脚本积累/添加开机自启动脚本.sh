#!/bin/bash

#使用awk检测 /etc/rc.d/rc.local 内又没有指定的命令

hope="su - root -c \"/opt/test.sh\""
res=$(awk ' /su - root -c \"\/opt\/test\.sh\"/  {print $0} ' /etc/rc.d/rc.local) #注意过滤语句中的转义

echo "res=${res}"
echo "hope=${hope}"

if [[ ${res} != ${hope} ]]
then
   echo "不存在自启动项目,增加自启动项"
   echo -e "${hope}\n"  >>  /etc/rc.d/rc.local #添加需要的命令
else
   echo "存在自启动项目"
fi

