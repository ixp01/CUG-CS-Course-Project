@echo off
chcp 65001 > nul
echo ==========================================================
echo 📄 Markdown转Word文档工具
echo 支持Mermaid图表转换和自定义字体格式设置
echo ==========================================================
echo.

echo 🔍 检查Python环境...
python --version >nul 2>&1
if errorlevel 1 (
    echo ❌ 未找到Python，请先安装Python 3.6+
    pause
    exit /b 1
)
echo ✓ Python环境正常

echo.
echo 📦 检查并安装必要依赖...

echo 检查 python-docx...
python -c "import docx" >nul 2>&1
if errorlevel 1 (
    echo 📥 安装 python-docx...
    pip install python-docx
    if errorlevel 1 (
        echo ❌ python-docx 安装失败
        pause
        exit /b 1
    )
) else (
    echo ✓ python-docx 已安装
)

echo 检查 requests...
python -c "import requests" >nul 2>&1
if errorlevel 1 (
    echo 📥 安装 requests...
    pip install requests
    if errorlevel 1 (
        echo ❌ requests 安装失败
        pause
        exit /b 1
    )
) else (
    echo ✓ requests 已安装
)

echo.
echo 🚀 开始转换...
python convert_md_to_docx.py

echo.
echo 转换完成！
pause 