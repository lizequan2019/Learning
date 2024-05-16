## awk基础

### awk基本格式

```shell
	awk 'awk_program' xxxx
```

&emsp;&emsp;xxxx是awk要读取的文件，可以是0和或者n(>=1)个文件，不给定任何文件，可以从标准输入中读取
&emsp;&emsp;被两个单引号包围的是awk代码


```shell
	awk '{aaa}{bbb;ccc}' xxxx
```

&emsp;&emsp;awk程序中，大量使用大括号，表示代码块，代码块中间可以连用，代码块内部的多个语句需要使用分号分隔


### BEGIN和END语句块

#### BEGIN

&emsp;&emsp;在读取文件之前执行，且执行一次，在BEGIN代码块中无法使用`$0`或其他一些特殊变量

#### END

&emsp;&emsp;在读取文件完成之后执行，且执行一次，在END代码中可以使用`$0`等一些特殊变量，使用的特殊变量保存的是下一轮awk循环的数据

## 分析awk如何读取文件

&emsp;&emsp;awk读取文件时，每次读取一条记录(**默认情况下按照行读取，所以此时记录就是行**)，每读取一条记录，将其保存到`$0`中，然后执行一次主代码段

&emsp;&emsp;如果是空文件，则因为无法读取到任何一条记录，将导致直接关闭文件，不会执行主代码段


## awk中的特殊变量

&emsp;&emsp;包含语法关键字和内置变量。

### RS(Record Separator)

&emsp;&emsp;通过在BEGIN代码块中设置此变量，来改变每次读取记录时的分割符

```shell
	$ echo "hello world" | awk 'BEGIN{RS="\n"}{print "i say "$0}'
	i say hello world  #以\n为分隔符从标准输入读取记录

        $ echo "hello world" | awk 'BEGIN{RS=" "}{print "i say "$0}'
        i say hello        #以" "为分隔符从标准输入读取记录
        i say world
```

&emsp;&emsp;RS的值可以时一个字符，也可以是正则表达式字符串

### RT(Record Termination)

&emsp;&emsp;在awk每次读完一条记录时，会设置一个称为RT的预定义变量，当RS为单个字符，RT和RS的值相同

&emsp;&emsp;当RS为正则表达式时，则RT设置为实际匹配到的分隔符字符串

&emsp;&emsp;当无法匹配到分隔符时，RT设置为控制空字符串，例子如下:

```shell
        #正则表达式(fe)?male 无法从"hello world"中匹配到内容 所以RT设置为空
	ubuntu@VM-8-12-ubuntu:~/test-awk$ echo "hello world" | awk 'BEGIN{RS="(fe)?male"}{print RT}' 
	
        #正则表达式可以匹配到内容female  所以RT设置为female
	ubuntu@VM-8-12-ubuntu:~/test-awk$ echo "afemale" | awk 'BEGIN{RS="(fe)?male"}{print RT}' 
	female

        #正则表达式可以匹配到内容male  所以RT设置为male
	ubuntu@VM-8-12-ubuntu:~/test-awk$ echo "male" | awk 'BEGIN{RS="(fe)?male"}{print RT}' 
	male
```

### NR和FNR

&emsp;&emsp;每个文件的行号都在一个数列中累加表示，比如每个文件都是10行，第一个文件占1-10，第二个文件占11-20

&emsp;&emsp;每个文件的行号分别用一个数列表示，比如每个文件都是10行，第一个文件占1-10，第二个文件占1-10

### FS

&emsp;&emsp;awk读取每一条记录之后，会将其赋值给`$0`,同时还会对这条记录进行分割(按照FS变量的值分割)，分割好的字符串会被分别赋值给`$1 $2 $3 $4...$N`,同时将分割的字段数量赋值给`NF`

&emsp;&emsp;在主代码块中设置FS对本次已经读取的记录没有影响，但是会影响下一次读取

### IGNORECASE

&emsp;&emsp;设置此变量为非零值，FS的正则匹配会忽略大小写(FS的单个字符无影响)

### FIELDWIDTHS

&emsp;&emsp;按字符宽度分割字段，在处理某字段缺失时很好用，例子如下

```shell
#这是要输入到awk程序中的内容，存储于FIELDWIDTHS-test.txt中
ID    name    sex    age    email          phone
1     Bob     m      13     abc@qq.com     123123123
2     Tony    w      23     def@qq.com     321321321
3     Andy    w      20                    213213213
4     Alex    m      18     dad@19.com     312312312
5     Alice   w      17     asd@ee.com     132132132
6     Peter   m      15     113@aq.com     231231231

#这是awk脚本4($1占4字符)  2:4(跳过2字符 $2占4字符) 
awk 'BEGIN{FIELDWIDTHS="4 2:4 4:3 4:3 4:10 5:10"}{print  "【"$1"】","【"$2"】","【"$3"】","【"$4"】","【"$5"】","【"$6"】"}' FIELDWIDTHS-test.txt

#这是运行结果
【ID  】 【name】 【sex】 【age】 【email     】 【phone】
【1   】 【Bob 】 【m  】 【13 】 【abc@qq.com】 【123123123】
【2   】 【Tony】 【w  】 【23 】 【def@qq.com】 【321321321】
【3   】 【Andy】 【w  】 【20 】 【          】 【213213213】
【4   】 【Alex】 【m  】 【18 】 【dad@19.com】 【312312312】
【5   】 【Alic】 【w  】 【17 】 【asd@ee.com】 【132132132】
【6   】 【Pete】 【m  】 【15 】 【113@aq.com】 【231231231】
```

### FPAT

&emsp;&emsp;此变量是取正则匹配到的字符串作为字段，存放到`$1 $2 $3 ... $N`,`$0`不会修改，例子如下
