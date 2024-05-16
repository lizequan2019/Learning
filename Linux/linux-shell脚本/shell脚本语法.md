[TOC]

# Shell语法


## 脚本编写规范
  
* 一个变量最好只干一种逻辑，否则容易混淆
  
* 可以使用 sh -n xxx.sh 检测脚本语法
  
* 可以使用 sh -x xxx.sh 跟踪脚本

* **基于bash解释器学习**


## 解释器的查看与设置

```shell

# 查看系统当前⽀持哪些shell解释器
  cat  /etc/shells

# 查看系统当前默认的shell解释器
  echo $SHELL

# 修改默认的shell解释器,修改了系统默认shell之后不会⽴即⽣效，之后再次登录系统修改的shell才会⽣效
  chsh -s /bin/sh

```

## 解析shell脚本入参

> 参考博客 https://blog.csdn.net/weixin_43999327/article/details/118968405  
 
> 参考博客 https://www.cnblogs.com/f-ck-need-u/p/9758075.html

&emsp;&emsp;这里使用`getopt`命令完成解析。使用版本`util-linux 2.38.1`。

```shell
    getopt optstring parameters                                       #第一种解析方式
    getopt [options] [--] optstring parameters                        #第二种解析方式
    getopt [options] -o|--options optstring [options] [--] parameters #第三种解析方式
```


### 验证方法

&emsp;&emsp;如果写脚本的时候需要验证解析参数规则的正确性，那直接在命令行中写就可以。如下图所示

