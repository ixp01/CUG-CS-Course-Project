/**
 * 随机图片HTTP服务 - C++版本
 * 使用 cpp-httplib 库实现
 * 参考: https://github.com/yhirose/cpp-httplib
 */

// 不定义 CPPHTTPLIB_OPENSSL_SUPPORT，默认不启用 HTTPS
#include "../include/httplib.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <random>
#include <algorithm>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

// 配置常量
const int PORT = 6006;  // 改为6006，AutoDL支持公网访问
const std::string IMAGE_DIR = "images";

// 支持的图片格式
const std::vector<std::string> SUPPORTED_EXTENSIONS = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp", ".ico"
};

/**
 * 设置控制台UTF-8编码（Windows）
 */
void setupConsoleEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);  // UTF-8
#endif
}

/**
 * 检查文件是否为支持的图片格式
 */
bool isImageFile(const fs::path& filepath) {
    std::string ext = filepath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(SUPPORTED_EXTENSIONS.begin(),
                     SUPPORTED_EXTENSIONS.end(),
                     ext) != SUPPORTED_EXTENSIONS.end();
}

/**
 * 获取目录中所有图片文件
 */
std::vector<fs::path> getImageFiles(const std::string& directory) {
    std::vector<fs::path> imageFiles;

    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "警告: 图片目录不存在: " << directory << std::endl;
            return imageFiles;
        }

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && isImageFile(entry.path())) {
                imageFiles.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "错误: 无法读取目录 " << directory << ": " << e.what() << std::endl;
    }

    return imageFiles;
}

/**
 * 读取文件内容
 */
std::string readFile(const fs::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法打开文件: " + filepath.string());
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
        );
}

/**
 * 获取文件的MIME类型
 */
std::string getMimeType(const fs::path& filepath) {
    std::string ext = filepath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".bmp") return "image/bmp";
    if (ext == ".webp") return "image/webp";
    if (ext == ".ico") return "image/x-icon";

    return "application/octet-stream";
}

/**
 * 主页HTML内容
 */
std::string getHomePageHTML() {
    return R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>随机图片服务 - C++ 版本</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            max-width: 700px;
            width: 100%;
        }
        h1 {
            color: #333;
            margin-bottom: 10px;
            font-size: 2em;
        }
        .subtitle {
            color: #666;
            margin-bottom: 30px;
            font-size: 1.1em;
        }
        h2 {
            color: #667eea;
            margin-top: 30px;
            margin-bottom: 15px;
            font-size: 1.5em;
        }
        ul {
            line-height: 2;
            margin-left: 20px;
        }
        li {
            margin-bottom: 10px;
        }
        a {
            color: #667eea;
            text-decoration: none;
            font-weight: 500;
            transition: color 0.3s;
        }
        a:hover {
            color: #764ba2;
            text-decoration: underline;
        }
        code {
            background: #f5f5f5;
            padding: 4px 8px;
            border-radius: 4px;
            font-family: 'Courier New', monospace;
            color: #e83e8c;
            font-size: 0.9em;
        }
        .btn {
            display: inline-block;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 12px 30px;
            border-radius: 25px;
            text-decoration: none;
            margin-top: 20px;
            font-weight: 600;
            transition: transform 0.3s, box-shadow 0.3s;
            box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4);
        }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(102, 126, 234, 0.6);
            color: white;
            text-decoration: none;
        }
        .footer {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #eee;
            text-align: center;
            color: #999;
            font-size: 0.9em;
        }
        .badge {
            display: inline-block;
            background: #764ba2;
            color: white;
            padding: 4px 12px;
            border-radius: 12px;
            font-size: 0.85em;
            margin-left: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📸 随机图片服务 <span class="badge">C++ 版本</span></h1>
        <p class="subtitle">基于 cpp-httplib 实现的高性能HTTP服务</p>

        <h2>✨ 使用方法</h2>
        <ul>
            <li>访问 <a href="/photo">/photo</a> 获取随机图片</li>
            <li>每次访问都会随机返回一张不同的图片</li>
            <li>支持 JPG、PNG、GIF、BMP 等多种格式</li>
        </ul>

        <h2>🧪 测试接口</h2>
        <p>
            <a href="/photo" target="_blank" class="btn">🎲 获取随机图片</a>
        </p>

        <h2>💻 API 说明</h2>
        <ul>
            <li><strong>GET /</strong> - 显示此帮助页面</li>
            <li><strong>GET /photo</strong> - 返回随机图片</li>
        </ul>

        <h2>📡 测试方法</h2>
        <ul>
            <li><strong>浏览器</strong>: 直接访问 <code>http://localhost:6006/photo</code></li>
            <li><strong>Postman</strong>: 发送 GET 请求到上述地址</li>
            <li><strong>cURL</strong>: <code>curl http://localhost:6006/photo -o image.jpg</code></li>
        </ul>

        <div class="footer">
            <p>🚀 Powered by <a href="https://github.com/yhirose/cpp-httplib" target="_blank">cpp-httplib</a></p>
            <p>软件工程课程实验 - 作业一</p>
        </div>
    </div>
</body>
</html>)";
}

