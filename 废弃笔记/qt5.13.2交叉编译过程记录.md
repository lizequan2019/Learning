## 说明

&emsp;&emsp;内容都是从网上抄的还有一点自己研究的，不一定全对，读者需要甄别，有出处的地方的我都贴上网址了。

&emsp;&emsp;本次要适配的是`麒麟V10SP1`操作系统，交叉编译的qt版本是`qt5.13.2`

---

## 基础环境和搭建

&emsp;&emsp;使用虚拟机进行编译，安装`ubuntu-20.04.2-desktop-amd64.iso`操作系统，下载`qt-everywhere-src-5.13.2.tar.xz`源码

&emsp;&emsp;虚拟机的配置尽量要高一些，内存我配置到了16G，CPU配置到了6核数

---

## 搭建根文件系统

&emsp;&emsp;搭建`arm根文件系统`，主要是提供一些交叉编译时需要的第三方库，参考博客

> https://blog.csdn.net/m0_59551305/article/details/127253818

> https://blog.csdn.net/qq_36956154/article/details/100606619


&emsp;&emsp;下面记录一下操作步骤，这是融合两篇博客的方法 

```shell
    # 安装必要依赖 debootstrap就是构建的命令
    $ sudo apt-get install qemu qemu-user-static binfmt-support debootstrap

    # 创建一个build目录
    $ sudo mkdir build
    $ cd build

    # 构建文件系统的命令 
    # 格式 sudo debootstrap --arch [平台]             [发行版本代号]     [目录]               [源]
    $      sudo debootstrap --arch=arm64   --foreign    focal       linux-rootfs
    # –foreign：在与主机架构不相同时需要指定此参数，仅做初始化的解包
    # [发行版本代号] 我认为用和当前ubuntu系统相同的发行版本一样就行，使用lsb_release -a获取
    # [源] 我使用默认源，所以不填写


    # ==================注意此时还是在build目录下操作==================

    # qemu-aarch64-static是其中的关键，能在 x86_64 主机系统下 chroot 到 arm64 文件系统
    $ sudo cp -a /usr/bin/qemu-aarch64-static ~/build/linux-rootfs/usr/bin/qemu-aarch64-static

    # 此脚本有两个参数  -u 是取消挂载  -m 是挂载  ch-mount.sh脚本的内容在后面
    $ ./ch-mount.sh -m linux-rootfs/

    # 执行脚本后，没有报错会进入文件系统，显示 I have no name ，这是正常的
    I have no name!@node2:/# 

    # 初始化文件系统，会把一个系统的基础包等全部初始化
    $ debootstrap/debootstrap --second-stage

    # 初始化好了以后，退出文件系统
    $ exit

    # 再次进入时，不需要执行脚本，使用chroot命令即可
    $ sudo chroot linux-rootfs
```

&emsp;&emsp;安装完成后，需要给根文件系统换源，换国内的源安装的速度快，资源也多。虚拟机和根文件系统的源都换一下比较好

> https://blog.csdn.net/weixin_39855998/article/details/128385200

```shell
    #更新的命令
    sudo apt update
    sudo apt-get upgrade
```

<details>

<summary> 上文提到的ch-mount.sh脚本内容 </summary>

```shell
    #!/bin/bash

    function mnt() {
        echo "MOUNTING"
        sudo mount -t proc /proc ${2}proc
        sudo mount -t sysfs /sys ${2}sys
        sudo mount -o bind /dev ${2}dev
        sudo mount -o bind /dev/pts ${2}dev/pts		
        sudo chroot ${2}
    }

    function umnt() {
        echo "UNMOUNTING"
        sudo umount ${2}proc
        sudo umount ${2}sys
        sudo umount ${2}dev/pts
        sudo umount ${2}dev

    }


    if [ "$1" == "-m" ] && [ -n "$2" ] ;
    then
        mnt $1 $2
    elif [ "$1" == "-u" ] && [ -n "$2" ];
    then
        umnt $1 $2
    else
        echo ""
        echo "Either 1'st, 2'nd or both parameters were missing"
        echo ""
        echo "1'st parameter can be one of these: -m(mount) OR -u(umount)"
        echo "2'nd parameter is the full path of rootfs directory(with trailing '/')"
        echo ""
        echo "For example: ch-mount -m /media/sdcard/"
        echo ""
        echo 1st parameter : ${1}
        echo 2nd parameter : ${2}
    fi
```
</details>


