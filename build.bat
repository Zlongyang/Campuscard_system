@echo off
chcp 65001 >nul
title 校园卡系统编译脚本 build.bat

:: 1. 检查是否存在make工具（MinGW mingw32-make）
where mingw32-make >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误：未检测到MinGW的mingw32-make，请配置环境变量！
    pause
    exit /b 1
)

echo ======================================
echo        校园卡系统一键编译工具
echo ======================================
echo 1. 清理旧编译产物
echo 2. 重新编译项目
echo 3. 自动运行程序（UTF-8控制台）
echo ======================================
echo.

:: 清理旧文件
echo [1/3] 正在清理旧编译文件...
mingw32-make clean

:: 完整编译
echo [2/3] 正在编译全部源码...
mingw32-make all
if not exist campuscard.exe (
    echo ❌ 编译失败，未生成可执行文件！
    pause
    exit /b 1
)
echo ✅ 编译完成，生成 campuscard.exe
echo.

:: 启动程序，强制UTF8编码解决中文乱码
echo [3/3] 启动校园卡系统（UTF-8控制台）
echo ======================================
campuscard.exe
echo ======================================
echo 程序已退出，按任意键关闭窗口
pause >nul