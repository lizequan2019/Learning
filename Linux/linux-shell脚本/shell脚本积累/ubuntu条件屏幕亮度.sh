#!/bin/bash

Brightnessvalue=$1
ScreenName=$( xrandr | grep -w connected | cut -d " " -f 1 ) 
echo ${ScreenName}
xrandr --output ${ScreenName} --brightness ${Brightnessvalue}