# JMeter 性能测试详细教程
## ASP.NET Core CRUD 页面并发测试

---

## 📋 实验目标

- 掌握 JMeter 基本使用方法
- 对 ASP.NET Core CRUD 页面执行并发测试
- 分析 TPS、响应时间、错误率等指标
- 培养 Web 性能优化意识

---

## 🔧 实验环境准备

### 1. 被测系统信息
- **系统类型**：ASP.NET Core 8 Web 应用
- **测试页面**：`/StudentPage`（学生管理页面）
- **测试 URL**：`https://localhost:7202/StudentPage`
- **技术栈**：Razor Pages + EF Core + SQL Server

### 2. 软件准备
- **JMeter**：Apache JMeter 5.x 或更高版本
- **Java**：JDK 8 或更高版本（JMeter 运行依赖）
- **ASP.NET Core 应用**：确保已正常运行

### 3. 启动被测应用
```bash
# 在你的 ASP.NET Core 项目目录下运行
dotnet run
```
**⚠️ 重要**：测试期间保持应用程序运行，不要关闭终端！

---

## 📊 测试参数配置（核心）

根据实验要求，需要进行两组对比测试：

| 参数 | 第一组测试 | 第二组测试 |
|------|-----------|-----------|
| **并发用户数** | 200 | 200 |
| **循环次数** | 2 | 2 |
| **Ramp-Up 时间** | **3 秒** | **10 秒** |
| **请求类型** | GET + POST | GET + POST |

---

## 🚀 JMeter 配置步骤（逐步详解）

### 步骤 1：启动 JMeter

1. 进入 JMeter 安装目录的 `bin` 文件夹
2. Windows 用户双击运行 `jmeter.bat`
3. 等待 JMeter GUI 界面启动

---

### 步骤 2：创建测试计划

JMeter 启动后会自动创建一个 `Test Plan`，你可以：
1. 点击左侧树形结构中的 `Test Plan`
2. 在右侧面板的 `Name` 字段修改为：`StudentPage 性能测试`

---

### 步骤 3：添加线程组（模拟并发用户）

**操作路径**：
```
右键 Test Plan → Add → Threads (Users) → Thread Group
```

**配置参数**（第一组测试）：

| 配置项 | 值 | 说明 |
|--------|-----|------|
| **Name** | `200用户并发测试-Ramp3s` | 方便识别 |
| **Number of Threads (users)** | `200` | 模拟 200 个并发用户 |
| **Ramp-Up Period (seconds)** | `3` | 3 秒内启动所有用户 |
| **Loop Count** | `2` | 每个用户执行 2 次 |

**配置说明**：
- **Ramp-Up = 3秒**：意味着 200 个用户在 3 秒内全部启动，压力上升很快
- **总请求数计算**：200 用户 × 2 次循环 × 2 个请求（GET+POST）= **800 个请求**

---

### 步骤 4：配置 HTTP 请求默认值（推荐）

这一步可以避免在每个请求中重复填写服务器信息。

**操作路径**：
```
右键 Thread Group → Add → Config Element → HTTP Request Defaults
```

**配置参数**：

| 配置项 | 值 |
|--------|-----|
| **Protocol** | `https` |
| **Server Name or IP** | `localhost` |
| **Port Number** | `7202` |

---

### 步骤 5：添加 HTTP 请求 - GET（查看学生列表）

**操作路径**：
```
右键 Thread Group → Add → Sampler → HTTP Request
```

**配置参数**：

| 配置项 | 值 | 说明 |
|--------|-----|------|
| **Name** | `GET-获取学生列表` | 请求名称 |
| **Method** | `GET` | HTTP 方法 |
| **Path** | `/StudentPage` | 页面路径 |

**截图位置**：配置完成后的界面

---

### 步骤 6：添加 HTTP 请求 - POST（创建学生）

**操作路径**：
```
右键 Thread Group → Add → Sampler → HTTP Request
```

**配置参数**：

| 配置项 | 值 | 说明 |
|--------|-----|------|
| **Name** | `POST-创建学生` | 请求名称 |
| **Method** | `POST` | HTTP 方法 |
| **Path** | `/StudentPage` | 页面路径 |

**添加请求参数**（在 Parameters 表格中点击 Add 添加）：

| Name | Value | 说明 |
|------|-------|------|
| `Name` | `张三` | 学生姓名 |
| `Age` | `20` | 学生年龄 |
| `Email` | `zhangsan@test.com` | 学生邮箱（根据你的模型调整） |

**⚠️ 防伪令牌问题（重要）**

ASP.NET Core 默认启用 CSRF 防护，直接发送 POST 请求可能会收到 **HTTP 400 错误**。

**解决方案**：

**方法 1：在代码中临时禁用（推荐用于测试）**

在你的 `StudentPage.cshtml.cs` 的 `OnPostAsync` 方法上添加：
```csharp
[IgnoreAntiforgeryToken]
public async Task<IActionResult> OnPostAsync()
{
    // 你的创建逻辑
}
```

**方法 2：在 JMeter 中提取 Token（复杂，生产环境使用）**
需要配置正则表达式提取器和 Cookie 管理器，这里暂不详述。

