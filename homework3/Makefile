TARGET = main

# 编译器
CC = g++

# 编译器参数
CFLAGS = -g -lglut -lGLU -lGL -lOpenMeshTools -lOpenMeshCore

# 源文件
SOURCES = main.cpp simplify.cpp

# 默认参数
DEFAULT_ARGS = dragon_repair simplified_dragon 0.5

# 默认目标
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

run: $(TARGET)
	./$(TARGET) $(or $(ARGS),$(DEFAULT_ARGS))

clean:
	rm -f $(TARGET)