#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Doc文件转Markdown脚本
支持多种转换方式：
1. 使用Word COM对象 (Windows)
2. 使用pandoc工具
3. 使用LibreOffice命令行
"""

import os
import sys
import subprocess
from pathlib import Path

def convert_with_word_com(doc_path, md_path):
    """
    使用Word COM对象转换 (需要安装Microsoft Word)
    """
    try:
        import win32com.client
        
        # 创建Word应用程序对象
        word = win32com.client.Dispatch("Word.Application")
        word.Visible = False
        
        # 打开文档
        doc = word.Documents.Open(str(doc_path.absolute()))
        
        # 先转换为docx格式（Word COM不能直接转markdown）
        temp_docx = doc_path.with_suffix('.docx')
        doc.SaveAs2(str(temp_docx.absolute()), FileFormat=16)  # 16 = docx格式
        
        # 关闭文档和Word
        doc.Close()
        word.Quit()
        
        print(f"✓ 已转换为临时docx文件: {temp_docx}")
        
        # 使用pandoc转换docx到markdown
        if convert_with_pandoc(temp_docx, md_path):
            # 删除临时文件
            temp_docx.unlink()
            return True
        else:
            print(f"保留临时文件: {temp_docx}")
            return False
            
    except ImportError:
        print("❌ 需要安装 pywin32: pip install pywin32")
        return False
    except Exception as e:
        print(f"❌ Word COM转换失败: {e}")
        return False

def convert_with_pandoc(input_path, md_path):
    """
    使用pandoc工具转换
    """
    try:
        cmd = [
            'pandoc', 
            str(input_path),
            '-o', str(md_path),
            '--to=markdown',
            '--wrap=none',
            '--extract-media=./images'
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"✓ Pandoc转换成功: {md_path}")
            return True
        else:
            print(f"❌ Pandoc转换失败: {result.stderr}")
            return False
            
    except FileNotFoundError:
        print("❌ 未找到pandoc，请安装: https://pandoc.org/installing.html")
        return False
    except Exception as e:
        print(f"❌ Pandoc转换出错: {e}")
        return False

def convert_with_libreoffice(doc_path, output_dir):
    """
    使用LibreOffice命令行转换
    """
    try:
        # 先转换为docx
        cmd = [
            'soffice', 
            '--headless', 
            '--convert-to', 'docx',
            '--outdir', str(output_dir),
            str(doc_path)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode == 0:
            docx_path = output_dir / f"{doc_path.stem}.docx"
            md_path = output_dir / f"{doc_path.stem}.md"
            
            print(f"✓ LibreOffice转换为docx成功: {docx_path}")
            
            # 再用pandoc转换为markdown
            if convert_with_pandoc(docx_path, md_path):
                docx_path.unlink()  # 删除临时docx文件
                return True
            else:
                print(f"保留临时文件: {docx_path}")
                return False
        else:
            print(f"❌ LibreOffice转换失败: {result.stderr}")
            return False
            
    except FileNotFoundError:
        print("❌ 未找到LibreOffice，请安装: https://www.libreoffice.org/")
        return False
    except Exception as e:
        print(f"❌ LibreOffice转换出错: {e}")
        return False

def main():
    # 设置文件路径
    doc_file = Path("数据结构实习报告模板.doc")
    md_file = Path("数据结构实习报告模板.md")
    
    if not doc_file.exists():
        print(f"❌ 文件不存在: {doc_file}")
        return
    
    print(f"📄 准备转换: {doc_file} -> {md_file}")
    print("=" * 50)
    
    # 尝试不同的转换方法
    methods = [
        ("Word COM对象", lambda: convert_with_word_com(doc_file, md_file)),
        ("Pandoc直接转换", lambda: convert_with_pandoc(doc_file, md_file)),
        ("LibreOffice + Pandoc", lambda: convert_with_libreoffice(doc_file, Path.cwd()))
    ]
    
    for method_name, method_func in methods:
        print(f"\n🔄 尝试方法: {method_name}")
        try:
            if method_func():
                print(f"✅ 转换成功！输出文件: {md_file}")
                
                # 显示文件大小
                if md_file.exists():
                    size = md_file.stat().st_size
                    print(f"📊 文件大小: {size} bytes")
                
                return
        except Exception as e:
            print(f"❌ {method_name} 失败: {e}")
            continue
    
    print("\n💡 所有转换方法都失败了。建议:")
    print("1. 安装 pandoc: https://pandoc.org/installing.html")
    print("2. 安装 LibreOffice: https://www.libreoffice.org/")
    print("3. 确保有 Microsoft Word (用于COM方法)")
    print("4. 手动将.doc文件另存为.docx格式")

if __name__ == "__main__":
    main() 