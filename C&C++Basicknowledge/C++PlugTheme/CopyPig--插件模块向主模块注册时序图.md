```plantuml

autonumber

主模块--->主模块 : 初始化**PlugManager(插件管理器)**,并获取对应的对象指针pPlug
activate 主模块 #0000ff

    loop 遍历指定目录下的动态库文件

                        == 主模块加载子模块动态库 ==

        
        主模块--->主模块 : pPlug动态加载指定的库文件，得到QLibrary对象指针lib
        activate 主模块 #00ff00
        deactivate 主模块

        alt QMap<QString, QLibrary * > dynamicLibraryMap_ 中不存在lib


            主模块--->主模块 : pPlug将lib添加到**dynamicLibraryMap_**中
            activate 主模块 #00ff00
            deactivate 主模块

            主模块--->主模块 :  lib解析插件模块中的导出函数并获取导出函数的函数指针fun
            note bottom: 导出函数fun本质上是插件模块的一个回调函数。\n主模块可以主动调用插件的回调函数，\n使插件模块完成向主模块注册的操作。
            activate 主模块 #00ff00
            deactivate 主模块


                        == 主模块调用插件模块回调函数 ==


            主模块--->插件模块 :  执行导出函数指针fun,向其中传入**Server_OperPlug**类型变量
            note bottom:  Server_OperPlug是一个结构体，\n其中主要包含的是\n主模块的一个回调函数PlugManager::registerPlug。\n 在插件模块调用此函数。
            activate 主模块 #00ff00
            deactivate 主模块
            
                        == 插件模块调用主模块回调函数 ==

        activate 插件模块 #0000ff

        插件模块--->主模块 :  调用主模块中的回调函数,\n并向其中传入参数(**插件模块对应的名称**，\n**存储着插件模块版本、构造和析构函数等等**)
        note right: 插件模块向主模块传递的信息不止上面所说的这些，\n还可以根据情况进行扩展

        deactivate 插件模块



        主模块--->主模块 : 调用PlugManager::registerPlug
        activate 主模块 #00ff00
        note left : 注意，这个函数是插件模块调用的，\n不是主模块主动调用的

                alt registerPlug函数入参中的插件版本符号主模块要求的版本

                        alt QMap<QString, Plug_RegisterInfo> exactMatchMap_ \n变量中不存在registerPlug函数入参中的插件名称

                            主模块--->主模块 : exactMatchMap_ 保存插件名称以及\n对应的Plug_RegisterInfo信息此时插件注册成功
                        end

                end

        deactivate 主模块

    end

deactivate 主模块

end

```