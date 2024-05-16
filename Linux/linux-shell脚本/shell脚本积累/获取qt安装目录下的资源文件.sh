#!/bin/bash

#判断上次执行是否正确
fun_judg(){
  if [[ $1 -ne 0 ]]
  then
      echo "上次执行错误，结束执行 line:$2"
  else
      echo "执行正确 line:$2"
  fi
}


export QT_SELECT=qt_5.15.2
qmakev_output=$(qmake -v)
fun_judg $? ${LINENO}

#获取qmake路径
qmake_path=$(echo ${qmakev_output} | gawk '{print $9}')
fun_judg $? ${LINENO}


#根据qmake路径获取qt的安装目录
qt_install_path=$(echo ${qmake_path} | gawk 'BEGIN{FS="/";num=0}
  {
     num=NF;
     temp="";

     # $0 /home/lzq/Qt/5.15.2/gcc_64/lib  
     # $1 为空
     # $2 home
     # 所以for循环从2开始
     
     for( i=2;i<num;i++ ){ 
        temp=temp"/"$i;
     }
     print temp
  }
')
fun_judg $? ${LINENO}



echo "QT安装目录下的资源文件 ${qt_install_path}"