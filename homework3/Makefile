# 可执行文件的名称
TARGET = main

# 编译器
CC = g++

# 编译器参数
CFLAGS = -g -lglut -lGLU -lGL -lOpenMeshTools -lOpenMeshCore

# 源文件
SOURCES = main.cpp simplify.cpp

# 默认目标
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)