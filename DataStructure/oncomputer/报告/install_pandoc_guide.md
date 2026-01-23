# Windows安装Pandoc指南

## 方法1：官方安装程序（推荐）

### 步骤1：下载安装程序
1. 访问官网：https://pandoc.org/installing.html
2. 点击 "Download the latest installer for Windows (x86_64)"
3. 下载 `pandoc-x.x.x-windows-x86_64.msi` 文件

### 步骤2：安装
1. 双击下载的 `.msi` 文件
2. 按照安装向导提示操作
3. 选择安装路径（默认即可）
4. 完成安装

### 步骤3：验证安装
打开命令提示符或PowerShell，输入：
```bash
pandoc --version
```
如果显示版本信息，说明安装成功。

---

## 方法2：使用Chocolatey包管理器

### 步骤1：安装Chocolatey
以管理员身份运行PowerShell，执行：
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

### 步骤2：安装Pandoc
```powershell
choco install pandoc
```

---

## 方法3：使用Scoop包管理器

### 步骤1：安装Scoop
在PowerShell中执行：
```powershell
iwr -useb get.scoop.sh | iex
```

### 步骤2：安装Pandoc
```powershell
scoop install pandoc
```

---

## 方法4：手动下载解压

### 步骤1：下载压缩包
1. 访问：https://github.com/jgm/pandoc/releases
2. 下载 `pandoc-x.x.x-windows-x86_64.zip`

### 步骤2：解压和配置
1. 解压到指定目录（如 `C:\pandoc\`）
2. 将 `pandoc.exe` 所在目录添加到系统PATH环境变量

### 添加到PATH的步骤：
1. 右键"此电脑" → 属性
2. 高级系统设置 → 环境变量
3. 系统变量中找到"Path"，点击编辑
4. 新建，输入pandoc.exe所在目录路径
5. 确定保存

---

## 验证安装成功

安装完成后，重新打开命令提示符，执行：

```bash
# 检查版本
pandoc --version

# 测试基本功能
echo "# 测试" | pandoc -f markdown -t html
```

如果能正常显示版本信息和HTML输出，说明安装成功！

---

## 常见问题解决

### 问题1：找不到pandoc命令
**解决方案：**
- 重新启动命令提示符/PowerShell
- 检查PATH环境变量是否正确设置
- 确认pandoc.exe文件存在

### 问题2：权限问题
**解决方案：**
- 以管理员身份运行安装程序
- 检查用户权限设置

### 问题3：网络问题
**解决方案：**
- 使用VPN或代理
- 手动下载安装文件方法

---

## 安装完成后的下一步

安装pandoc后，您就可以：

1. **转换您的doc文件：**
   ```bash
   pandoc "数据结构实习报告模板.doc" -o "数据结构实习报告模板.md"
   ```

2. **运行我们的转换脚本：**
   ```bash
   python convert_doc_to_md.py
   ```

3. **直接使用批处理脚本：**
   ```bash
   .\convert_doc.bat
   ``` 