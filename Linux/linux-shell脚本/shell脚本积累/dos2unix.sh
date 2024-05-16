#!/bin/bash
 
#输出日志
function printf_log()
{
	if [[ ${showlog} -eq 1 ]]
	then 
         echo "[LOG] $1" 
	fi
} 

#输出错误信息
function printf_err()
{
      echo -e "\033[31m $1 \033[0m"
}

#输出系统信息提示信息
function printf_tip()
{
	  echo -e "\033[33m $1 \033[0m"
}


#输出脚本参数使用说明
function printf_paramrule()
{
   echo "==============================脚本说明============================================================"
   echo "【功能】 此脚本以dos2unix工具为基础,可以递归的将目录下的文本文件由dos格式转换为unix格式"
   echo "【环境】 核心的转换使用git工具的dos2unix.exe因此需要安装git, 请在命令行界面上运行  "
   echo "【参数】 -r[可选项]  使用此参数启动递归遍历                                       "
   echo "         -d[可选项]  使用此参数启动输出日志                                       "
   echo "         -s[必填项]  需要转换的目录文件路径   例如 -s \"/xxx/xxx/目录  /xxx/xxx/文件 \" "
   echo "         -e[可选项]  匹配需要转换文件的后缀名 例如 -e \"*.cpp  *.h\" 不使用此选项默认转换所有文件"
   echo "         -h[可选项]  帮助                                                        "		   
   echo "【兼容性】支持windows和linux平台"
   echo "=================================================================================================="
}
 
 
#作用 :  递归(或不递归)查找文件并进行dos2unix转换
#参数 :  $1表示是否递归查找 0不递归查找 1递归查找   
#        $2表示根目录
function read_dir(){
 
    local isRecursion=$1  #是否递归查找, 0不递归查找 1递归查找
 
	for file in `ls $2`   #执行ls 获取根目录下的目录或文件
	do
		 local filepath=$2/$file  #拼接路径
		 
		 if [[ -d ${filepath} ]]    
		 then #如果filepath是目录 还需要判断是否需要进一步递归

		    if [[ ${isRecursion} -eq 1 ]]
			then #判断是否需要递归查找
			   read_dir ${isRecursion}  ${filepath}
			else
			   printf_tip "未启用递归查找 ${filepath} 目录下的内容不再转换"
			fi
			
		 elif  [[ -f ${filepath} ]] 
		 then #如果查到的是文件
 
            local isignore=1 #是否忽略转换标志位,1表示忽略 0表示不忽略
			
			#当存储后缀名的数组有内容时，则符合后缀名的文件才会被转换
            local i
			for(( i=0;i<${#aryParamUse[*]};i=i+1 ))
			do
			     if [[ "${filepath}" == ${aryParamUse[${i}]} ]]
                 then
				        isignore=0  #即将转换的文件后缀名符合 aryParamUse中存储的后缀,不忽略转换
				 fi
			done
 
            #如果存储后缀名的数组为空，则表示任意文件都需要转换
            if [[ ${#aryParamUse[*]} -eq 0 ]]
            then 
			      isignore=0;
			fi
 
			if [[ ${isignore} -eq 0 ]] 
			then #如果标志位为0,则最后进行转换文件
			    printf_log "即将被转换的文件 ${filepath}"

                local platform=$(uname | tr [a-z] [A-Z])  #tr将所有的字母转换为大写
				local linux="LINUX"

				if [[ ${platform} == ${linux} ]]
                then
			         dos2unix     ${filepath}
				else 
				     dos2unix.exe ${filepath}
				fi  
				
				let filenum+=1
			fi
		 fi 
	done
} 
 

#==========================================全局变量定义===================================
isSopt=0            #入参中是否存在 -s
 
aryParamUse=()      #此数组存储会使用到的后缀名格式
aryParamSrc=()      #此数组存储需要转换的目录或文件
 
isRecursion=0       #递归标志变量 值为0表示不递归 值为1表示递归
 
showlog=0           #是否显示日志 值为0不显示日志 值为1显示日志  
filenum=0           #遍历的文件数量
#=========================================================================================
 
 
 
#======================================解析输入参数是否正确===============================

#如果输入参数中存在未定义参数则脚本退出报错
args=$(getopt -o drhs:e: -- drhs:e: "$@") 
 
if [[ $? != 0 ]]
then 
	 printf_err "解析输入参数错误，程序退出"
	 printf_paramrule
	 exit 1
fi
 
#初始化 位置参数  也就是将 args 的参数值 分别赋值给 $1 $2 $3 ....
eval set -- "${args}"

 
#循环处理输入参数
set -f #禁用通配符
while true
do
  case "$1" in
      -d)
	      #设置是否显示日志
		  printf_tip "已开启日志输出"
		  showlog=1
	      ;;

	  -r) 
	      #设置递归遍历标志位
		  printf_tip "已开启递归转换"
		  isRecursion=1
		  ;;
	  
	  -e) 
	      #设置后缀名		
          caseindex=0
		  for var in $2 
		  do
		       printf_log "   ${var}   存储的后缀名数组"
			   aryParamUse[${caseindex}]=${var}
			   caseindex=`expr ${caseindex} + 1`
          done
 
		  shift
		  ;;
  
      -s)
	      #设置路径
		  caseindex=0
		  for var in $2
		  do
			   aryParamSrc[${caseindex}]=${var}
			   caseindex=`expr ${caseindex} + 1`
          done
		  
		  isSopt=1 #设置全局变量,置1表明有-s选项
 
		  shift
          ;;

	  -h)
		  printf_paramrule                                                
		  exit 0
		  ;;
		  
	  --) #随着参数左移，所有的参数都会被解析完毕，此时跳出switch case
	      break
	      ;;

	   *) #这个是负责错误参数的
	      printf_err "$1 参数选项值错误"
	      printf_paramrule
		  exit 0
	      ;;
  esac
 
  shift  # 左移参数 也就是 $1参数已经使用后 将 $2赋值给$1  将$3赋值给$2
  
done
set +f #使能通配符


if [[ ${isSopt} -eq 0 ]]
then
	  printf_err "-s选项必须添加"
	  printf_paramrule
	  exit
fi 
#========================================================================================
 
 
 
echo "******开始转换****** `date`"
 
#遍历aryParamSrc数组
for(( index=0;index<${#aryParamSrc[*]};index++ ))
do
    aryParamSrcele=${aryParamSrc[${index}]}

	printf_log "遍历到第{$index}个目录(文件)   ${aryParamSrcele}"
 
    if [[  -d ${aryParamSrcele}  ]] 
	then #如果是目录
	     read_dir ${isRecursion} ${aryParamSrcele}
 
	elif [[  -f ${aryParamSrcele} ]] 
	then #如果是文件
	     dos2unix.exe  ${aryParamSrcele}
		 let filenum+=1
	fi
done
 
echo "共转换文件数量 : ${filenum}"
echo "******转换结束****** `date`"