---

### 步骤 7：添加监听器（收集测试数据）

#### 7.1 添加"察看结果树"（用于调试）

**操作路径**：
```
右键 Thread Group → Add → Listener → View Results Tree
```

**用途**：
- 查看每个请求的详细信息
- 检查请求是否成功（绿色 = 成功，红色 = 失败）
- 查看响应数据和错误信息

#### 7.2 添加"聚合报告"（核心数据来源）

**操作路径**：
```
右键 Thread Group → Add → Listener → Summary Report
```

**用途**：
- 显示平均响应时间、吞吐量、错误率等关键指标
- **这是实验报告数据的主要来源**

#### 7.3 添加"图形结果"（可选，用于可视化）

**操作路径**：
```
右键 Thread Group → Add → Listener → Graph Results
```

**用途**：
- 以图表形式展示响应时间趋势
- 报告中可以截图使用

---

### 步骤 8：保存测试计划

**操作**：
```
File → Save Test Plan As → 保存为：StudentPage_Test_Ramp3s.jmx
```

---

## ▶️ 执行测试与数据收集

### 第一轮测试（Ramp-Up = 3 秒）

#### 1. 执行测试
1. 点击顶部工具栏的 **绿色"启动"按钮**（或按 `Ctrl + R`）
2. 观察 JMeter 界面右上角的运行状态
3. 等待测试完成（进度条到 100%）

#### 2. 查看结果树
- 点击左侧的 `View Results Tree`
- 检查请求是否成功：
  - ✅ **绿色**：请求成功
  - ❌ **红色**：请求失败，点击查看错误详情

**常见错误**：
- `HTTP 400`：防伪令牌问题，参考步骤 6 解决
- `Connection refused`：ASP.NET Core 应用未运行
- `HTTP 500`：服务器内部错误，检查应用日志

#### 3. 查看聚合报告（核心数据）

点击左侧的 `Summary Report`，你会看到如下表格：

| Label | # Samples | Average | Min | Max | Std. Dev. | Error % | Throughput | KB/sec |
|-------|-----------|---------|-----|-----|-----------|---------|------------|--------|
| GET-获取学生列表 | 400 | 125 | 45 | 350 | 32.5 | 0.00% | 87.5/sec | 125.3 |
| POST-创建学生 | 400 | 156 | 78 | 420 | 45.2 | 0.25% | 75.2/sec | 28.4 |
| **TOTAL** | **800** | **140** | **45** | **420** | **38.9** | **0.12%** | **162.7/sec** | **153.7** |

**指标解释**：

| 指标 | 英文名称 | 含义 | 报告中的重要性 |
|------|---------|------|--------------|
| **平均响应时间** | Average | 处理单个请求的平均耗时（毫秒） | ⭐⭐⭐ 核心指标 |
| **吞吐量** | Throughput | 每秒处理的请求数（TPS） | ⭐⭐⭐ 核心指标 |
| **错误率** | Error % | 失败请求的百分比 | ⭐⭐⭐ 核心指标 |
| **最小值** | Min | 最快的响应时间 | ⭐⭐ 辅助分析 |
| **最大值** | Max | 最慢的响应时间 | ⭐⭐ 辅助分析 |
| **样本数** | # Samples | 总请求数 | ⭐ 验证测试完整性 |

#### 4. 截图保存数据

**需要截图的内容**：
1. ✅ 线程组配置（显示 Ramp-Up = 3）
2. ✅ Summary Report 完整表格
3. ✅ View Results Tree（展示成功/失败状态）
4. ✅ Graph Results（如果添加了）

**截图命名建议**：
- `Test1_Ramp3s_线程组配置.png`
- `Test1_Ramp3s_聚合报告.png`
- `Test1_Ramp3s_结果树.png`

---

### 第二轮测试（Ramp-Up = 10 秒）

#### 1. 清除之前的测试结果
点击顶部工具栏的 **"清除全部"按钮**（扫把图标）或按 `Ctrl + Shift + E`

#### 2. 修改线程组配置
1. 点击左侧的 `Thread Group`
2. 将 **Ramp-Up Period** 从 `3` 修改为 `10`
3. 将 **Name** 修改为：`200用户并发测试-Ramp10s`（便于区分）

#### 3. 重新执行测试
重复"第一轮测试"的步骤 1-4

#### 4. 保存新的测试计划
```
File → Save Test Plan As → 保存为：StudentPage_Test_Ramp10s.jmx
```

---

## 📈 数据分析与对比

### 对比分析表格（示例）

| 指标 | Ramp-Up 3秒 | Ramp-Up 10秒 | 变化 | 分析 |
|------|------------|-------------|------|------|
| **平均响应时间** | 140 ms | 95 ms | ↓ 32% | Ramp-Up 时间越长，系统压力越分散，响应更快 |
| **吞吐量 (TPS)** | 162.7/s | 145.3/s | ↓ 11% | 快速施压下瞬时吞吐量更高 |
| **错误率** | 0.12% | 0.00% | ↓ 100% | 缓慢增压让系统有更多准备时间 |
| **最大响应时间** | 420 ms | 280 ms | ↓ 33% | 峰值压力减小 |

