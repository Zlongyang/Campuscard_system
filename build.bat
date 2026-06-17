@echo off
title 校园卡管理系统

echo.
echo   ╔══════════════════════════════════════════╗
echo   ║       校园卡管理系统 — 一键启动         ║
echo   ╚══════════════════════════════════════════╝
echo.

:: =============================================
::  智能启动：有 gcc 就编译运行，没有就直接运行
:: =============================================

set HAS_GCC=0
where gcc >nul 2>&1 && set HAS_GCC=1

if %HAS_GCC%==1 (
    :: ── 有 gcc：重新编译后再运行 ──────────────
    echo   [模式] 检测到 gcc，重新编译...
    echo.

    :: 清理
    if exist obj rmdir /s /q obj 2>nul

    :: 编译
    gcc -I./include -Wall -O2 -static -fexec-charset=GBK ^
        src\main.c ^
        src\card_data.c ^
        src\file_io.c ^
        src\login.c ^
        src\admin_op.c ^
        src\student_op.c ^
        src\ui.c ^
        src\utils.c ^
        -o campuscard.exe

    if %errorlevel% neq 0 (
        echo   [X] 编译失败！请检查上方错误。
        pause
        exit /b 1
    )
    echo   编译成功！
    echo.
) else (
    :: ── 没有 gcc：直接运行已有 exe ────────────
    if not exist campuscard.exe (
        echo   [X] 未找到 campuscard.exe 且没有 gcc 编译器！
        echo.
        echo   请安装 MinGW-w64 后重试：
        echo     下载: https://winlibs.com
        echo     解压到 C:\mingw64 并添加 bin 目录到 PATH
        echo.
        pause
        exit /b 1
    )
    echo   [模式] 直接运行已编译好的程序
    echo.
)

:: ── 运行程序 ──────────────────────────────────
echo   ═══════════════════════════════════════════
echo.
campuscard.exe
echo.
echo   ═══════════════════════════════════════════
echo   程序已退出。
echo.
pause