---

## 交叉编译工具链的准备

&emsp;&emsp;交叉编译要注意C和C++库版本问题，主要是向目标机器(**最终要运行程序的机器**)看齐，多花一些时间把目标机器开发环境的版本搞清楚，找到对应版本的交叉编译链，后面的大方向大多是正确的。

&emsp;&emsp;主要就是看目标机和交叉编译链工具的版本，也就是看`libc.so.6`支持的版本。

```shell
    strings /lib/aarch64-linux-gnu/libc.so.6 |grep GLIBC_
```

&emsp;&emsp;查看结果，可以支持的版本有`2.17、2.18、2.22、2.23`，也就是说我们要找的交叉编译链gcc中的那个库的版本符合其中的一个就行，这样提高了兼容性。

```shell
    xxx@xxx:/lib/aarch64-linux-gnu$ strings libc.so.6 | grep GLIBC_
    GLIBC_2.17
    GLIBC_2.18
    GLIBC_2.22
    GLIBC_2.23
    GLIBC_PRIVATE
```

&emsp;&emsp;自己制作编译链太复杂，我这次是在下面这个网站找的

> https://releases.linaro.org/components/toolchain/binaries/   旧版本

> https://snapshots.linaro.org/gnu-toolchain/ 新版本

> https://www.linaro.org/downloads/ 官方网站 :)

> https://developer.arm.com/downloads/-/gnu-a  arm开发工具下载

&emsp;&emsp;再点击进入几层目录，直接到达这个地址，选择解压后能直接用的成品(**还要注意32位、64位，大端序还是小端序，选择基于自己开发环境的系统和机器**)

> https://releases.linaro.org/components/toolchain/binaries/5.4-2017.05/aarch64-linux-gnu/

```shell
    gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz	27-Feb-2018 00:14	86.5M	open     #这个是解压后就能用的
```

&emsp;&emsp;直接下载，放到虚拟机中，解压，首先查看一下上面说到的版本，结果如下,目标机器的版本正好兼容虚拟机的版本，这应该是可以用的
```shell
    xxx@xxx:/usr/local/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/lib$ strings libc.so.6  | grep GLIBC_
    GLIBC_2.17
    GLIBC_2.18
    GLIBC_PRIVATE
```

---

## qt配置脚本以及相关配置

&emsp;&emsp;qt的`configure`脚本

```shell
    export LD_LIBRARY_PATH=/opt/build/linux-rootfs/usr/lib:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH=/opt/build/linux-rootfs/lib/aarch64-linux-gnu:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH=/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu:${LD_LIBRARY_PATH}

    export PATH=/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/:${PATH}
    export PATH=/opt/build/linux-rootfs/usr/lib/:${PATH}
    export PATH=/opt/build/linux-rootfs/lzq:${PATH}

    export PKG_CONFIG_PATH=/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/pkgconfig #<<========================这个配置在后面解决编译错误中有说明

    ./configure  -prefix  opt/qtfor5.13.2arm \
    -opensource \
    -confirm-license \
    -nomake examples \
    -nomake tests \
    -sysroot /opt/build/linux-rootfs \
    -xplatform linux-aarch64-gnu-g++ \
    -release \
    -pkg-config \
    -no-opengl
```

&emsp;&emsp;记得设置`/opt/qt-src/qt-everywhere-src-5.13.2/qtbase/mkspecs/linux-aarch64-gnu-g++`对应的编译链

