#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试修复后的Mermaid代码
"""

import base64
import requests

def test_mermaid_code():
    """测试修复后的Mermaid代码"""
    
    print("🧪 测试修复后的Floyd算法Mermaid代码...")
    
    # 修复后的代码
    mermaid_code = '''graph TD
    A["初始化距离矩阵"] --> B["k = 0"]
    B --> C["i = 0"]
    C --> D["j = 0"]
    D --> E{"dist[i][k] + dist[k][j] < dist[i][j]?"}
    E -->|是| F["更新 dist[i][j] = dist[i][k] + dist[k][j]"]
    E -->|否| G["j++"]
    F --> G
    G --> H{"j < n?"}
    H -->|是| D
    H -->|否| I["i++"]
    I --> J{"i < n?"}
    J -->|是| C
    J -->|否| K["k++"]
    K --> L{"k < n?"}
    L -->|是| B
    L -->|否| M["算法结束"]'''
    
    try:
        # Base64编码
        encoded = base64.b64encode(mermaid_code.encode('utf-8')).decode('ascii')
        print(f"📝 编码长度: {len(encoded)} 字符")
        
        # 测试mermaid.ink服务
        url = f'https://mermaid.ink/img/{encoded}'
        print(f"🌐 请求URL: {url[:60]}...")
        
        response = requests.get(url, timeout=10)
        
        if response.status_code == 200:
            print(f"✅ 测试成功!")
            print(f"📊 状态码: {response.status_code}")
            print(f"📏 图片大小: {len(response.content)} bytes")
            print(f"📋 内容类型: {response.headers.get('content-type', 'unknown')}")
            return True
        else:
            print(f"❌ 测试失败!")
            print(f"📊 状态码: {response.status_code}")
            print(f"📄 响应内容: {response.text[:200]}...")
            return False
            
    except Exception as e:
        print(f"❌ 测试出错: {str(e)}")
        return False

def show_fixes():
    """显示修复方案"""
    print("\n" + "="*60)
    print("🔧 Mermaid语法错误修复方案")
    print("="*60)
    
    print("\n❌ 错误的写法:")
    print("D --> E{dist[i][k] + dist[k][j] < dist[i][j]?}")
    print("E -->|是| F[更新 dist[i][j] = dist[i][k] + dist[k][j]]")
    
    print("\n✅ 正确的写法:")
    print('D --> E{"dist[i][k] + dist[k][j] < dist[i][j]?"}')
    print('E -->|是| F["更新 dist[i][j] = dist[i][k] + dist[k][j]"]')
    
    print("\n📋 修复要点:")
    print("1. 所有节点标签必须用双引号包围")
    print("2. 特别是包含方括号 [ ] 的文本")
    print("3. 特别是包含小于号 < 的文本")
    print("4. 特别是包含加号 + 的文本")
    print("5. 使用双引号可以避免Mermaid解析器的语法冲突")
    
    print("\n🎨 进一步优化建议:")
    print("- 使用 <br/> 进行换行，提高可读性")
    print("- 添加样式 (style) 美化图表")
    print("- 简化变量名避免复杂表达式")

if __name__ == "__main__":
    show_fixes()
    print("\n" + "="*60)
    test_result = test_mermaid_code()
    print("="*60)
    
    if test_result:
        print("\n🎉 修复成功！可以在文档中使用这个代码了。")
    else:
        print("\n⚠️  如果在线服务不可用，建议使用简化版本。") 