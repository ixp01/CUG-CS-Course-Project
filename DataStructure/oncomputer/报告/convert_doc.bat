@echo off
chcp 65001 >nul
echo ====================================
echo       Doc文件转换工具
echo ====================================
echo.

set "doc_file=数据结构实习报告模板.doc"
set "md_file=数据结构实习报告模板.md"

if not exist "%doc_file%" (
    echo ❌ 文件不存在: %doc_file%
    pause
    exit /b 1
)

echo 📄 找到文件: %doc_file%
echo.

echo 🔄 尝试方法1: 使用pandoc直接转换...
pandoc "%doc_file%" -o "%md_file%" --to=markdown --wrap=none 2>nul
if exist "%md_file%" (
    echo ✅ 转换成功！
    echo 📊 输出文件: %md_file%
    for %%i in ("%md_file%") do echo 📊 文件大小: %%~zi bytes
    goto :success
)

echo ❌ 方法1失败，尝试方法2: 使用LibreOffice...
echo.

rem 尝试使用LibreOffice转换为docx
soffice --headless --convert-to docx --outdir . "%doc_file%" 2>nul
if exist "数据结构实习报告模板.docx" (
    echo ✓ 已转换为docx格式
    
    rem 再使用pandoc转换为markdown
    pandoc "数据结构实习报告模板.docx" -o "%md_file%" --to=markdown --wrap=none 2>nul
    if exist "%md_file%" (
        echo ✅ 转换成功！
        echo 📊 输出文件: %md_file%
        for %%i in ("%md_file%") do echo 📊 文件大小: %%~zi bytes
        del "数据结构实习报告模板.docx" 2>nul
        goto :success
    )
)

echo ❌ 方法2失败，尝试方法3: 使用Python脚本...
echo.

python convert_doc_to_md.py
if exist "%md_file%" (
    goto :success
)

echo.
echo ❌ 所有转换方法都失败了
echo.
echo 💡 建议解决方案:
echo 1. 安装 pandoc: https://pandoc.org/installing.html
echo 2. 安装 LibreOffice: https://www.libreoffice.org/
echo 3. 手动将 .doc 文件另存为 .docx 或 .txt 格式
echo 4. 使用在线转换工具
echo.
pause
exit /b 1

:success
echo.
echo ✅ 转换完成！
echo 📄 现在可以查看 %md_file% 文件内容
echo.
pause 