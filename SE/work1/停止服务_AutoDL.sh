#!/bin/bash
# AutoDL 停止服务脚本

echo "=========================================="
echo "停止 Photo Server"
echo "=========================================="
echo ""

# 查找进程
if pgrep -x "photo_server" > /dev/null; then
    echo "找到运行中的服务:"
    ps aux | grep photo_server | grep -v grep
    echo ""
    
    # 停止服务
    echo "正在停止服务..."
    pkill -9 photo_server
    
    sleep 1
    
    # 确认已停止
    if ! pgrep -x "photo_server" > /dev/null; then
        echo ""
        echo "✅ 服务已停止"
    else
        echo ""
        echo "⚠️  服务可能未完全停止，请手动检查"
    fi
else
    echo "❌ 服务未运行"
fi

echo ""
echo "=========================================="


