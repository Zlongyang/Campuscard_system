# 编译器（Windows下如果用MinGW，用gcc；Linux/macOS默认就是gcc）
CC = gcc
# 编译选项：指定头文件目录、开启警告、添加调试信息
CFLAGS = -I./include -Wall -g
# 目标可执行文件名（Windows自动生成.exe，Linux/macOS生成无后缀文件）
ifeq ($(OS),Windows_NT)
    TARGET = campuscard.exe
else
    TARGET = campuscard
endif
# 所有源文件（自动找到src目录下的所有.c文件，不用手动列）
SRCS = $(wildcard ./src/*.c)

# 默认目标：执行make就会编译
all: $(TARGET)

# 编译规则：所有源文件编译成目标程序
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# 清理规则：执行make clean删除编译产物
clean:
ifeq ($(OS),Windows_NT)
	del /f $(TARGET) *.o 2>nul || exit 0
else
	rm -f $(TARGET) *.o
endif

# 声明伪目标（避免和文件名冲突）
.PHONY: all clean