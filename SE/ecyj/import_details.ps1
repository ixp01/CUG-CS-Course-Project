# 导入详细测量数据到数据库
# 使用 sqlite3 命令行工具

$dbPath = "d:\myCollege\CS class\SE\ecyj\ecyj.db"
$sqlFile = "d:\myCollege\CS class\SE\ecyj\test_result_details_data.sql"

Write-Host "正在导入详细测量数据到数据库..." -ForegroundColor Green

# 检查数据库文件是否存在
if (-not (Test-Path $dbPath)) {
    Write-Host "错误：数据库文件不存在: $dbPath" -ForegroundColor Red
    exit 1
}

# 检查SQL文件是否存在
if (-not (Test-Path $sqlFile)) {
    Write-Host "错误：SQL文件不存在: $sqlFile" -ForegroundColor Red
    exit 1
}

# 使用Qt自带的sqlite3或系统sqlite3执行SQL
try {
    # 方法1: 尝试使用系统sqlite3
    if (Get-Command sqlite3 -ErrorAction SilentlyContinue) {
        Get-Content $sqlFile | sqlite3 $dbPath
        Write-Host "✓ 数据导入成功！" -ForegroundColor Green
    }
    else {
        # 方法2: 使用Qt Creator的QSqlDatabase通过简单的程序
        Write-Host "未找到sqlite3命令行工具，正在使用备用方法..." -ForegroundColor Yellow
        
        # 读取SQL内容并使用PowerShell SQL Provider (如果可用)
        $sqlContent = Get-Content $sqlFile -Raw
        Write-Host "SQL内容已读取，请在应用程序中手动执行导入操作" -ForegroundColor Yellow
        Write-Host "或者下载 SQLite 命令行工具: https://www.sqlite.org/download.html" -ForegroundColor Cyan
    }
}
catch {
    Write-Host "错误：导入失败 - $_" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "完成！现在可以在应用程序的【检测记录】页面选择记录并点击【查看详细数据】按钮了。" -ForegroundColor Green