```shell
    lzq@lzq-VirtualBox:/opt/qt-src/qt-everywhere-src-5.13.2/qtbase/mkspecs/linux-aarch64-gnu-g++$ cat qmake.conf
    #
    # qmake configuration for building with aarch64-linux-gnu-g++
    #

    MAKEFILE_GENERATOR      = UNIX
    CONFIG                 += incremental
    QMAKE_INCREMENTAL_STYLE = sublib

    include(../common/linux.conf)
    include(../common/gcc-base-unix.conf)
    include(../common/g++-unix.conf)

    # modifications to g++.conf
    QMAKE_CC                = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
    QMAKE_CXX               = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
    QMAKE_LINK              = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
    QMAKE_LINK_SHLIB        = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++

    # modifications to linux.conf
    QMAKE_AR                = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ar cqs
    QMAKE_OBJCOPY           = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-objcopy
    QMAKE_NM                = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-nm -P
    QMAKE_STRIP             = /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-strip
    load(qt_config)
```

---

## configure配置出现的错误以及编译出现的错误

&emsp;&emsp;这里记录的主要是配置`webengine`需要的第三方库环境,下面的操作配置都是在arm根文件系统上面进行的

### 问题一 : 没有检测到fontconfig

&emsp;&emsp;这个问题类型也是很经典，qt的config.log中提示我没有找到`fontconfig`，但是我确实安装了，此刻我就知道了是系统没有找到`fontconfig`的.pc文件，或者说.pc的文件路径不对。

&emsp;&emsp;我在arm根文件系统里面全局搜索没有找到，那么就是缺少pc文件了

&emsp;&emsp;我没有单独交叉编译`fontconfig`,因为arm根文件系统中就可以`apt install`


### 问题二 : 编译webengine错误没找到nss

&emsp;&emsp;qt configure后输出如下

<details>

<summary> webengine依赖情况 </summary>

Qt WebEngineCore:
  Qt WebEngine Widgets ................... yes
  Qt WebEngine Qml ....................... yes
  Embedded build ......................... yes
  Full debug information ................. no
  Pepper Plugins ......................... no
  Printing and PDF ....................... no
  Proprietary Codecs ..................... no
  Spellchecker ........................... yes
  Native Spellchecker .................... no
  WebRTC ................................. no
  Use System Ninja ....................... no
  Geolocation ............................ yes
  WebChannel support ..................... yes
  Use v8 snapshot ........................ yes
  Kerberos Authentication ................ no
  Extensions ............................. no
  Support qpa-xcb ........................ no
  Building v8 snapshot supported ......... yes
  Use ALSA ............................... no
  Use PulseAudio ......................... no
  Optional system libraries used:
    re2 .................................. no
    icu .................................. no
    libwebp, libwebpmux and libwebpdemux . no
    opus ................................. no
    ffmpeg ............................... no
    libvpx ............................... no
    snappy ............................... no
    glib ................................. no
    zlib ................................. no
    minizip .............................. no
    libevent ............................. no
    jsoncpp .............................. no
    protobuf ............................. no
    libxml2 and libxslt .................. no
    lcms2 ................................ no
    png .................................. no
    JPEG ................................. no
    harfbuzz ............................. no
    freetype ............................. no
  Required system libraries:                       <========对于系统的要求 都满足了，其余的先不管
    fontconfig ........................... yes
    dbus ................................. yes
    nss .................................. yes
    khr .................................. yes
    glibc ................................ yes
  Required system libraries for qpa-xcb:
    x11 .................................. yes
    libdrm ............................... no
    xcomposite ........................... no
    xcursor .............................. no
    xi ................................... no
    xtst ................................. no
Qt WebEngineQml:
  UI Delegates ........................... yes
  Test Support ........................... no

</details>

&emsp;&emsp;开始编译出现报错

