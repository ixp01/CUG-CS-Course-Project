@echo off
chcp 65001 > nul
echo ==========================================
echo 📄 清理版 MD转DOCX工具
echo 自动去除粗体标记，生成干净格式
echo ==========================================
echo.

python convert_md_to_docx_clean.py

echo.
echo ✅ 转换完成！生成的文件：
echo    数据结构实习报告_干净版.docx
echo.
echo 🧹 清理功能说明：
echo    **线性表** → 线性表
echo    **哈希表** → 哈希表
echo    **数据结构** → 数据结构
echo    其他格式保持不变
echo.
echo 💡 字体格式：
echo    # 标题 → 宋体三号
echo    ## 标题 → 宋体四号  
echo    ### 标题 → 宋体小四
echo    正文 → 宋体五号
echo.
pause 