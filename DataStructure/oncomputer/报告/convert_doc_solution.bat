@echo off
chcp 65001 >nul
echo ====================================
echo       Doc文件转换解决方案
echo ====================================
echo.

set "doc_file=数据结构实习报告模板.doc"
set "docx_file=数据结构实习报告模板.docx"
set "md_file=数据结构实习报告模板.md"

if not exist "%doc_file%" (
    echo ❌ 文件不存在: %doc_file%
    pause
    exit /b 1
)

echo 📄 找到文件: %doc_file%
echo 🔍 问题分析: Pandoc不支持.doc格式，只支持.docx格式
echo.

echo 💡 解决方案选择:
echo 1. 自动尝试LibreOffice转换
echo 2. 手动转换指南
echo 3. 使用在线转换工具
echo.

echo 🔄 方法1: 尝试使用LibreOffice转换...
soffice --version >nul 2>&1
if %errorlevel% == 0 (
    echo ✓ 检测到LibreOffice，正在转换...
    soffice --headless --convert-to docx --outdir . "%doc_file%"
    if exist "%docx_file%" (
        echo ✅ 成功转换为docx格式！
        echo 🔄 现在使用pandoc转换为markdown...
        pandoc "%docx_file%" -o "%md_file%" --to=markdown --wrap=none
        if exist "%md_file%" (
            echo ✅ 转换完成！
            echo 📊 输出文件: %md_file%
            for %%i in ("%md_file%") do echo 📊 文件大小: %%~zi bytes
            echo.
            echo 🗑️ 清理临时文件...
            del "%docx_file%" 2>nul
            goto :success
        )
    )
)

echo ❌ LibreOffice方法失败
echo.

echo 💡 手动解决方案:
echo.
echo 方案A - 使用Microsoft Word:
echo   1. 用Word打开 "%doc_file%"
echo   2. 点击 文件 → 另存为
echo   3. 选择格式: Word文档(*.docx)
echo   4. 保存为 "%docx_file%"
echo   5. 重新运行此脚本
echo.
echo 方案B - 安装LibreOffice:
echo   1. 下载: https://www.libreoffice.org/download/download/
echo   2. 安装LibreOffice
echo   3. 重新运行此脚本
echo.
echo 方案C - 在线转换:
echo   1. 访问: https://convertio.co/doc-docx/
echo   2. 上传 "%doc_file%"
echo   3. 下载转换后的docx文件
echo   4. 重命名为 "%docx_file%"
echo   5. 重新运行此脚本
echo.

rem 检查是否已有docx文件
if exist "%docx_file%" (
    echo 🔍 检测到docx文件存在，尝试转换...
    pandoc "%docx_file%" -o "%md_file%" --to=markdown --wrap=none
    if exist "%md_file%" (
        echo ✅ 转换完成！
        echo 📊 输出文件: %md_file%
        for %%i in ("%md_file%") do echo 📊 文件大小: %%~zi bytes
        goto :success
    )
)

echo.
echo 等待您完成上述任一方案后，重新运行此脚本...
pause
exit /b 1

:success
echo.
echo ✅ 转换成功完成！
echo 📄 生成文件: %md_file%
echo.
echo 🎉 现在可以查看转换结果了！
echo 下一步: 让AI助手查看markdown文件内容并帮您写报告
echo.
pause 