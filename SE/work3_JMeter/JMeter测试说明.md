# JMeter 性能测试使用说明

## 📋 测试计划概述

**测试文件**: `StudentApp_Performance_Test.jmx`

此测试计划用于对学生管理系统进行性能测试，覆盖完整的 CRUD 操作。

## 🎯 测试场景

### 测试配置
- **线程数**: 10 个并发用户
- **循环次数**: 10 次
- **Ramp-Up 时间**: 5 秒（即在 5 秒内启动 10 个线程）
- **请求间延时**: 500 毫秒

### 测试步骤

1. **查询学生列表** (GET /StudentPage)
   - 断言：响应码 200
   - 断言：响应内容包含 "Student"

2. **创建新学生** (POST /StudentPage/Create)
   - 参数：
     - `Student.Name`: 测试学生{时间戳}
     - `Student.Age`: 18-30 随机值
     - `Student.Email`: test{时间戳}@example.com
     - `Student.EnrollmentDate`: 当前日期
   - 断言：响应码 200
   - 提取：创建的学生 ID 供后续操作使用

3. **查看学生详情** (GET /StudentPage/Details/{id})
   - 使用上一步提取的学生 ID
   - 断言：响应码 200
   - 断言：响应内容包含 "Details"

4. **编辑学生信息** (POST /StudentPage/Edit/{id})
   - 参数：
     - `Student.Id`: 提取的学生 ID
     - `Student.Name`: 更新学生{时间戳}
     - `Student.Age`: 20-35 随机值
     - `Student.Email`: updated{时间戳}@example.com
     - `Student.EnrollmentDate`: 当前日期
   - 断言：响应码 200

5. **删除学生** (POST /StudentPage/Delete/{id})
   - 使用提取的学生 ID
   - 断言：响应码 200

## 🚀 运行步骤

### 前置条件
1. 确保 StudentApp 应用已启动并运行在 `http://localhost:5110`
2. 确保已安装 Apache JMeter（推荐 5.5 或更高版本）

### 启动应用
```powershell
cd StudentApp
dotnet run
```

### 运行测试

#### 方式一：GUI 模式（推荐用于调试）
```bash
# 打开 JMeter GUI
jmeter

# 在 GUI 中打开测试计划
File -> Open -> 选择 StudentApp_Performance_Test.jmx

# 点击绿色启动按钮运行测试
```

#### 方式二：命令行模式（推荐用于正式测试）
```bash
# 运行测试并生成 HTML 报告
jmeter -n -t StudentApp_Performance_Test.jmx -l results.jtl -e -o report

# 参数说明：
# -n: 非 GUI 模式
# -t: 测试计划文件
# -l: 结果文件（JTL 格式）
# -e: 生成报告
# -o: 报告输出目录
```

## 📊 查看结果

### GUI 模式下的监听器
测试计划包含以下监听器：
1. **查看结果树** - 查看每个请求的详细信息
2. **汇总报告** - 查看整体性能指标
3. **用表格查看结果** - 表格形式显示所有请求
4. **图形结果** - 可视化响应时间趋势
5. **聚合报告** - 详细的统计数据（平均值、中位数、吞吐量等）

### 命令行模式下的 HTML 报告
运行后会在 `report` 目录生成完整的 HTML 报告，包含：
- 测试概览
- APDEX（应用性能指数）
- 响应时间分布
- 吞吐量
- 错误率统计

## 🔧 自定义配置

### 修改测试参数
在测试计划中定义了以下用户变量，可以根据需要修改：

```
BASE_URL = localhost      # 服务器地址
PORT = 5110              # 端口号
PROTOCOL = http          # 协议
```

### 调整并发和压力
在"学生CRUD操作线程组"中可以调整：
- **线程数**: 模拟的并发用户数
- **Ramp-Up 时间**: 所有线程启动所需时间
- **循环次数**: 每个线程执行测试的次数

### 修改请求延时
"固定延时500毫秒"组件控制请求之间的等待时间，可根据需要调整。

## ⚠️ 注意事项

1. **防伪令牌**: Create.cshtml.cs 中已添加 `[IgnoreAntiforgeryToken]` 特性，允许 JMeter 直接提交表单
2. **数据清理**: 测试会创建和删除学生数据，但多次运行可能会留下一些测试数据，建议定期清理数据库
3. **性能基线**: 首次运行建议使用较小的负载（如 5 个线程），逐步增加以找到系统性能瓶颈
4. **资源监控**: 运行测试时建议同时监控数据库和应用服务器的资源使用情况

## 📈 性能指标说明

### 关键指标
- **吞吐量 (Throughput)**: 每秒处理的请求数
- **平均响应时间 (Average)**: 所有请求的平均响应时间
- **中位数 (Median)**: 50% 的请求响应时间在此值以下
- **90% Line**: 90% 的请求响应时间在此值以下
- **95% Line**: 95% 的请求响应时间在此值以下
- **错误率 (Error %)**: 失败请求的百分比

### 性能目标建议
- 平均响应时间: < 200ms
- 95% 响应时间: < 500ms
- 错误率: < 1%
- 吞吐量: 根据业务需求设定

## 🐛 常见问题

### 问题 1: 连接被拒绝
**原因**: StudentApp 未启动或端口不正确
**解决**: 检查应用是否运行，确认端口号是否为 5110

### 问题 2: 所有请求都失败
**原因**: URL 路径不正确或应用未正常响应
**解决**: 在浏览器中手动访问 http://localhost:5110/StudentPage 确认应用正常

### 问题 3: 防伪令牌错误
**原因**: Create/Edit/Delete 操作需要防伪令牌
**解决**: 确认对应的 PageModel 类中已添加 `[IgnoreAntiforgeryToken]` 特性

### 问题 4: 提取学生 ID 失败
**原因**: 正则表达式不匹配或重定向未跟随
**解决**: 检查"创建新学生"请求的响应，确认重定向路径

## 📝 扩展测试场景

### 场景 1: 仅查询压力测试
禁用 Create/Edit/Delete 请求，增加线程数和循环次数，测试查询性能。

### 场景 2: 混合负载测试
使用多个线程组，模拟不同比例的读写操作（如 80% 查询，20% 写入）。

### 场景 3: 持续稳定性测试
设置较长的测试时间（如 1 小时），使用适中的并发数，观察系统稳定性。

## 🎓 学习资源

- [Apache JMeter 官方文档](https://jmeter.apache.org/usermanual/index.html)
- [JMeter 最佳实践](https://jmeter.apache.org/usermanual/best-practices.html)
- [ASP.NET Core 性能优化](https://docs.microsoft.com/zh-cn/aspnet/core/performance/)
