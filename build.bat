@echo off
echo [BUILD] Dang quet toan bo file .cpp trong project...
cd /d "%~dp0"

setlocal enabledelayedexpansion
set "SRC="

:: Quet de quy (Recursive) tat ca cac file .cpp trong thu muc hien tai va thu muc con
for /r %%i in (*.cpp) do (
    set "SRC=!SRC! "%%i""
)

:: Neu khong tim thay file nao thi bao loi
if "!SRC!"=="" (
    echo [ERROR] Khong tim thay file .cpp nao!
    pause
    exit /b 1
)

echo [BUILD] Dang bien dich...
g++.exe -g -fdiagnostics-color=always !SRC! -I"%~dp0." -o main.exe

:: Kiem tra ket qua build
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build thanh cong! Da tao ra file main.exe
) else (
    echo [ERROR] Build that bai! Vui long doc thong bao loi mau do o tren.
    pause
    exit /b %ERRORLEVEL%
)
