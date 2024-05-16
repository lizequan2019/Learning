#!/bin/bash


cvt 1920 1080
# 1920x1080 59.96 Hz (CVT 2.07M9) hsync: 67.16 kHz; pclk: 173.00 MHz
Modeline "1920x1080_60.00"  173.00  1920 2048 2248 2576  1080 1083 1088 1120 -hsync +vsync

xrandr  --newmode  "1920x1080"  173.00  1920 2048 2248 2576  1080 1083 1088 1120 -hsync +vsync

xrandr  --addmode VGA-0 "1920x1080"

xrandr

xrandr --output VGA-0 --mode 1920x1080 #VGA-0 这个是显示名称