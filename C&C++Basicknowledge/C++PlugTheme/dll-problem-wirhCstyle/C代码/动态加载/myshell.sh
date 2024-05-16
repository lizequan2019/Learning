#!/bin/bash

gcc plug.c   -fPIC -shared -o libplug.so
gcc plug2.c  -fPIC -shared -o libplug2.so

gcc main.c -rdynamic -ldl -o main.out 
