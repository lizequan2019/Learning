#!/bin/bash

#编译脚本 生成库和执行
g++ ./plugfile/plug.cpp  -I./ -I./plugfile -fPIC -shared -fvisibility=hidden -o  ./libplug.so
g++ ./plugfile/plug2.cpp -I./ -I./plugfile -fPIC -shared -fvisibility=hidden -o  ./libplug2.so

g++ main.cpp  -L ./  -lplug -lplug2   -I./plugfile -I./   -Wl,-rpath="./"   -o main.out
