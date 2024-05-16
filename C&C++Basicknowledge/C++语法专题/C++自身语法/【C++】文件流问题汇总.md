###  计算机中的输入、输出

&emsp;&emsp;【输入】文件 -> 内存    是指**输入到内存**中，也就是读取文件然后输入到内存中，
&emsp;&emsp;【输出】内存 -> 文件    是指**输出到文件**中，也就是写入到内存然后输出到文件中。

###  C++中二进制数据文件流有关函数

|函数|含义|用法|
|----|----|----|
|seekg()|输入流定位||
|seekp()|输出流定位||
|tellg()|输入流返回定位的位置||
|tellp()|输出流返回定位的位置||

```c++
        #include <ifstream>

        ifstream in("./出师表.txt");
        assert(in);

        //输入流示范
        in.seekg(0, ios::end);     //定位到文件末尾，不偏移
        streampos sp = in.tellg(); //返回定位的指针赋值给sp

        cout << "filesize:" << endl << sp << endl; //表示输入流的大小


        in.seekg(-sp / 3, ios::end); //负数为向前偏移，正数为向后偏移
        streampos sp2 = in.tellg();  //返回定位指针
        cout << "from file topoint:" << endl << sp2 << endl;


        cout << "====================================================================" << endl;


        //输出流示范
        in.seekg(0, ios::beg);    
        cout << in.rdbuf()<<endl;   //从头读出文件内容
        cout << "====================================================================" << endl;
        in.seekg(sp2);
        cout << in.rdbuf() << endl; //从sp2开始读出文件内容
```


### 处理不同类型流的类

&emsp;&emsp;**str**stream处理字符流；**io**stream处理输入输出流；**f**stream处理文件流。