```shell
    ERROR at //build/config/linux/pkg_config.gni:103:17: Script returned non-zero exit code.
        pkgresult = exec_script(pkg_config_script, args, "value")


    Package nss was not found in the pkg-config search path.
    Perhaps you should add the directory containing `nss.pc'
    to the PKG_CONFIG_PATH environment variable
    No package 'nss' found

    Traceback (most recent call last):  <<====== 这个好像是堆栈分析

    File "/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/3rdparty/chromium/build/config/linux/pkg-config.py", line 288, in <module>
        sys.exit(main())
    File "/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/3rdparty/chromium/build/config/linux/pkg-config.py", line 171, in main
        prefix = GetPkgConfigPrefixToStrip(options, args)
    File "/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/3rdparty/chromium/build/config/linux/pkg-config.py", line 89, in GetPkgConfigPrefixToStrip
        "--variable=prefix"] + args, env=os.environ)
    File "/usr/lib/python2.7/subprocess.py", line 223, in check_output
        raise CalledProcessError(retcode, cmd, output=output)
    subprocess.CalledProcessError: Command '['/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/pkg-config_wrapper.sh', '--variable=prefix', 'nss']' returned non-zero exit status 1
```

&emsp;&emsp;从上面的日志中发现，是执行`pkg-config_wrapper.sh`出现错误，但提示为了找到`nss`需要设置`PKG_CONFIG_PATH`，所以在`make`前再设置一下环境变量

```shell
    # 注意这里设置的是nss.pc所在的目录，注意是arm根文件系统下的nss.pc
    export PKG_CONFIG_PATH=/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/pkgconfig
```


### 问题三 : 编译webengine时ninja报错

```shell
   ninja: build stopped: subcommand failed.
```

&emsp;&emsp;先是设置了一下解除Linux系统的最大进程数和最大文件打开数限制，然后重启一下，发现没用

&emsp;&emsp;又看了一下编译错误的日志，发现了一句

```shell
    subprocess.CalledProcessError: Command '['gperf', '--key-positions=*', '-D', '-s', '2']' returned non-zero exit status 127
```

搞了这么久的交叉编译，直觉告诉我可能是没有找到`gperf`，但是arm根文件系统已经安装了`gperf`，于是我就在本地linux系统上进行安装`gperf`

```shell
   sudo apt install gperf
```

&emsp;&emsp;如法炮制，我又安装了`bison`

&emsp;&emsp;然后继续编译，编译报错

```shell
    aarch64-linux-gnu-g++: internal compiler error: Killed (program cc1plus)
```

开始我以为交叉编译器有问题，然后我又多试了几次发现虚拟机的内存消耗和cpu占用很高，于是我重新设置了虚拟机的内存和cpu核数(16G内存 6个核)，在编译的时候执行`make -j 4`，同时我在网上查了一下还有可能是系统对最大进程数和文件数有限制，顺便把限制也解除了，如下，设置完了记得重启电脑生效

```shell
    #解除Linux系统的最大进程数和最大文件打开数限制

    vim /etc/security/limits.conf

    # 添加如下的行
    *  soft  nproc  655350
    *  hard  nproc  655350
    *  soft  nofile 655350
    *  hard  nofile 655350
```

&emsp;&emsp;继续编译，还是有错误，主要是`libpthread`和`libdl`库加载后，在编译时找不到符号

```shell
    #大概是这样的报错
    platform_jumbo_43.cc:(.text+0x1ddc): additional relocation overflows omitted from the output
    /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libpthread.a(pthread_create.o): In function `create_thread':
    /build/glibc-hV2QTJ/glibc-2.31/nptl/../sysdeps/unix/sysv/linux/createthread.c:63: undefined reference to `__aarch64_cas4_acq'
    /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libpthread.a(pthread_create.o): In function `setxid_mark_thread':
    /build/glibc-hV2QTJ/glibc-2.31/nptl/allocatestack.c:998: undefined reference to `__aarch64_cas4_acq'
    /build/glibc-hV2QTJ/glibc-2.31/nptl/allocatestack.c:973: undefined reference to `__aarch64_cas4_acq'
```

