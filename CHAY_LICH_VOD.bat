@echo off
cd /d "%~dp0"
if not exist "build\TV36.exe" (
    echo Chua co build\TV36.exe. Chay build.bat, sau do chay main.exe.
    pause
    exit /b 1
)
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
"build\TV36.exe"
pause
