@echo off
cd /d E:\Data\3-Projects\TV36\Production Base
set /p msg="What did you change? (commit message^): "
git add .
git commit -m "%msg%"
git push
echo.
echo Done! Code uploaded.
pause
