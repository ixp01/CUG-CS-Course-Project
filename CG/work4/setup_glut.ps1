# 配置OpenGL/GLUT环境的PowerShell脚本

Write-Host "===========================================" -ForegroundColor Cyan
Write-Host "   OpenGL/GLUT 环境配置助手" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

# 检查NuGet包管理器
Write-Host "检查NuGet配置..." -ForegroundColor Yellow

$nugetPath = Get-Command nuget.exe -ErrorAction SilentlyContinue

if ($nugetPath) {
    Write-Host "✓ NuGet 已安装" -ForegroundColor Green
    Write-Host ""
    Write-Host "推荐方法：使用NuGet安装GLUT" -ForegroundColor Cyan
    Write-Host "1. 在Visual Studio中打开 work4.sln" -ForegroundColor White
    Write-Host "2. 右键项目 -> 管理NuGet程序包" -ForegroundColor White
    Write-Host "3. 搜索 'nupengl.core' 并安装" -ForegroundColor White
    Write-Host "4. 编译运行项目" -ForegroundColor White
} else {
    Write-Host "✗ NuGet 未找到" -ForegroundColor Red
}

Write-Host ""
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host "   手动配置方法" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "如果NuGet方法不可用，请手动配置：" -ForegroundColor Yellow
Write-Host ""
Write-Host "步骤1: 下载freeglut" -ForegroundColor Green
Write-Host "  访问: https://www.transmissionzero.co.uk/software/freeglut-devel/" -ForegroundColor White
Write-Host "  下载: freeglut 3.x MSVC Package" -ForegroundColor White
Write-Host ""

Write-Host "步骤2: 解压并放置文件" -ForegroundColor Green
Write-Host "  从 freeglut/include/GL/ 复制所有头文件(.h)到:" -ForegroundColor White
Write-Host "    项目的include目录 或 系统include目录" -ForegroundColor Gray
Write-Host ""
Write-Host "  从 freeglut/lib/x64/ 复制 .lib 文件到:" -ForegroundColor White
Write-Host "    项目的lib目录 或 系统lib目录" -ForegroundColor Gray
Write-Host ""
Write-Host "  从 freeglut/bin/x64/ 复制 freeglut.dll 到:" -ForegroundColor White
Write-Host "    项目输出目录(Debug/Release)" -ForegroundColor Gray
Write-Host ""

Write-Host "步骤3: 配置项目属性" -ForegroundColor Green
Write-Host "  在Visual Studio中:" -ForegroundColor White
Write-Host "  右键项目 -> 属性 -> C/C++ -> 常规" -ForegroundColor White
Write-Host "    附加包含目录: 添加GL头文件路径" -ForegroundColor Gray
Write-Host ""
Write-Host "  链接器 -> 常规" -ForegroundColor White
Write-Host "    附加库目录: 添加.lib文件路径" -ForegroundColor Gray
Write-Host ""
Write-Host "  链接器 -> 输入" -ForegroundColor White
Write-Host "    附加依赖项: opengl32.lib;glu32.lib;freeglut.lib" -ForegroundColor Gray
Write-Host ""

Write-Host "===========================================" -ForegroundColor Cyan
Write-Host "   快速测试" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "配置完成后，在Visual Studio中：" -ForegroundColor Yellow
Write-Host "1. 打开 work4.sln" -ForegroundColor White
Write-Host "2. 选择 x64 平台（不要用 Win32）" -ForegroundColor White
Write-Host "3. 按 F5 运行" -ForegroundColor White
Write-Host ""

Write-Host "预期效果：" -ForegroundColor Green
Write-Host "  - 打开一个窗口显示太阳系动画" -ForegroundColor White
Write-Host "  - 可以用 W/A/S/D/Q/E 控制视角" -ForegroundColor White
Write-Host "  - 可以用 +/- 控制动画速度" -ForegroundColor White
Write-Host ""

Write-Host "===========================================" -ForegroundColor Cyan
Write-Host "   常见问题" -ForegroundColor Cyan
Write-Host "===========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Q: 编译错误 - 找不到 GL/glut.h" -ForegroundColor Red
Write-Host "A: 检查步骤2，确保头文件放置正确" -ForegroundColor Green
Write-Host ""

Write-Host "Q: 链接错误 - 无法解析的外部符号" -ForegroundColor Red
Write-Host "A: 检查步骤3，确保添加了所有.lib依赖项" -ForegroundColor Green
Write-Host ""

Write-Host "Q: 运行时错误 - 缺少 freeglut.dll" -ForegroundColor Red
Write-Host "A: 将 freeglut.dll 复制到 exe 所在目录" -ForegroundColor Green
Write-Host ""

Write-Host "Q: 窗口黑屏或无响应" -ForegroundColor Red
Write-Host "A: 检查显卡驱动，确保支持 OpenGL" -ForegroundColor Green
Write-Host ""

Write-Host "===========================================" -ForegroundColor Cyan
Write-Host "按任意键退出..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
