# 编译器
CC = gcc
# 头文件搜索目录 + 警告 + 调试 + Windows UTF8编码修复参数
CFLAGS = -I./include -Wall -g
# Windows MinGW 将UTF-8源文件转为GBK编码输出，匹配中文控制台
ifeq ($(OS),Windows_NT)
    CFLAGS += -fexec-charset=GBK
    TARGET = campuscard.exe
else
    TARGET = campuscard
endif

# 源文件、目标文件自动匹配
SRCS = $(wildcard ./src/*.c)
OBJS = $(patsubst ./src/%.c, ./obj/%.o, $(SRCS))

# 主生成目标
all: $(TARGET)

# 链接：所有.o合成可执行程序
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# 编译规则：src/*.c 生成 obj/*.o，自动创建obj文件夹
./obj/%.o: ./src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

# 自动创建obj存放中间文件
obj:
ifeq ($(OS),Windows_NT)
	mkdir obj 2>nul
else
	mkdir -p obj
endif

# 清理编译产物
clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q obj 2>nul
	del /f $(TARGET) 2>nul
else
	rm -rf obj
	rm -f $(TARGET)
endif

# 伪目标，防止和文件重名冲突
.PHONY: all clean