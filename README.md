# 文档维护说明

1. 如果工作学习中遇到笔记上已记录的知识点,那么就要<b>及时验证笔记的正确性</b>,优化笔记
<br>

2. 记录笔记的内容一定要通顺易懂,保证以后阅读能短时间理解
<br>

   
3. 笔记中代码的存储位置,为了方便阅读,单文件代码直接写在在笔记中(尽量写完整的代码,拷贝到其他地方可直接编译使用),需要多个文件编译的代码放在单独的目录下,笔记中加引用链接。<b><font color="ff0000">(要求完整的代码可以使用g++编译过,方便以后验证笔记中的内容,而且每个示例代码要写详细的注释、用途和运行结果,也是方便以后快速理解代码)</font></b> 
<br>


4. 为了兼容移动端手机可以正常查看并渲染学习笔记,以后的学习笔记都转为html格式,以前的markdown笔记慢慢转换为html<font color="ff0000">【虽然手机端浏览笔记的项目停止了,但是考虑到又浏览器的电脑就能轻松阅读html笔记,笔记html化还是要继续执行】</font>
<br>

5. <b>笔记和笔记对应的图片存放在同一目录下,方便使用、查找和区分</b>
<br>

6. <b>图片文件的后缀名只能为png</b>
<br>

7. 文档中因为需要会出现外链接资源,这是不可避免,链接分为两种,<b>网络链接</b>和<b>本地链接</b>,网络链接就是http和https开头的链接,本地内链接使用的是相对路径(<b>注意是相对仓库根目录的路径</b>),下面举例说明
```
         网络链接 在html笔记中常见的形式 : 
         <a href="https://learn.microsoft.com/zh-cn/cpp/build/reference/exports?view=msvc-170">【网络链接】EXPORTS</a>
   
         本地文本文件链接 在html笔记中常见的形式 : 
         <a href="./accumulation/工作难题记录/程序界面无法拖拽到4K副屏问题.md">【本地链接】程序界面无法拖拽到4K副屏问题.md</a> 

         本地图片链接  
         <img src="./pic/shell脚本语法-1.png"></img>
         
         【注意】一定要按照上面的格式规范编写
```
<br>

8. 代码太长可以折叠

<details>
   <summary> 猜猜里面是什么?? </summary>
   <pre>
      ////////////////////////////////////////////////////////////////////
      //                          _ooOoo_                               //
      //                         o8888888o                              //
      //                         88" . "88                              //
      //                         (| ^_^ |)                              //
      //                         O\  =  /O                              //
      //                      ____/`---'\____                           //
      //                    .'  \\|     |//  `.                         //
      //                   /  \\|||  :  |||//  \                        //
      //                  /  _||||| -:- |||||-  \                       //
      //                  |   | \\\  -  /// |   |                       //
      //                  | \_|  ''\---/''  |   |                       //
      //                  \  .-\__  `-`  ___/-. /                       //
      //                ___`. .'  /--.--\  `. . ___                     //
      //              ."" '<  `.___\_<|>_/___.'  >'"".                  //
      //            | | :  `- \`.;`\ _ /`;.`/ - ` : | |                 //
      //            \  \ `-.   \_ __\ /__ _/   .-` /  /                 //
      //      ========`-.____`-.___\_____/___.-`____.-'========         //
      //                           `=---='                              //
      //      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        //
      //         佛祖保佑       永无BUG     永不修改                    //
      ////////////////////////////////////////////////////////////////////
   </pre>

</details>

<br>

9. 写笔记时遇到不明白、要优化的、未完成的地方需要使用【TODO】标记

<table border="1px">
   <tr> <td>标记类型</td> <td>解释</td> </tr>
   <tr> <td>【TODO疑惑】</td> <td>写笔记时遇到不明白的地方增加此标记,以后明白了,需要把思路写出来,【TODO疑惑】标记转为->【DONE疑惑】</td> </tr>
   <tr> <td>【TODO】</td> <td>还未完成的事,标记以后完成,完成后可以删除该标记</td> </tr>
