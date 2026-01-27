#!/bin/bash
# AutoDL 启动服务脚本

# 设置路径
PROJECT_DIR="/root/autodl-tmp/SE_Work1_server/work1"
BUILD_DIR="${PROJECT_DIR}/build"
BIN_DIR="${BUILD_DIR}/bin"

echo "=========================================="
echo "启动 Photo Server"
echo "=========================================="
echo ""

# 检查可执行文件是否存在
if [ ! -f "${BIN_DIR}/photo_server" ]; then
    echo "❌ 错误: 找不到可执行文件!"
    echo "请先运行部署脚本: bash autodl_deploy.sh"
    exit 1
fi

# 检查是否已经在运行
if pgrep -x "photo_server" > /dev/null; then
    echo "⚠️  服务器已经在运行!"
    echo ""
    ps aux | grep photo_server | grep -v grep
    echo ""
    read -p "是否要重启服务? (y/n): " restart
    if [ "$restart" = "y" ]; then
        echo "停止旧服务..."
        pkill -9 photo_server
        sleep 2
    else
        exit 0
    fi
fi

# 确保 images 目录存在
if [ ! -d "${BIN_DIR}/images" ]; then
    echo "复制 images 目录..."
    cp -r ${PROJECT_DIR}/images ${BIN_DIR}/
fi

# 进入可执行文件目录
cd ${BIN_DIR}

echo "启动服务器..."
nohup ./photo_server > server.log 2>&1 &

# 等待启动
sleep 2

# 检查是否启动成功
if pgrep -x "photo_server" > /dev/null; then
    echo ""
    echo "=========================================="
    echo "✅ 服务器启动成功！"
    echo "=========================================="
    echo ""
    echo "进程信息:"
    ps aux | grep photo_server | grep -v grep
    echo ""
    echo "日志位置: ${BIN_DIR}/server.log"
    echo ""
    echo "查看日志:"
    echo "  tail -f ${BIN_DIR}/server.log"
    echo ""
    echo "停止服务:"
    echo "  pkill photo_server"
    echo ""
    echo "访问服务: http://localhost:6006"
    echo "=========================================="
else
    echo ""
    echo "❌ 启动失败！查看日志:"
    tail -20 ${BIN_DIR}/server.log
fi


