@echo off
chcp 65001 >nul
echo ====================================
echo   使用新版本Pandoc 3.7.0.2转换文档
echo ====================================
echo.

set "PANDOC_PATH=C:\Program Files\Pandoc\pandoc.exe"
set "doc_file=数据结构实习报告模板.doc"
set "docx_file=数据结构实习报告模板.docx"
set "md_file=数据结构实习报告模板.md"

echo 🔍 使用Pandoc版本:
"%PANDOC_PATH%" --version | findstr "pandoc.exe"
echo.

echo 📄 目标文件: %doc_file%
if not exist "%doc_file%" (
    echo ❌ 文件不存在: %doc_file%
    pause
    exit /b 1
)

echo.
echo ⚠️  重要提示: Pandoc无法直接读取.doc格式
echo 💡 需要先转换为.docx格式
echo.

rem 检查是否已有docx文件
if exist "%docx_file%" (
    echo ✓ 发现docx文件: %docx_file%
    echo 🔄 正在转换为markdown...
    "%PANDOC_PATH%" "%docx_file%" -o "%md_file%" --to=markdown --wrap=none --extract-media=./images
    if exist "%md_file%" (
        echo ✅ 转换成功！
        echo 📊 输出文件: %md_file%
        for %%i in ("%md_file%") do echo 📊 文件大小: %%~zi bytes
        echo.
        echo 🎉 现在可以查看转换结果了！
        pause
        exit /b 0
    ) else (
        echo ❌ 转换失败
    )
) else (
    echo 💻 请按以下步骤操作:
    echo.
    echo 步骤1: 手动转换doc为docx
    echo   选项A - 使用Microsoft Word:
    echo     1. 打开 "%doc_file%"
    echo     2. 文件 → 另存为
    echo     3. 格式选择: Word文档(*.docx)
    echo     4. 保存为 "%docx_file%"
    echo.
    echo   选项B - 在线转换:
    echo     1. 访问: https://convertio.co/doc-docx/
    echo     2. 上传 "%doc_file%"
    echo     3. 下载并重命名为 "%docx_file%"
    echo.
    echo 步骤2: 重新运行此脚本
    echo   命令: .\convert_with_new_pandoc.bat
    echo.
    echo 等待您完成转换后继续...
)

pause 