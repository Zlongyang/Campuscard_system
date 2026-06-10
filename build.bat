@echo off
chcp 65001 >nul
echo 正在编译校园卡管理系统...

:: 检查gcc是否可用
gcc --version >nul 2>&1
if errorlevel 1 (
    echo 错误：未检测到gcc编译器，请先安装MinGW并配置环境变量！
    pause
    exit /b 1
)

:: 编译命令，和Makefile逻辑一致
gcc -I./include ./src/*.c -o campuscard.exe

if exist campuscard.exe (
    echo 编译成功！可执行文件：campuscard.exe
) else (
    echo 编译失败，请检查代码是否有错误。
)

pause