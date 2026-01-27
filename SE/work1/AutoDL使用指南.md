# AutoDL 部署使用指南

## 📍 项目路径

```
工作目录: /root/autodl-tmp
项目目录: /root/autodl-tmp/SE_Work1_server/work1
编译目录: /root/autodl-tmp/SE_Work1_server/work1/build
可执行文件: /root/autodl-tmp/SE_Work1_server/work1/build/bin/photo_server
```

---

## 🚀 快速部署（一键完成）

### 步骤 1：连接到 AutoDL

```bash
ssh -p 端口号 root@connect.xxx.autodl.com
```

### 步骤 2：下载并运行部署脚本

```bash
# 进入工作目录
cd /root/autodl-tmp

# 克隆项目
git clone https://github.com/ixp01/SE_Work1_server.git

# 进入项目
cd SE_Work1_server/work1

# 运行部署脚本
bash autodl_deploy.sh
```

---

## 📝 手动部署（分步执行）

### 1. 克隆项目

```bash
cd /root/autodl-tmp
git clone https://github.com/ixp01/SE_Work1_server.git
cd SE_Work1_server/work1
```

### 2. 安装编译工具

```bash
apt-get update
apt-get install -y build-essential cmake
```

### 3. 编译项目

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 4. 复制图片

```bash
cp -r ../images bin/
```

### 5. 运行服务器

```bash
cd bin
./photo_server
```

---

## 🎯 服务管理

### 启动服务

**前台运行**（测试用）:
```bash
cd /root/autodl-tmp/SE_Work1_server/work1/build/bin
./photo_server
```

**后台运行**（推荐）:
```bash
cd /root/autodl-tmp/SE_Work1_server/work1/build/bin
nohup ./photo_server > server.log 2>&1 &
```

**使用启动脚本**:
```bash
cd /root/autodl-tmp/SE_Work1_server/work1
bash 启动服务_AutoDL.sh
```

### 停止服务

**方式 1：使用脚本**
```bash
cd /root/autodl-tmp/SE_Work1_server/work1
bash 停止服务_AutoDL.sh
```

**方式 2：手动停止**
```bash
# 查找进程
ps aux | grep photo_server

# 停止进程
pkill photo_server
# 或
kill -9 <PID>
```

### 查看服务状态

```bash
# 检查进程
ps aux | grep photo_server

# 查看日志
tail -f /root/autodl-tmp/SE_Work1_server/work1/build/bin/server.log
```

---

## 🌐 访问服务

### 方式 1：SSH 端口转发（推荐）⭐⭐⭐⭐⭐

**在本地电脑**新开一个终端：

```bash
ssh -p AutoDL端口 -L 6006:localhost:6006 root@connect.xxx.autodl.com
```

保持这个连接，然后在浏览器访问：
```
http://localhost:6006
```

### 方式 2：AutoDL 自定义服务

1. 登录 AutoDL 控制台
2. 找到你的实例 → 自定义服务
3. 添加服务：
   - 名称：photo-server
   - 端口：6006
4. 获取公网地址
5. 在浏览器访问该地址

### 方式 3：在服务器上测试

```bash
# 测试服务是否正常
curl http://localhost:6006

# 下载随机图片
curl http://localhost:6006/photo -o test.jpg
```

---

## 🔄 更新代码

### 方式 1：Git 拉取

```bash
cd /root/autodl-tmp/SE_Work1_server
git pull
cd work1/build
cmake --build .
```

### 方式 2：重新克隆

```bash
cd /root/autodl-tmp
rm -rf SE_Work1_server
git clone https://github.com/ixp01/SE_Work1_server.git
cd SE_Work1_server/work1
bash autodl_deploy.sh
```

---

## 📊 常用命令

### 系统信息

```bash
# 查看系统资源
htop
# 或
top

# 查看磁盘使用
df -h

# 查看内存使用
free -h
```

### 进程管理

```bash
# 查看所有进程
ps aux

# 查看特定进程
ps aux | grep photo_server

# 实时监控进程
watch -n 1 'ps aux | grep photo_server'
```

### 日志管理

```bash
# 实时查看日志
tail -f /root/autodl-tmp/SE_Work1_server/work1/build/bin/server.log

# 查看最后 50 行
tail -50 /root/autodl-tmp/SE_Work1_server/work1/build/bin/server.log

# 搜索日志
grep "ERROR" /root/autodl-tmp/SE_Work1_server/work1/build/bin/server.log
```

---

## ❓ 常见问题

### Q1: 编译失败 "g++: command not found"

**解决**:
```bash
apt-get update
apt-get install -y build-essential
```

### Q2: 编译失败 "cmake: command not found"

**解决**:
```bash
apt-get install -y cmake
```

### Q3: 图片不显示

**解决**:
```bash
# 确保 images 目录在可执行文件同级
cd /root/autodl-tmp/SE_Work1_server/work1/build/bin
ls -la images/

# 如果不存在，复制过去
cp -r ../../images ./
```

### Q4: 端口被占用

**解决**:
```bash
# 查找占用端口的进程
lsof -i :6006

# 停止进程
kill -9 <PID>
```

### Q5: 服务意外停止

**解决**:
```bash
# 查看日志
tail -50 /root/autodl-tmp/SE_Work1_server/work1/build/bin/server.log

# 重新启动
cd /root/autodl-tmp/SE_Work1_server/work1
bash 启动服务_AutoDL.sh
```

---

## 🎓 完整操作流程

### 首次部署

```bash
# 1. 连接到 AutoDL
ssh -p PORT root@connect.xxx.autodl.com

# 2. 克隆并部署
cd /root/autodl-tmp
git clone https://github.com/ixp01/SE_Work1_server.git
cd SE_Work1_server/work1
bash autodl_deploy.sh

# 3. 启动服务
bash 启动服务_AutoDL.sh

# 4. 在本地电脑新开终端，端口转发
ssh -p PORT -L 6006:localhost:6006 root@connect.xxx.autodl.com

# 5. 浏览器访问
# http://localhost:6006
```

### 日常使用

```bash
# 启动服务
cd /root/autodl-tmp/SE_Work1_server/work1
bash 启动服务_AutoDL.sh

# 查看日志
tail -f build/bin/server.log

# 停止服务
bash 停止服务_AutoDL.sh
```

---

**祝你部署顺利！** 🚀


