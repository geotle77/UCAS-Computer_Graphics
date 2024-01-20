# 编译器
CC = g++

# 编译器参数
CFLAGS = -g -lglut -lGLU -lGL -pthread 

# 源文件
SOURCES = src/main.cpp src/ray_tracing.cpp

# 目标文件
TARGET = main

# 默认参数
DEFAULT_ARG = NO

# 默认目标
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(DEFAULT_ARG)

run-gif: $(TARGET)
	./$(TARGET) YES
	python3 src/creategif.py

clean:
	rm -f $(TARGET)