&emsp;&emsp;我这里修改的方法是编辑报错项目中的Makefile中的`LIBS`项，将原来的`-plthread`(还有可能是`-ldl`)修改成指定路径

```shell
   #大概如此
   LIBS += /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libpthread-2.31.so  /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libdl-2.31.so
```

### 问题四 : qtwebengine/src/core编译错误

&emsp;&emsp;下面是编译报错，我看是`Makefile.core_module`，真不会了

```shell
ore_a.rsp -Wl,--end-group -Wl,-z,noexecstack -Wl,--fatal-warnings -fPIC -Wl,-z,now -Wl,-z,relro -Wl,-z,defs --sysroot=../../../../../../build/linux-rootfs -L../../../../../../build/linux-rootfs/usr/local/lib/aarch64-linux-gnu -Wl,-rpath-link=../../../../../../build/linux-rootfs/usr/local/lib/aarch64-linux-gnu -L../../../../../../build/linux-rootfs/lib/aarch64-linux-gnu -Wl,-rpath-link=../../../../../../build/linux-rootfs/lib/aarch64-linux-gnu -L../../../../../../build/linux-rootfs/usr/lib/aarch64-linux-gnu -Wl,-rpath-link=../../../../../../build/linux-rootfs/usr/lib/aarch64-linux-gnu -L../../../../../../build/linux-rootfs/usr/local/lib -Wl,-rpath-link=../../../../../../build/linux-rootfs/usr/local/lib --sysroot=/opt/build/linux-rootfs -Wl,--enable-new-dtags -Wl,-rpath=/opt/qt-src/qt-everywhere-src-5.13.2/opt/qtfor5.13.2arm/lib -Wl,-whole-archive -lqtwebenginecoreapi -Wl,-no-whole-archive -Wl,--no-undefined -Wl,--version-script,QtWebEngineCore.version -Wl,--enable-new-dtags -Wl,-z,origin -Wl,-rpath,\$ORIGIN -shared -Wl,-soname,libQt5WebEngineCore.so.5 -o libQt5WebEngineCore.so.5.13.2   /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libpthread-2.31.so  /opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/libdl-2.31.so /opt/qt-src/qt-everywhere-src-5.13.2/qtdeclarative/lib/libQt5Quick.so /opt/qt-src/qt-everywhere-src-5.13.2/qtbase/lib/libQt5Gui.so /opt/qt-src/qt-everywhere-src-5.13.2/qtwebchannel/lib/libQt5WebChannel.so /opt/qt-src/qt-everywhere-src-5.13.2/qtdeclarative/lib/libQt5Qml.so /opt/qt-src/qt-everywhere-src-5.13.2/qtbase/lib/libQt5Network.so /opt/qt-src/qt-everywhere-src-5.13.2/qtlocation/lib/libQt5Positioning.so /opt/qt-src/qt-everywhere-src-5.13.2/qtbase/lib/libQt5Core.so -L/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu -L/opt/build/linux-rootfs/usr/local/lib  -lrt -lnss3 -lnssutil3 -lsmime3 -lplds4 -lplc4 -lnspr4 -lresolv -lfontconfig -lexpat -lm -lz -ldbus-1 -L/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/api/debug   

/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `void blink::(anonymous namespace)::Pack<47, 1, float, unsigned short>(float const*, unsigned short*, unsigned int) [clone .isra.31]':
platform_jumbo_43.cc:(.text+0x1b20): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
platform_jumbo_43.cc:(.text+0x1b3c): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6
/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `void blink::(anonymous namespace)::Unpack<18, unsigned char, unsigned int>(unsigned char const*, unsigned int*, unsigned int) [clone .isra.156]':
platform_jumbo_43.cc:(.text+0x1b94): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
platform_jumbo_43.cc:(.text+0x1bb0): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6
/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `void blink::(anonymous namespace)::FormatConverter::Convert<(blink::WebGLImageConversion::DataFormat)7, (blink::WebGLImageConversion::DataFormat)7, (blink::WebGLImageConversion::AlphaOp)0>() [clone .isra.224]':
platform_jumbo_43.cc:(.text+0x1c08): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
platform_jumbo_43.cc:(.text+0x1c24): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6
/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `operator new(unsigned long, NotNullTag, void*) [clone .isra.277]':
platform_jumbo_43.cc:(.text+0x1c8c): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
platform_jumbo_43.cc:(.text+0x1cb0): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6
/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `blink::GraphicsContext::Save() [clone .part.480]':
platform_jumbo_43.cc:(.text+0x1d30): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
platform_jumbo_43.cc:(.text+0x1d60): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6
/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `blink::GraphicsContext::SaveLayer(SkRect const*, cc::PaintFlags const*) [clone .part.481]':
platform_jumbo_43.cc:(.text+0x1ddc): additional relocation overflows omitted from the output
collect2: error: ld returned 1 exit status
make[4]: *** [Makefile.core_module:94: ../../lib/libQt5WebEngineCore.so.5.13.2] Error 1
make[4]: Leaving directory '/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core'
make[3]: *** [Makefile:124: sub-core_module-pro-make_first] Error 2
make[3]: Leaving directory '/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core'
make[2]: *** [Makefile:77: sub-core-make_first] Error 2
make[2]: Leaving directory '/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src'
make[1]: *** [Makefile:49: sub-src-make_first] Error 2
make[1]: Leaving directory '/opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine'
make: *** [Makefile:1034: module-qtwebengine-make_first] Error 2

```

