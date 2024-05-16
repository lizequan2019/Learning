

## Checking all files in a folder  检查整个目录
&emsp;&emsp;Normally a program has many source files. Cppcheck can check all source files in a directory:  
&emsp;&emsp;语法cppcheck path  
&emsp;&emsp;If “path” is a folder, then Cppcheck will recursively check all source files in this folder:  如果“路径”是一个文件夹，那么Cppcheck将递归检查该文件夹中的所有源文件
```
        Checking path/file1.cpp...
        1/2 files checked 50% done
        Checking path/file2.cpp...
        2/2 files checked 100% done
```


## Check files matching a given file filter  过滤文件
&emsp;&emsp;With --file-filter=<str> you can set a file filter and only those files matching the filter will be checked.(你可以设置一个文件过滤器，只有那些与过滤器匹配的文件才会被检查。)  

&emsp;&emsp;For example: if you want to check only those files and folders starting from a subfolder src/ that start with “test” you have to type: （如果只想检查以“test”开头的子文件夹src/开头的文件和文件夹，则必须键入）
```
cppcheck src/ --file-filter=src/test*
```
&emsp;&emsp;Cppcheck first collects all files in src/ and will apply the filter after that. So the filter must start with the given start folder.(Cppcheck首先收集src/中的所有文件，然后应用过滤器。**因此，过滤器必须从给定的开始文件夹开始。**)


## Excluding a file or folder from checking  忽略目录
&emsp;&emsp;有时候并非整个目录下都需要cppCheck检测，有些目录需要忽略，使用选项 -i 可以忽略目录
```
cppcheck -isrc/c src
```
&emsp;&emsp;This option is only valid when supplying an input directory. To ignore multiple
directories supply the -i flag for each directory individually.(此选项仅在提供输入目录时有效。要忽略多个目录，请分别为每个目录提供-i标志。) 
&emsp;&emsp;The following command ignores both the src/b and src/c directories:
```
cppcheck -isrc/b -isrc/c
```

## Severities  消息说明
&emsp;&emsp;The possible severities for messages are:
* error  错误
when code is executed there is either undefined behavior or other error, such as
a memory leak or resource leak

* warning 警告
when code is executed there might be undefined behavior

* style 风格
stylistic issues, such as unused functions, redundant code, constness, operator
precedence, possible mistakes.

* performance 性能
run time performance suggestions based on common knowledge, though it is not certain any measurable speed difference will be achieved by fixing these messages.(基于公共知识的运行时性能建议，但不确定通过修复这些消息是否能实现任何可测量的速度差异。)

* portability 可移植性
portability warnings. Implementation defined behavior. 64-bit portability. Some undefined behavior that probably works “as you want”, etc.

* information 消息通知
configuration problems, which does not relate to the syntactical correctness, but the used Cppcheck configuration could be improved.(配置问题，这与语法正确性无关，但使用的Cppcheck配置可以改进。)



## Cppcheck build folder  检测结果目录构建
&emsp;&emsp;On the command line you configure that through --cppcheck-build-dir=path.Example:
```
mkdir b
cppcheck --cppcheck-build-dir=b src # <- All files are analyzed
cppcheck --cppcheck-build-dir=b src # <- Faster! Results of unchanged files are reused  重复执行会检测的更快
```

## Importing a project 导入项目

TODO  //后面看一下怎么导入项目

## Include paths  包含目录设置
&emsp;&emsp;To add an include path, use -I, followed by the path
&emsp;&emsp;Cppcheck’s preprocessor basically handles includes like any other preprocessor.However, while other preprocessors stop working when they encounter a missing header, Cppcheck will just print an information message and continues parsing the code.(基本上包括任何预处理器的检查一样。**然而，当其他预处理器遇到丢失的头时会停止工作，Cppcheck只会打印一条信息消息并继续解析代码。**)
&emsp;&emsp;The purpose of this behaviour is that Cppcheck is meant to work without necessarily seeing the entire code. **Actually, it is recommended to not give all include paths. While it is useful for Cppcheck to see the declaration of a class when checking the implementation of its members, passing standard library headers is discouraged, because the analysis will not wor fully and lead to a longer checking time.** For such cases, .cfg files are the preferred way to provide information about the implementation of functions and types to Cppcheck.
实际上，建议不要提供所有的包含路径。虽然Cppcheck在检查类成员的实现时看到类的声明很有用，但不鼓励通过标准库头，因为分析不会完全失败，并且会导致更长的检查时间。对于这个我的理解是，系统和框架的头文件就不要设置了，这样会增加分析的时间，我们可以设置自己项目的头文件目录


## Platform 平台设置
&emsp;&emsp;Cppcheck has builtin configurations for Unix and Windows targets. You can easily use these with the --platform command line flag.(Cppcheck有针对Unix和Windows目标的内置配置。您可以轻松地将它们与——平台命令行标志一起使用。)
```
 --platform=<type>, --platform=<file>
                         Specifies platform specific types and sizes. The
                         available builtin platforms are:
                          * unix32
                                 32 bit unix variant
                          * unix64
                                 64 bit unix variant
                          * win32A
                                 32 bit Windows ASCII character encoding
                          * win32W
                                 32 bit Windows UNICODE character encoding
                          * win64
                                 64 bit Windows
                          * avr8
                                 8 bit AVR microcontrollers
                          * elbrus-e1cp
                                 Elbrus e1c+ architecture
                          * pic8
                                 8 bit PIC microcontrollers
                                 Baseline and mid-range architectures
                          * pic8-enhanced
                                 8 bit PIC microcontrollers
                                 Enhanced mid-range and high end (PIC18) architectures
                          * pic16
                                 16 bit PIC microcontrollers
                          * mips32
                                 32 bit MIPS microcontrollers
                          * native
                                 Type sizes of host system are assumed, but no
                                 further assumptions.
                          * unspecified
                                 Unknown type sizes
```


## C++标准配置
&emsp;&emsp;Use --std on the command line to specify a C/C++ standard.
&emsp;&emsp;Cppcheck assumes that the code is compatible with the latest C/C++ standard,but it is possible to override this. The available options are:
* c89: C code is C89 compatible
* c99: C code is C99 compatible
* c11: C code is C11 compatible (default)
* c++03: C++ code is C++03 compatible
* c++11: C++ code is C++11 compatible
* c++14: C++ code is C++14 compatible
* c++17: C++ code is C++17 compatible
* c++20: C++ code is C++20 compatible (default)