### 结论模板

**实验结论**：

1. **响应时间**：
   - Ramp-Up 时间越短（3秒），系统在短时间内承受大量并发请求，平均响应时间为 XXX ms，最大响应时间达到 XXX ms。
   - Ramp-Up 时间增加到 10 秒后，系统有更多时间处理请求，平均响应时间降低至 XXX ms。

2. **吞吐量**：
   - 快速施压（3秒）的瞬时吞吐量较高，为 XXX TPS。
   - 缓慢施压（10秒）的吞吐量略有下降，但更加稳定。

3. **错误率**：
   - Ramp-Up 3秒时出现 X% 的错误，主要原因可能是数据库连接池不足、线程池饱和等。
   - Ramp-Up 10秒时错误率降为 0%，说明系统需要适当的"预热"时间。

4. **优化建议**：
   - 增加数据库连接池大小
   - 优化 EF Core 查询（添加索引、使用异步查询）
   - 考虑使用缓存（Redis）减少数据库压力

---

## 🐛 常见问题与解决方案

### 问题 1：JMeter 启动失败
**原因**：未安装 Java 或环境变量未配置

**解决**：
```powershell
# 检查 Java 版本
java -version

# 如果未安装，下载 JDK 并配置 JAVA_HOME 环境变量
```

### 问题 2：所有请求都显示红色（失败）
**可能原因**：
- ASP.NET Core 应用未运行
- URL 配置错误（协议、端口号）
- SSL 证书问题

**解决**：
```powershell
# 确认应用正在运行
Get-Process | Where-Object {$_.ProcessName -like "*dotnet*"}

# 浏览器测试 URL 是否可访问
Start-Process "https://localhost:7202/StudentPage"
```

### 问题 3：POST 请求返回 HTTP 400
**原因**：防伪令牌验证失败

**解决**：在 PageModel 方法上添加 `[IgnoreAntiforgeryToken]`

### 问题 4：错误率很高（>10%）
**可能原因**：
- 数据库连接数不足
- 请求参数错误（必填字段缺失）
- 服务器资源耗尽

**解决**：
1. 检查 View Results Tree 的具体错误信息
2. 查看 ASP.NET Core 应用的控制台日志
3. 检查数据库连接池配置

### 问题 5：JMeter 界面卡死或无响应
**原因**：监听器占用大量内存

**解决**：
- 测试时不要打开 View Results Tree（仅调试时使用）
- 使用命令行模式运行大规模测试：
```powershell
jmeter -n -t StudentPage_Test.jmx -l results.jtl
```

---

## 📝 实验报告撰写要点

### 报告结构建议

#### 1. 实验目的
- 掌握 JMeter 性能测试工具的使用
- 分析不同并发策略对系统性能的影响

#### 2. 实验环境
- 操作系统、JMeter 版本、被测系统信息

#### 3. 实验步骤
- 简要描述 JMeter 配置过程
- 说明两组测试的参数差异

#### 4. 实验结果（重点）
- **必须包含的截图**：
  - 线程组配置（两组）
  - Summary Report（两组）
  - View Results Tree 成功/失败示例
- **必须包含的表格**：
  - 两组测试的对比数据表

#### 5. 数据分析
- 对比 Average、Throughput、Error% 的差异
- 分析 Ramp-Up 时间对性能的影响

#### 6. 结论与改进建议
- 总结实验发现
- 提出至少 2-3 条性能优化建议

---

## 🎯 实验检查清单

在提交报告前，请确认以下内容：

- [ ] 完成两组测试（Ramp-Up 3秒 和 10秒）
- [ ] 截图包含清晰的 Summary Report 数据
- [ ] 截图显示线程组配置参数（200 用户、2 次循环）
- [ ] 测试了 GET 和 POST 两种请求
- [ ] POST 请求成功执行（错误率 < 5%）
- [ ] 报告中包含对比分析表格
- [ ] 报告中有明确的结论和优化建议
- [ ] 文档格式为 PDF

---

## 📚 补充资源

### JMeter 官方文档
- 用户手册：https://jmeter.apache.org/usermanual/index.html
- 最佳实践：https://jmeter.apache.org/usermanual/best-practices.html

### ASP.NET Core 性能优化
- 微软官方文档：https://learn.microsoft.com/en-us/aspnet/core/performance/
- EF Core 性能：https://learn.microsoft.com/en-us/ef/core/performance/

---

## ✅ 总结

本教程涵盖了从环境准备、JMeter 配置、测试执行到数据分析的完整流程。核心要点：

1. **线程组配置**：200 用户、2 次循环、Ramp-Up 分别设为 3秒 和 10秒
2. **请求类型**：GET（查看列表）+ POST（创建学生）
3. **关键指标**：Average（响应时间）、Throughput（TPS）、Error%（错误率）
4. **对比分析**：不同 Ramp-Up 时间对系统性能的影响

**祝你实验顺利！** 🚀

---

**文档版本**：v1.0  
**最后更新**：2025年12月15日
