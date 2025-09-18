@echo off

REM 编译photo_watermark.cpp程序
REM 支持Visual Studio的cl编译器和MinGW的g++编译器

REM 设置默认编译器为cl
set COMPILER=cl

REM 检查是否指定了编译器参数
if "%1"=="g++" (
    set COMPILER=g++
    echo 将使用g++编译器
) else if not "%1"=="" (
    echo 用法：build.bat [g++]
    echo   不提供参数：使用Visual Studio的cl编译器
    echo   g++：使用MinGW的g++编译器
    exit /b 1
) else (
    echo 将使用Visual Studio的cl编译器
    echo 提示：可以使用build.bat g++来使用g++编译器
)

REM 如果使用cl编译器，尝试设置Visual Studio环境
if "%COMPILER%"=="cl" (
    call :CheckVSInstallation
    if %ERRORLEVEL% neq 0 (
        echo 警告：未找到Visual Studio环境，尝试使用系统PATH中的编译器
    )
)

REM 根据选择的编译器编译程序
if "%COMPILER%"=="cl" (
    cl /EHsc photo_watermark.cpp gdiplus.lib user32.lib
) else (
    g++ -o photo_watermark.exe photo_watermark.cpp -lgdiplus -lgdi32 -luser32
)

REM 检查编译是否成功
goto :CheckCompileResult

:CheckVSInstallation
REM 检查常见的Visual Studio安装路径
set VS_PATHS=
set "VS_PATHS=%VS_PATHS%;%ProgramFiles%\Microsoft Visual Studio\2022\Community"
set "VS_PATHS=%VS_PATHS%;%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community"
set "VS_PATHS=%VS_PATHS%;%ProgramFiles%\Microsoft Visual Studio\2022\Professional"
set "VS_PATHS=%VS_PATHS%;%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Professional"
set "VS_PATHS=%VS_PATHS%;%ProgramFiles%\Microsoft Visual Studio\2019\Community"
set "VS_PATHS=%VS_PATHS%;%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community"

REM 遍历路径寻找vcvarsall.bat
for %%i in (%VS_PATHS%) do (
    if exist "%%i\VC\Auxiliary\Build\vcvarsall.bat" (
        echo 找到Visual Studio在：%%i
        call "%%i\VC\Auxiliary\Build\vcvarsall.bat" x64
        exit /b 0
    )
)

exit /b 1

:CheckCompileResult

REM 检查编译是否成功
if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    exit /b 1
)

REM 清理中间文件
if exist photo_watermark.obj del photo_watermark.obj
if exist photo_watermark.exp del photo_watermark.exp
if exist photo_watermark.lib del photo_watermark.lib

REM 编译成功提示
if exist photo_watermark.exe (
    echo 编译成功！可执行文件已生成：photo_watermark.exe
)

pause