/**
 * 错误页面HTML
 */
std::string getErrorPageHTML(const std::string& title, const std::string& message) {
    return "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>" + title +
           "</title><style>body{font-family:Arial;padding:50px;background:#f5f5f5;}" +
           ".error{background:white;padding:30px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}" +
           "h1{color:#e74c3c;}a{color:#3498db;text-decoration:none;}</style></head>" +
           "<body><div class=\"error\"><h1>❌ " + title + "</h1><p>" + message +
           "</p><p><a href=\"/\">← 返回首页</a></p></div></body></html>";
}

int main() {
    // 设置控制台编码
    setupConsoleEncoding();

    // 初始化随机数生成器
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 创建 HTTP 服务器
    httplib::Server server;

    // 设置日志输出
    server.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        std::cout << "[" << req.method << "] " << req.path
                  << " - Status: " << res.status << std::endl;
    });

    // 错误处理
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
        std::string error_html = getErrorPageHTML(
            "错误 " + std::to_string(res.status),
            "请求的资源不存在或发生错误"
            );
        res.set_content(error_html, "text/html; charset=utf-8");
    });

    // 路由: 首页
    server.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] 访问首页" << std::endl;
        res.set_content(getHomePageHTML(), "text/html; charset=utf-8");
    });

    // 路由: 获取随机图片
    server.Get("/photo", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] 请求随机图片" << std::endl;

        // 获取所有图片文件
        auto imageFiles = getImageFiles(IMAGE_DIR);

        if (imageFiles.empty()) {
            std::cerr << "[ERROR] 图片目录为空" << std::endl;
            res.status = 404;
            res.set_content(
                getErrorPageHTML(
                    "未找到图片",
                    "图片目录 '" + IMAGE_DIR + "' 中没有图片文件！<br>"
                                               "请在该目录中放入 .jpg、.png 等图片文件。"
                    ),
                "text/html; charset=utf-8"
                );
            return;
        }

        // 随机选择一张图片
        int randomIndex = std::rand() % imageFiles.size();
        const auto& selectedFile = imageFiles[randomIndex];

        try {
            // 读取图片文件
            std::string imageData = readFile(selectedFile);
            std::string mimeType = getMimeType(selectedFile);

            // 设置响应
            res.set_content(imageData, mimeType.c_str());

            std::cout << "[INFO] 返回图片: " << selectedFile.filename()
                      << " (大小: " << imageData.size() << " 字节, "
                      << "类型: " << mimeType << ")" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[ERROR] 读取图片失败: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(
                getErrorPageHTML(
                    "服务器错误",
                    "无法读取图片文件: " + std::string(e.what())
                    ),
                "text/html; charset=utf-8"
                );
        }
    });

    // 打印启动信息
    std::cout << "==================================================" << std::endl;
    std::cout << "🚀 随机图片HTTP服务正在启动..." << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "📂 图片目录: " << IMAGE_DIR << std::endl;
    std::cout << "🌐 访问地址: http://localhost:" << PORT << std::endl;
    std::cout << "📸 随机图片: http://localhost:" << PORT << "/photo" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "💡 使用 cpp-httplib 库 (参考 httplib.h 设计)" << std::endl;
    std::cout << "⏳ 服务器正在运行，按 Ctrl+C 停止..." << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    // 检查图片目录
    auto initialImages = getImageFiles(IMAGE_DIR);
    if (initialImages.empty()) {
        std::cout << "⚠️  警告: 图片目录中没有图片文件！" << std::endl;
        std::cout << "   请在 '" << IMAGE_DIR << "' 目录中添加图片。" << std::endl;
    } else {
        std::cout << "✅ 找到 " << initialImages.size() << " 张图片" << std::endl;
    }
    std::cout << std::endl;

    // 启动服务器
    if (!server.listen("0.0.0.0", PORT)) {
        std::cerr << "❌ 无法启动服务器！端口 " << PORT << " 可能已被占用。" << std::endl;
        return 1;
    }

    return 0;
}


