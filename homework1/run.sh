#!/bin/bash

# 获取脚本参数（简单的字符串）
str=$1

# 根据字符串设置文件名并切换到相应的目录
if [ "$str" == "test1" ]; then
    filename="computer"
    cd project1
elif [ "$str" == "test2" ]; then
    filename="color_computer"
    cd project2
elif [ "$str" == "test3" ]; then
    filename="interactivewindow"
    cd project3
elif [ "$str" == "test4" ]; then
    filename="teapot"
    cd project4
else
    echo "Unknown filename"
    exit 1
fi

# 执行编译命令并打印编译信息
g++ $filename.cpp -lglut -lGLU -lGL -o $filename

# 执行编译后的程序
./$filename