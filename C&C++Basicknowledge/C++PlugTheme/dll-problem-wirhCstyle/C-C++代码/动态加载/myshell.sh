#!/bin/bash

g++ plug.cpp   -fPIC -shared -o libplug.so
g++ plug2.cpp  -fPIC -shared -o libplug2.so

g++ main.cpp -rdynamic -ldl -o main.out 
