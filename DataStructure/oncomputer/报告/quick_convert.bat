@echo off
chcp 65001 > nul
echo ========================================
echo 📄 快速MD转DOCX工具
echo ========================================
echo.

python convert_md_to_docx_final.py

echo.
echo ✅ 转换完成！生成的文件：
echo    数据结构实习报告_完整版.docx
echo.
echo 💡 文件格式说明：
echo    # 标题 → 宋体三号
echo    ## 标题 → 宋体四号  
echo    ### 标题 → 宋体小四
echo    正文 → 宋体五号
echo    Mermaid图表 → 代码文本格式
echo.
pause 