#!/bin/bash

#编译脚本 生成库和执行
gcc ./plugfile/plug.c  -I./ -I./plugfile -fPIC -shared  -o  ./libplug.so
gcc ./plugfile/plug2.c -I./ -I./plugfile -fPIC -shared  -o  ./libplug2.so

gcc main.c  -L./  -lplug2  -lplug  -I./plugfile -I./   -Wl,-rpath="./"   -o main.out