&emsp;&emsp;看报错的日志，是关于一个中间文件的问题，就是`platform_jumbo_43.o`，所以从这里入手看看吧，可以注意到报错信息中有`.text+0xxxxx`这种信息，

> .text 代码段 : 常是指用来存放程序执行代码的一块内存区域。这部分区域的大小在程序运行前就已经确定，并且内存区域通常属于只读, 某些架构也允许代码段为可写，即允许修改程序。在代码段中，也有可能包含一些只读的常数变量，例如字符串常量等。

&emsp;&emsp;直接使用工具查看中间文件对应位置的内容是什么

> 参考博客 https://www.cnblogs.com/baiduboy/p/7061365.html

```shell
# 反汇编platform_jumbo_43.o中的text段内容，并尽可能用源代码形式表示： 

# 注意！！！这个中间文件是使用交叉编译器生成的，所以下面的命令应该在arm架构下执行

objdump -j .text -S platform_jumbo_43.o
```

&emsp;&emsp;别看上面日志那么多行，其实都是重复的，主要就是下面两行，我去，这都干到汇编了:(

```
    # 问题
    /opt/qt-src/qt-everywhere-src-5.13.2/qtwebengine/src/core/debug/obj/third_party/blink/renderer/platform/platform/platform_jumbo_43.o: In function `void blink::(anonymous namespace)::Pack<47, 1, float, unsigned short>(float const*, unsigned short*, unsigned int) [clone .isra.31]':
    platform_jumbo_43.cc:(.text+0x1b20): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4' defined in .text section in /opt/arm-libc/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/5.4.1/../../../../aarch64-linux-gnu/lib/../lib64/libstdc++.so
    platform_jumbo_43.cc:(.text+0x1b3c): relocation truncated to fit: R_AARCH64_CALL26 against symbol `__stack_chk_fail@@GLIBC_2.17' defined in .text section in /opt/build/linux-rootfs/lib/aarch64-linux-gnu/libc.so.6

    # 执行 objdump -j .text -S platform_jumbo_43.o  找到对应的位置
    ==>    1b20:	94000000 	bl	0 <_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc>
           1b24:	aa1303e0 	mov	x0, x19
           1b28:	94000000 	bl	0 <_ZN7logging10LogMessageD1Ev>
           1b2c:	f940dfa1 	ldr	x1, [x29, #440]
           1b30:	f9400280 	ldr	x0, [x20]
           1b34:	ca000020 	eor	x0, x1, x0
           1b38:	b4000040 	cbz	x0, 1b40 <_ZN5blink12_GLOBAL__N_14PackILi46ELi1EhhEEvPKT1_PT2_j.isra.29+0x68>
    ==>    1b3c:	94000000 	bl	0 <__stack_chk_fail>
```

&emsp;&emsp;还有这句话也很关键

```
platform_jumbo_43.cc:(.text+0x1b20): relocation truncated to fit: R_AARCH64_CALL26 against symbol `std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*)@@GLIBCXX_3.4'

