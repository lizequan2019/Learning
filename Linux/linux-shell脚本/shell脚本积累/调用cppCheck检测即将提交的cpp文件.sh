#!/bin/bash
 
input1=$1  #命令行输入第一个参数  【-sd】   Specify directory 指定路径参数
input2=$2  #命令行输入第二个参数  【路径】  路径的最后不需要加/ 否则路径会错误
 
echo "***************开始检测代码***************"
 
checkfilefolder="./cppCheckFolder"  #脚本执行的结果存储路径
 
rm -rf ${checkfilefolder}           #删除上次的check结果目录
mkdir  ${checkfilefolder}           #此脚本生成的文件都会存放到此目录中
 
checkfilename="-check.txt"          #cppCheck 分析文件后缀
 
statusfile="${checkfilefolder}/1.txt" # git status 输出文件路径
 
newfile="*new file"           #  * 是通配符的意思  这些是git status中的关键字段
modfile="*modified"           #  * 是通配符的意思
untrackfile="Untracked files" #未跟踪文件
 
#递归查找文件的函数
function read_dir(){
	for file in `ls $1` #注意此处这是两个反引号，表示运行系统命令
	do
		 filepath=$1/$file
		 if [ -d ${filepath} ]    #如果查找到的是目录
		 then
			read_dir ${filepath}
		 elif  [ -f ${filepath} ] #如果查到的是文件
		 then
			echo "发现文件 ${filepath}"
			if [[ ${file} == *.cpp || ${file} == *.h ]] #如果文件后缀是.cpp 或者 .h 
			then
				 resfile=$(echo ${file} | cut  -d "/" -f 2)               #获取 文件 xxx.cpp 或者 xxx.h  
				 resfile=${checkfilefolder}/${resfile}${checkfilename}    #拼接字符串 得出保存结果的文件名(对应着check的cpp)
				 echo "即将被检测的文件 ${filepath}"
				 cppcheck   ${filepath} --enable=all --std=c++11 --output-file=${resfile}  #调用 cppCheck 检测
			else
			   echo "${file} 此文件非.cpp 或者 .h, 不检测"
			fi
		 fi 
	done
} 
 
 
if [[ ${input1} = -sd ]] #输入参数为 -all 则检测所有的 .cpp 和 .h 文件 
then
 
    read_dir ${input2}
	
else
 
    git status ./ -uall >  ${checkfilefolder}/1.txt       #git status 的结果重定向到 1.txt 文件中  
 
	row=$(wc -l ${statusfile} | cut -d " " -f 1)
	for((i=1; i<=row; i++))  #for((i=1; i<=${row}; i++)) 原来使用的for循环在工作机上报错
	do
		rowcontent=$( sed -n ${i}p ${statusfile} | cut  -d ":" -f 1)  #获取是不是新增文件或者修改文件
		echo "${rowcontent}      第${i}行 | 共${row}行"
		if [[ ${rowcontent} == ${newfile} ||  ${rowcontent} == ${modfile} ]]  #这里是模糊比较因为${rowcontent}前面有空格
		#这个if判断 是否是 新增文件或者修改文件，如果是则进入到if中
			then
				filesuffix=$( sed -n ${i}p ${statusfile} | cut  -d "." -f 2)  #获取 新增文件或者修改文件的后缀名
				rowcontent=$( sed -n ${i}p ${statusfile} | cut  -d ":" -f 2)  #获取 新增文件或者修改文件的文件名(带路径)
				if [[ ${filesuffix} == cpp ]]
				#这个if判断的是 新增加或者修改文件是不是.cpp文件，如果是则进入if进行检测
					then
					 resfile=$(echo ${rowcontent} | cut  -d "." -f 1)         #获取 文件名不带后缀名带路径
					 resfile=$(echo ${resfile} | awk -F '/' '{print $NF}')    #去除路径 仅留下文件名
					 resfile=${checkfilefolder}/${resfile}${checkfilename}    #拼接字符串 得出保存结果的文件名(对应着check的cpp)
					 
					 echo "即将被检测的文件  ${rowcontent}"
					 cppcheck   ${rowcontent} --enable=all --std=c++11 --output-file=${resfile}  #调用 cppCheck 检测
				fi
	    elif [[ ${rowcontent} == ${untrackfile} ]] #到这里都是未跟踪的文件所以不需要检测
		   then 
		      break
		fi
	done
 
fi
 
echo "***********检测代码完毕***************"