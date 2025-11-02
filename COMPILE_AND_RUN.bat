@echo off
echo Compiling Online Examination System...
gcc exam_system.c -o exam_system.exe
if %errorlevel% == 0 (
    echo.
    echo Compilation successful!
    echo.
    echo Starting program...
    echo.
    exam_system.exe
) else (
    echo.
    echo Compilation failed! Please check for errors.
)
pause

