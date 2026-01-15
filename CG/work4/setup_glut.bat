@echo off
chcp 65001 >nul
color 0B
cls

echo ============================================
echo    OpenGL/GLUT 环境配置助手
echo ============================================
echo.
echo [推荐方法] 使用NuGet安装GLUT
echo --------------------------------------------
echo 1. 在Visual Studio中打开 work4.sln
echo 2. 右键项目 -^> 管理NuGet程序包
echo 3. 搜索 'nupengl.core' 并安装
echo 4. 编译运行项目（按F5）
echo.
echo ============================================
echo    手动配置方法
echo ============================================
echo.
echo 如果NuGet不可用，请手动配置：
echo.
echo [步骤1] 下载freeglut
echo   访问: https://www.transmissionzero.co.uk/software/freeglut-devel/
echo   下载: freeglut 3.x MSVC Package
echo.
echo [步骤2] 解压并放置文件
echo   - 复制 include/GL/*.h 到项目include目录
echo   - 复制 lib/x64/*.lib 到项目lib目录
echo   - 复制 bin/x64/freeglut.dll 到项目输出目录
echo.
echo [步骤3] 配置项目属性
echo   在Visual Studio中右键项目 -^> 属性：
echo   - C/C++ -^> 常规 -^> 附加包含目录: 添加GL头文件路径
echo   - 链接器 -^> 常规 -^> 附加库目录: 添加.lib路径
echo   - 链接器 -^> 输入 -^> 附加依赖项: 
echo     opengl32.lib;glu32.lib;freeglut.lib
echo.
echo ============================================
echo    编译运行
echo ============================================
echo.
echo 1. 打开 work4.sln
echo 2. 选择 x64 平台
echo 3. 按 F5 运行
echo.
echo 预期效果：
echo   - 打开窗口显示太阳系动画
echo   - W/A/S/D/Q/E 控制视角
echo   - +/- 控制动画速度
echo   - O 显示/隐藏轨道
echo   - C 显示/隐藏彗星
echo   - R 重置视角
echo   - ESC 退出
echo.
echo ============================================
echo    常见问题
echo ============================================
echo.
echo [Q] 编译错误 - 找不到 GL/glut.h
echo [A] 检查头文件是否正确放置
echo.
echo [Q] 链接错误 - 无法解析的外部符号
echo [A] 检查是否添加了所有.lib依赖项
echo.
echo [Q] 运行错误 - 缺少 freeglut.dll
echo [A] 将 freeglut.dll 复制到 exe 所在目录
echo.
echo ============================================
echo.
pause
