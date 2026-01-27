#!/bin/bash
# AutoDL 部署脚本 - 使用实际路径

echo "=========================================="
echo "开始部署 Photo Server 到 AutoDL"
echo "=========================================="
echo ""

# 设置工作目录
WORK_DIR="/root/autodl-tmp"
PROJECT_DIR="${WORK_DIR}/SE_Work1_server/work1"
BUILD_DIR="${PROJECT_DIR}/build"

# 1. 进入工作目录
cd ${WORK_DIR}

# 2. 克隆项目（如果不存在）
if [ ! -d "SE_Work1_server" ]; then
    echo "[1/5] 克隆项目..."
    git clone https://github.com/ixp01/SE_Work1_server.git
else
    echo "[1/5] 项目已存在，拉取最新代码..."
    cd SE_Work1_server
    git pull
    cd ${WORK_DIR}
fi

# 3. 安装编译工具
echo "[2/5] 检查并安装编译工具..."
apt-get update
apt-get install -y build-essential cmake

# 4. 编译项目
echo "[3/5] 编译项目..."
cd ${PROJECT_DIR}

# 清理旧的 build（可选）
if [ -d "build" ]; then
    echo "清理旧的编译文件..."
    rm -rf build
fi

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# 5. 检查图片
echo "[4/5] 检查图片..."
if [ ! -d "${PROJECT_DIR}/images" ] || [ -z "$(ls -A ${PROJECT_DIR}/images/*.jpg 2>/dev/null)" ]; then
    echo "创建 images 目录并下载测试图片..."
    mkdir -p ${PROJECT_DIR}/images
    cd ${PROJECT_DIR}/images
    wget https://picsum.photos/800/600 -O test1.jpg
    wget https://picsum.photos/800/600 -O test2.jpg
    wget https://picsum.photos/800/600 -O test3.jpg
fi

# 6. 复制 images 到 build 目录
echo "[5/5] 复制图片到可执行文件目录..."
cp -r ${PROJECT_DIR}/images ${BUILD_DIR}/bin/

echo ""
echo "=========================================="
echo "✅ 部署完成！"
echo "=========================================="
echo ""
echo "项目路径: ${PROJECT_DIR}"
echo "编译目录: ${BUILD_DIR}"
echo "可执行文件: ${BUILD_DIR}/bin/photo_server"
echo ""
echo "=========================================="
echo "运行方式："
echo "=========================================="
echo ""
echo "【方式1】前台运行（测试用）:"
echo "  cd ${BUILD_DIR}/bin"
echo "  ./photo_server"
echo ""
echo "【方式2】后台运行（推荐）:"
echo "  cd ${BUILD_DIR}/bin"
echo "  nohup ./photo_server > server.log 2>&1 &"
echo ""
echo "【查看日志】:"
echo "  tail -f ${BUILD_DIR}/bin/server.log"
echo ""
echo "【停止服务】:"
echo "  ps aux | grep photo_server"
echo "  kill -9 <PID>"
echo ""
echo "=========================================="
echo "访问服务："
echo "=========================================="
echo ""
echo "【本地测试】:"
echo "  curl http://localhost:6006"
echo ""
echo "【SSH 端口转发】（在本地电脑执行）:"
echo "  ssh -p PORT -L 6006:localhost:6006 root@connect.xxx.autodl.com"
echo "  然后访问: http://localhost:6006"
echo ""
echo "=========================================="