这里面的 GLIBCXX_3.4 很让我奇怪，我已经觉得目前选用的交叉编译链有问题了，起码是版本上不对应了
```

&emsp;&emsp;后面又查了一下，交叉编译链gcc是7.5，而arm文件系统的gcc是9.4版本，我决定换一个新的交叉编译链


## 软件对应的pc文件没有怎么办

&emsp;&emsp;前期对于这个问题我是很困惑的，我发现自己使用`apt-install`安装的软件都是没有pc文件的，这就导致qt在configure时使用pkg-config找不到要依赖的程序，如果必须依赖的程序找不到，那么qt就无法编译

&emsp;&emsp;刚开始我就是直接找源码交叉编译，但是简单的还行，一到复杂的配置就歇菜了，我还是决定从为什么没有pc文件这个角度入手

&emsp;&emsp;最终在这篇博客下找到了答案

> https://www.5axxw.com/questions/content/5vk5py

`由于这些是编译库所需的文件，因此它们往往位于相应的*-dev包中。openssl.pc包含在包openssl-dev中 libvlc.pc包含在包vlc-dev中`

> https://www.coder.work/article/4055765

`*-dev 包通常包含与库接口(interface)相关的 header 。其次最常见的是描述构建选项和静态链接库的包配置文件 (*.pc)。`

&emsp;&emsp;这一下就点醒了我，我记得每次安装软件的时候确实有一些*-dev的包需要安装，只是当时并不在意

&emsp;&emsp;所以我直接在arm根文件系统安装软件对应的-dev包，不过我也不知道这些包的实际名字，不过可以用`apt-cache search xxx`模糊搜索一下

```shell
    apt-cache search fontconfig | grep dev

    apt install libfontconfig1-dev
```

&emsp;&emsp;此时还剩下最后一步，就是设置对`pkg-config`的环境变量，确保检测时能找到pc文件

```shell
    export PKG_CONFIG_PATH=/opt/build/linux-rootfs/usr/lib/aarch64-linux-gnu/pkgconfig
```


## 反思-20230813

&emsp;&emsp;这次研究花费了三周，最终还是在webengine这里倒下了，感觉再尝试没有什么进展了，精力在这上面花费的太多了，已经影响正常工作了，值得欣慰的是，还是将自己的操作完整的记录了下来，等到下次心血来潮的时候可以从当前的进展继续前进。

&emsp;&emsp;下一次，或许可以按照李晨的方法，直接使用目标机上的文件系统库以及其他依赖。 


## 记录-20230910

&emsp;&emsp;这一次进展很大，首先是`webengine`必要的系统环境终于配置齐全了，还有一些编译中出现的问题也知道了修改的方法，其实做一件事的时候不用太急，只要能坚持做下去也能有好的结果，最怕的就是一蹶不振。

## 记录-20230917

&emsp;&emsp;又搞了几天，离成功越来越近但是出现的问题也是越来约奇怪了，还得暂停一段时间了


------


## 使用新的交叉编译链

&emsp;&emsp;这回使用这个`gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu`比较贴近9.4





https://forum.qt.io/topic/90586/qt-cross-compile-for-imx-cannot-find-crt1-o-no-such-file-or-directory/5?_=1694935483909&lang=zh-CN