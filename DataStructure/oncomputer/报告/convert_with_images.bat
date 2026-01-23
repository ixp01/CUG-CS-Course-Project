@echo off
chcp 65001 > nul
echo ==========================================
echo 🖼️  Markdown转Word - 图片增强版
echo 支持Mermaid图表转换为真实图片
echo ==========================================
echo.

python convert_md_to_docx_with_images.py

echo.
echo 按任意键退出...
pause > nul 