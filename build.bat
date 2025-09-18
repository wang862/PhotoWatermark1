@echo off

REM 编译photo_watermark.cpp程序
REM 确保Visual Studio的环境变量已设置，或者修改下面的路径以指向正确的vcvarsall.bat文件

REM 检查是否提供了Visual Studio路径参数
if "%1"=="" (
    echo 请提供Visual Studio的安装路径，例如：build.bat "C:\Program Files\Microsoft Visual Studio\2022\Community"
    exit /b 1
)

REM 设置Visual Studio环境变量
call "%1\VC\Auxiliary\Build\vcvarsall.bat" x64

REM 编译程序
cl /EHsc /I"%1\VC\Tools\MSVC\14.30.30705\include" /I"%1\SDK\ScopeCppSDK\SDK\include\ucrt" photo_watermark.cpp gdiplus.lib user32.lib

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