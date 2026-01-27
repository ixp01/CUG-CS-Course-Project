# 自动清理并重新导入完整测试数据
# 使用方法：编译运行程序后，点击菜单依次导入各个CSV文件

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  二次压降检测仪数据库重置与导入指南" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "1. 启动应用程序" -ForegroundColor Yellow
Write-Host ""

Write-Host "2. 在菜单中添加清理功能（或者手动删除数据库文件 ecyj.db 重新启动）" -ForegroundColor Yellow
Write-Host ""

Write-Host "3. 依次导入以下CSV文件（顺序很重要！）：" -ForegroundColor Yellow
Write-Host "   a) 厂商信息页面 -> 导入 -> test_manufacturers_new.csv" -ForegroundColor Green
Write-Host "   b) 设备信息页面 -> 导入 -> test_equipment_new.csv" -ForegroundColor Green
Write-Host "   c) 检测记录页面 -> 导入 -> test_records_new.csv" -ForegroundColor Green
Write-Host "   d) 检测结果项页面 -> 导入 -> test_result_items_new.csv" -ForegroundColor Green
Write-Host ""

Write-Host "4. 详细数据（ao/bo/co）会在程序启动时自动插入" -ForegroundColor Yellow
Write-Host ""

Write-Host "数据文件说明：" -ForegroundColor Cyan
Write-Host "- 5个厂商" -ForegroundColor White
Write-Host "- 10个设备（ID 1-10，对应厂商ID 1-5）" -ForegroundColor White
Write-Host "- 13条检测记录（设备ID引用 1-10）" -ForegroundColor White
Write-Host "- 52条检测结果项（每条记录4项：PT1/PT2/CT1/CT2）" -ForegroundColor White
Write-Host "- 13条详细数据（自动插入，对应13条检测记录）" -ForegroundColor White
Write-Host ""

Write-Host "注意事项：" -ForegroundColor Red
Write-Host "- CSV导入时详细数据不会自动导入，需要程序启动时自动插入" -ForegroundColor White
Write-Host "- 如果要完全重置，删除 ecyj.db 文件再重新运行程序" -ForegroundColor White
Write-Host "- 导入顺序不能错，否则外键约束会失败" -ForegroundColor White
Write-Host ""

$response = Read-Host "是否要删除现有数据库文件 ecyj.db？(y/n)"
if ($response -eq 'y' -or $response -eq 'Y') {
    $dbPath = "ecyj.db"
    if (Test-Path $dbPath) {
        Remove-Item $dbPath -Force
        Write-Host "✓ 数据库文件已删除" -ForegroundColor Green
        Write-Host "  现在可以重新运行程序导入新数据" -ForegroundColor Green
    } else {
        Write-Host "! 未找到数据库文件" -ForegroundColor Yellow
    }
} else {
    Write-Host "已取消操作" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "按任意键退出..." -ForegroundColor Cyan
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
