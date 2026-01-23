@echo off
chcp 65001 >nul
echo ====================================
echo       Pandoc自动安装脚本
echo ====================================
echo.

echo 正在检查pandoc是否已安装...
pandoc --version >nul 2>&1
if %errorlevel% == 0 (
    echo ✅ Pandoc已安装！
    pandoc --version
    echo.
    echo 现在可以运行转换脚本了：
    echo    .\convert_doc.bat
    pause
    exit /b 0
)

echo ❌ 未检测到pandoc，开始安装...
echo.

echo 🔄 方法1: 尝试使用Chocolatey安装...
choco --version >nul 2>&1
if %errorlevel% == 0 (
    echo ✓ 检测到Chocolatey，正在安装pandoc...
    choco install pandoc -y
    if %errorlevel% == 0 (
        echo ✅ 通过Chocolatey安装成功！
        goto :verify
    )
)

echo ❌ Chocolatey方法失败，尝试方法2...
echo.

echo 🔄 方法2: 尝试使用Scoop安装...
scoop --version >nul 2>&1
if %errorlevel% == 0 (
    echo ✓ 检测到Scoop，正在安装pandoc...
    scoop install pandoc
    if %errorlevel% == 0 (
        echo ✅ 通过Scoop安装成功！
        goto :verify
    )
)

echo ❌ 自动安装失败
echo.
echo 💡 请手动安装pandoc：
echo.
echo 方法1 - 官方安装程序（推荐）：
echo   1. 访问: https://pandoc.org/installing.html
echo   2. 下载Windows安装程序(.msi文件)
echo   3. 双击安装
echo.
echo 方法2 - 先安装Chocolatey：
echo   1. 以管理员身份运行PowerShell
echo   2. 执行: Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
echo   3. 重新运行此脚本
echo.
echo 方法3 - 先安装Scoop：
echo   1. 在PowerShell中执行: iwr -useb get.scoop.sh ^| iex
echo   2. 重新运行此脚本
echo.
pause
exit /b 1

:verify
echo.
echo 🔍 验证安装结果...
pandoc --version >nul 2>&1
if %errorlevel% == 0 (
    echo ✅ Pandoc安装成功！
    echo.
    pandoc --version
    echo.
    echo 🎉 现在可以转换doc文件了！
    echo 运行以下命令开始转换：
    echo    .\convert_doc.bat
    echo.
) else (
    echo ❌ 验证失败，请重新启动命令提示符后再试
    echo 或参考 install_pandoc_guide.md 进行手动安装
)

pause 