![avatar](http://43.138.39.50:8099/lzq/learnmyself/-/raw/main/pic/shell脚本语法-1.png?ref_type=heads)

&emsp;&emsp;如果要在脚本里面验证，可以使用下面的模板

```shell

    #!/bin/bash

    arg=$@
    echo "arg=${arg}"  #先输出一下输入参数

    args=$(getopt  abc:d "$@")

    if [[ $? != 0 ]]
    then #检查解析参数是否正确
            echo "解析输入参数错误，程序退出"
            exit 1
    fi

    eval set -- "${args}"

    #遍历选项并且使用
    while true
    do
            case "$1" in

                -a)
                        echo "【a】"
                        ;;
                -b)
                        echo "【b】"
                        ;;
                -c)
                        echo "【c】$2"
                        shift
                        ;;
                -d)
                        echo "【d】"
                        ;;
                --) 
                    break
                    ;;

                *) #没有对应的参数走这个分支
                        echo "【error】$1"
                        exit 0
                    ;;
            esac
            shift
    done
```


### 选项格式 optstring

&emsp;&emsp;选项格式定义了传入shell脚本中的参数哪些是选项，哪些参数。其中哪些选项需要参数，哪些不需要。选项分为`长选项`和`短选项`。定义写起来太麻烦了，我举几个例子就懂了

```shell
     
     a #短选项

     abc #3个短选项 a b c

     abc: #3个短选项 a b c(必须带参数)

     ab::c: #3个短选项 a b(有无参数都行) c(必须带参数)

     -l  a # 长选项

     -l  a,b #两个长选项 a b

     -l  aa,bbbb  #两个长选项 aa bbbb

     -l  aa,bb::,cc: #3个长选项 aa bbbb(有无参数都行) cc(必须带参数)
```


### getopt命令选项

&emsp;&emsp;这是`getopt`命令自身的选项，如下

```shell

#  getopt的选项 
#  -a, --alternative            允许长选项以 - 开始
#  -h, --help                   这个简短的用法指南
#  -l, --longoptions <长选项>    要识别的长选项
#  -n, --name <程序名>           将错误报告给的程序名
#  -o, --options <选项字符串>     要识别的短选项
#  -q, --quiet                  禁止 getopt(3) 的错误报告
#  -Q, --quiet-output           无正常输出
#  -s, --shell <shell>          设置 shell 引用规则
#  -T, --test                   测试 getopt(1) 版本
#  -u, --unquoted               不引用输出
#  -V, --version                输出版本信息
```


### 第一种解析方式

&emsp;&emsp;先介绍一下第一种解析方式的组成部分,**此方式仅能使用短选项**。

```shell
   getopt  optstring parameters
#         [选项格式] [被解析内容]
```

```shell

    #下面是在终端上输入命令，以及终端输出的内容

    #例子1    
    lzq@lzq-PC:~$ getopt  abc:d  -abc   #-abc 中c后面没有内容了，说明c没有参数，不符合选项格式abc:d的规范
    getopt: 选项需要一个参数 -- c          #报错
    -a -b --

    #例子2
    lzq@lzq-PC:~$ getopt  abc:d  -abcd  #本来是想传入abcd四个选项的，结果d选项被识别为c的参数了
    -a -b -c d --

    #例子3
    lzq@lzq-PC:~$ getopt  abc:d  -abc 1 -d #这次使用正确了，c选项后面有一个参数1
    -a -b -c 1 -d --

```


#### 第一种解析方式存在的缺陷

&emsp;&emsp;第一种方式是**不支持处理带空格和引号的参数值**。下面对此说法进行验证。

&emsp;&emsp;使用上面`验证方法`小节的脚本进行测试，给c选项**传递带空格的参数**

```shell
    #例子5
    lzq@lzq-PC:~/Desktop$ ./ss.sh -a  -b -c "2313 2312321" -d
    arg=-a -b -c 2313 2312321 -d  #输出传入脚本的参数  发现没有引号了
    【a】
    【b】
    【c】2313                     #输出c对应的参数 发现只有2313  
    【error】2312321              #2312321被当做了选项
```


### 第二种解析方式

&emsp;&emsp;先介绍一下第二种解析方式的组成部分。和第一种解析方式不同，此次参数的组成部分中增加`getopt的选项`。

```shell
   getopt [options]             [--]            optstring    parameters
#         [getopt的选项 可选]    [破折号可选]      [选项格式]    [被解析内容]
```

&emsp;&emsp;大部分的元素都已经在第一种解析方式中介绍，下面再次验证一下`参数包含空格的问题`。

```shell

    #例子6
    lzq@lzq-PC:~/Desktop$ getopt   -- abc:d -a -c "231 312321"
    -a -c '231 312321' --  # c后面的参数被解析成了一个整体
```

&emsp;&emsp;此种解析方式还可以解析`长选项`，但是我感觉用法很奇怪，怪在[选项格式]上

```shell

    #例子7  看的是不是很奇怪，按照上面的 [选项格式] 居然要写两遍 
    #       其实第二遍的aa,bb:,cc这里随便写点东西就行，就是不能为空
    lzq@lzq-PC:~/Desktop$ getopt -l aa,bb:,cc -- aa,bb:,cc  --aa --bb "42432"
    --aa --bb '42432' --

    #       这个1就是随便写的
    lzq@lzq-PC:~/Desktop$ getopt -l aa,bb:,cc -- 1  --aa --bb "42432"
    --aa --bb '42432' --


    #例子8  再按照这种格式定义短选项  虽然输出了多余的'abc'，但是在脚本里面是能解析的  
    lzq@lzq-PC:~/Desktop$ getopt -o abc -- abc -a
    -a -- 'abc'

    lzq@lzq-PC:~/Desktop$ getopt -o ab:c -- 1  -a -b "2313"
    -a -b '2313' -- '1'


    #例子9  同时定义短选项和长选项，发现了没有，这就是第三种解析方式
    lzq@lzq-PC:~/Desktop$ getopt -o abc -l aa,bb:,cc -- -a --a  --b "2131"
    -a --aa --bb '2131' --

    #例子10 当然按照第二种解析方式也可以用，就是写的有点多余而且输出上也有多余，但是不影响脚本使用
    lzq@lzq-PC:~/Desktop$ getopt -o abc -l aa,bb:,cc -- abc aa,bb,cc  -a --a  --b "2131"
    -a --aa --bb '2131' -- 'abc' 'aa,bb,cc'
```

&emsp;&emsp; 经过测试，只能冗余的内容在`--`后面输出就不影响使用，脚本中可以过滤掉


### 第三种解析方式

&emsp;&emsp;先介绍一下第三种解析方式的组成部分。基本和第二种解析方法差不多
```shell
   getopt [options]            -o|--options   optstring   [options]           [--]             parameters
 #        [getopt的选项 可选]                  [选项格式]    [getopt的选项 可选]  [破折号 可选]      [被解析内容]  
```


### 总结

&emsp;&emsp;感觉有时间还是要多看看帮助手册，毕竟这三种解析方式还是没太搞懂，这里先总结一下规律。

&emsp;&emsp;如果只使用`短选项`而且也不用`getopt`的选项，那么第一种解析方式是最简便的。如果要使用`getopt`的选项或者要使用`长选项`，那么可以使用第二种解析方式。第三种解析方式，我觉得可以看作是第二种解析方式在同时使用`短选项`和`长选项`的优化版本。


## shell通配符

|字符|含义|实例|
|----|----|----|
|*|匹配0或多个字符|a*b a与b之间可以有任意长度的任意字符, 也可以一个也没有, 如aabcb, axyzb, a012b, ab。|
|?|匹配任意一个字符|a?b a与b之间必须也只能有一个字符, 可以是任意字符, 如aab, abb, acb, a0b。|
|[list]|匹配list中的任意单一字符|a[xyz]b a与b之间必须也只能有一个字符, 但只能是x或y或z, 如: axb, ayb, azb。|
|[!list]|匹配除list中的任意单一字符|a[!0-9]b a与b之间必须也只能有一个字符, 但不能是阿拉伯数字, 如axb, aab, a-b。|
|[c1-c2]|匹配c1-c2中的任意单一字符如：[0-9] [a-z]|a[0-9]b 0与9之间必须也只能有一个字符 如a0b, a1b... a9b。|
|\{string1,string2,...\}|匹配sring1或string2(或更多)其一字符串| a{abc,xyz,123}b a与b之间只能是abc或xyz或123这三个字符串之一。|


## 终端打印echo

&emsp;&emsp;作为终端的输出，下面列举一下使用的技巧  

```shell

        * echo输出变量的时候使用${var} , 注意$()是用来输出命令的输出

        * echo -e  可以使用转义字符 \n \t

        * echo $(#var)  会输出var变量的长度
```


### echo输出内容的颜色

```shell
#字体颜色：30m-37m 黑、红、绿、黄、蓝、紫、青、白
str="kimbo zhang"
echo -e "\033[30m ${str}\033[0m"      ## 黑色字体
echo -e "\033[31m ${str}\033[0m"      ## 红色
echo -e "\033[32m ${str}\033[0m"      ## 绿色
echo -e "\033[33m ${str}\033[0m"      ## 黄色
echo -e "\033[34m ${str}\033[0m"      ## 蓝色
echo -e "\033[35m ${str}\033[0m"      ## 紫色
echo -e "\033[36m ${str}\033[0m"      ## 青色
echo -e "\033[37m ${str}\033[0m"      ## 白色

#背景颜色：40-47 黑、红、绿、黄、蓝、紫、青、白
str="kimbo zhang"
echo -e "\033[41;37m ${str} \033[0m"     ## 红色背景色，白色字体
echo -e "\033[41;33m ${str} \033[0m"     ## 红底黄字
echo -e "\033[1;41;33m ${str} \033[0m"   ## 红底黄字 高亮加粗显示
echo -e "\033[5;41;33m ${str} \033[0m"   ## 红底黄字 字体闪烁显示
echo -e "\033[47;30m ${str} \033[0m"     ## 白底黑字
echo -e "\033[40;37m ${str} \033[0m"     ## 黑底白字


#控制码(常用): 
00 取消所有格式 
04 下划线
05 闪烁
07 反显
          
#常见的使用格式就是  
echo -e "\033[背景色;字体色;控制码m ${str} \033[0m" ## 背景色;字体色;控制码 的顺序可以颠倒
```


## 环境变量的设置

&emsp;&emsp;export命令可以将一个普通变量变成环境变量，当前shell脚本执行的任何程序都会继承这个变量

![avatar](http://43.138.39.50:8099/lzq/learnmyself/-/raw/main/pic/shell脚本语法-3.png?ref_type=heads)



## 算数

&emsp;&emsp;基础的整数运算可以使用 $((var1 + var2))这样的符号 

&emsp;&emsp;当需要更高级的计算时可以使用  bc(数学运算的高级工具)，在man中学习详细使用方法



## 文件描述符与重定向

&emsp;&emsp;常用的文件描述符`stdin=0 stdout=1 stderr=2`,文件描述符是与文件输入、输出相关联的整数。它们用来跟踪已打开的文件。

&emsp;&emsp;输出重定向是将输出的文本指定到某个文件中，可以覆盖当前文件中的文本或者追加文本

```shell
$ echo "This is a sample text 1" > temp.txt    //覆盖
$ echo "This is sample text 2"  >> temp.txt    //追加

错误输出 2>  文件   只是将错误输出覆盖到文件
错误输出 2>> 文件   将错误输出追加到文件

```

&emsp;&emsp;兼容正确输出和错误输出的重定向, 命令 &> (这是覆盖的)或者 &>> (这是追加的)文件，注意这将错误信息和正确信息都放到同一个文件中。 

&emsp;&emsp;还有一种形式是将正确输出重定向到文件1将错误输出重定向到文件2  **命令  > 文件1  2> 文件2**   


## 数组

```shell

#声明一个空数组
array_var=()   

#定义一个数组,注意元素之间不要加逗号
array_var=(1 2 3 4 5 6)  

#下面这样单独定义也可以
array_var[0]="test1"
array_var[1]="test2"
array_var[2]="test3"
array_var[3]="test4"
array_var[4]="test5"
array_var[5]="test6"

#还有一点和C语言中数组不太一样的是，shell中的数组是可以不连续的，可以在上面定义的基础上定义array_var[100]
array_var[0]="test1"
array_var[1]="test2"
array_var[2]="test3"
array_var[3]="test4"
array_var[4]="test5"
array_var[5]="test6"
array_var[100]="test100"

#打印出特定索引的数组元素内容
echo ${array_var[0]}    

#以清单形式打印出数组中的所有值 * 也可以用 @ 代替
echo ${array_var[*]}  

#打印数组长度(即数组中元素的个数)
echo ${#array_var[*]} 

#列出数组的索引
echo ${!array_var[*]}
```


## 日期

&emsp;&emsp;`date`可以获取当前的时间,也可以在`date`后面添加不同的参数来获取想要的数据,延时一般就用sleep就行了.

```shell

    $ date "+%Y/ %B/ %d"
    2021/ 一月/ 31

```

## 函数及其参数

&emsp;&emsp;shell脚本同样支持函数的定义,有以下两种方法

&emsp;&emsp;在使用变量的时候，自定义函数中的变量最好使用**local**修饰，变成局部变量

```shell
#函数的声明
    function fname()
    {
        statements
        return 0      #一般返回0都是正确退出状态  
    }

    fname()
    {
        statements
        return 0
    }

#函数的使用
    fname xxx # xxx是参数
```

&emsp;&emsp;接着说一下输入参数,shell脚本的输入参数和函数的输入参数是一样的。在脚本和函数的后面直接跟参数即可。

|参数|解释|
|----|----|
|$n|传递给脚本或函数的参数。n是一个数字，表示第几个参数。例如，第一个参数是$1，第二个参数是$2。参数在10以及10以上则需要用大括号$\{10\}, $0是命令本身|
|$()|$()是用来输出命令的结果|
|${}|输出变量的时候使用${变量}|
|$#|此变量代表命令行中所有参数的个数，不包括命令本身|
|$\*|此变量代表传递给脚本或函数的所有参数，会把所有参数当做一个整体，例如for循环传递参数时用 $\* 就不可以|
|$@|此变量代表传递给脚本或函数的所有参数，但每个参数还是独立的可以单独使用|
|$?|最近一次命令或自定义函数的执行情况|


## 多命令执行与管道符

|符号|含义|
|----|----|
|;|命令之间没有关系|
|&&|当前一个命令正确执行，后面的命令才会执行，前面命令错误，后面命令不执行|
|\|\||当前一个命令正确执行，后面命令不执行，前面命令错误，后面命令执行|
|\|(管道符)| 命令1 \| 命令2  命令1的正确输出作为命令2的操作对象|


## 字段分割符

&emsp;&emsp;内部字段分隔符(Internal Field Separator,IFS)是shell脚本编程中的一个重要概念。

&emsp;&emsp;内部字段分隔符是用于特定用途的定界符。 IFS是存储定界符的环境变量(默认是空白字符)。它是当前shell环境使用的默认定界字符串。

```shell
#!/bin/bash

data="name,sex,rollno,location"

oldIFS=$IFS

IFS=,               #给环境变量IFS赋值设置分隔符号为，

for item in $data;   
do
    echo Item: $item
done

IFS=$oldIFS
```

## 循环

```shell
#在循环这个部分我先学习for循环和while循环两个

#for循环有两种模式
for 变量 in x  #in 后面可以是一个列表 {1..100} {a..z}这样的，也可以是一个数组 ${array[@]}
do
     程序
done


for((i=1;i<=100;i=i+1))
do
    程序
done


#while 循环
while [ x ]
do
   程序
done
```

## if语句

```shell

#[ ]是符合POSIX标准的测试语句，兼容性更强，几乎可以运行在所有的Shell解释器中  
#[[ ]]仅可运行在特定的几个Shell解释器中(如Bash等)   这要学习[[]]格式的
  

#这里需要注意的格式if和[[]]要有空格，[[]]中的内容两端要有空格
#单分支if语句
    if [[ x ]];then
    程序
    fi


    if [[ x ]]
    then
    程序
    fi


#双分支if语句
    if [[ ]]
    then
        ...
    else
        ...
    fi


#多分支if语句
    if [[]]
    then
        ...
    elif [[]]
    then
        ...
    else
    fi

#与或条件
    if [[]] || [[]]

    if [[]] && [[]]
```


## 数字之间的比较

|关系运算符|说明|举例|
|----|----|----|
|-eq (==)|检测两个数是否相等,相等返回true|[ $a -eq $b ] 返回 true。|
|-ne (!=)|检测两个数是否相等,不相等返回true|[ $a -ne $b ] 返回 true。|
|-gt (>) |检测左边的数是否大于右边的,如果是,则返回true|[ $a -gt $b ] 返回 false。|
|-lt (<) |检测左边的数是否小于右边的,如果是,则返回true|[ $a -lt $b ] 返回 true。|
|-ge (>=)|检测左边的数是否大等于右边的,如果是,则返回true|[ $a -ge $b ] 返回 false。|
|-le (<=)|检测左边的数是否小于等于右边的,如果是,则返回true|[ $a -le $b ] 返回 true。|


## 字符串运算符

|字符串运算符|说明|举例|
|----|----|----|
|==|检测两个字符串是否相等,相等返回true |[[ $a == $b ]]|
|!=|检测两个字符串是否相等,不相等返回true |[[ $a != $b ]] 返回 true|
|-z|(zero)检测字符串长度是否为0,为0返回true |[[ -z $a ]] 返回 false|
|-n|(nozero)检测字符串长度是否为0,不为0返回true |[[ -z $a ]] 返回 true|


## 多重条件判断

|布尔运算符|说明|举例|
|----|----|----|
|!| 非运算,表达式为true则返回false,否则返回true |[ ! false ] 返回 true。|
|-o| 或运算,有一个表达式为true则返回true|[ $a -lt 20 -o $b -gt 100 ] 返回 true。|
|-a| 与运算.两个表达式都为true才返回true|[ $a -lt 20 -a $b -gt 100 ] 返回 false。|


## shell脚本的退出

&emsp;&emsp;使用exit x就可以退出脚本，x代表的是数字。