</table>
<br>

10. 笔记及时备份到硬盘中,每次使用前都要从服务器上来获去最新的内容,防止冲突
<br>

11. 文件和目录更改时,检查其他笔记是否引用当前路径
<br>

12. 每一个专题目录下都应该有一个日积月累目录,用于存放日常使用该专题技术时遇到的问题(解决方案)和使用经验,实用经验和技术机制不能混为一谈,所以要分开记录
<br>

13. 对于书籍阅读笔记,不应该刻板的将每一章内容都记录下来,应该是在理解消化后去记录,可以将感悟分成若干个小文本,里面标记来自于什么书籍的哪一部分
<hr>  

14. draw.io的图片可以用html格式文件保存,所以用draw.io绘制的图像可以保存成html格式文件,在绘制程序框架、设计图时可以使用

# 精读过了一遍的专题
&emsp;&emsp;精心过了一遍是指专题目录文件的内容和格式都由我自己从新审阅了一遍,内容没有很严重的错误
```
         [√]版本管理
            [√]版本管理\使用gogs搭建git服务器\使用gogs搭建git服务器.html
            [√]版本管理\diff格式讲解\diff格式讲解.html
            [√]版本管理\git-reset的使用\git-reset的使用.html
            [√]版本管理\git的基本操作\git的基本操作.html
            [√]版本管理\git子模块基本操作\git子模块基本操作.html
         [√]程序调试
            [√]程序调试\linux调试\错误信息查询\错误信息查询.html
            [√]程序调试\linux调试\定位数组越界代码问题\定位数组越界代码问题.html
            [√]程序调试\linux调试\gdb的使用\gdb的使用.html
            [√]程序调试\windows调试\windbg简单使用\windbg简单使用.html
         []计算机网络
            [√]计算机网络\日积月累\阻塞IO与非阻塞IO的阻塞点在哪里\阻塞IO与非阻塞IO的阻塞点在哪里.html
            [√]计算机网络\日积月累\IO-阻塞-非阻塞-同步-异步模式的组合\IO-阻塞-非阻塞-同步-异步模式的组合.html
            [√]计算机网络\日积月累\为什么EAGAIN在读写数据时不算错误.html
            [√]计算机网络\日积月累\tcp通信send消息遇到的问题-SIGPIPE的处理.html
            [√]计算机网络\网络基础\多路复用IO\多路复用IO模式的区别.html
            [√]计算机网络\网络基础\网络地址\基础知识.html
            [√]计算机网络\网络基础\网络模型\网络模型 .html
            [√]计算机网络\网络基础\网络设备\网络设备.html
            [√]计算机网络\网络基础\网络设备\vlan-虚拟局域网.html
            [√]计算机网络\网络基础\ensp的简单使用\ensp的简单使用.html
            [√]计算机网络\网络基础\tcp\tcp.html
            [√]计算机网络\网络基础\wireshark的使用\wireshark的使用.html
         []设计模式
         []书籍阅读笔记
         []数据库
         []accumulation
         []C&C++Basicknowledge
         []Linux
         []QT
```
<hr> 

# 文档html化
```
         []版本管理
         []程序调试
         []工作经验
         []计算机网络
         []设计模式
         []书籍阅读笔记
         []数据库
         []accumulation
         []C&C++Basicknowledge
         []Linux
         []QT
```
<hr>  


# 2023年度目标

1. CSDN发布15篇博客 [11/15]


<br>
<br>
<br>
<br>


# 2024年度目标

1. CSDN发布15篇博客 [6/15]
   
2. 本年度的重点学习目标,列举出来,避免精力分散
```
         (1) QT       : qt机制学习和源码
         (2) 书籍阅读 : <<程序员的自我修养>>
         (3) 整理仓库笔记
```

<br>
<br>



