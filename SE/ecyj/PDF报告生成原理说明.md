# 检定报告生成系统原理与优化方案

## 当前实现原理

### 技术架构
```
Qt框架
├── QPdfWriter：PDF文件写入器
├── QPainter：图形绘制引擎
├── QFont：字体管理
└── QRect：矩形区域定位
```

### 生成流程
1. **初始化PDF写入器**
   - 页面大小：A4 (210mm × 297mm)
   - 分辨率：300 DPI（高清晰度）
   - 页边距：20mm

2. **坐标系统转换**
   ```cpp
   Point单位（72 DPI） → 设备像素（300 DPI）
   转换比例 = 300 / 72 = 4.167
   ```

3. **绘制顺序**
   ```
   标题栏（居中大标题 + 日期）
      ↓
   设备信息区（双列表格布局）
      ↓
   检测结果项目（7列数据表）
      ↓
   详细测量数据（6列ao/bo/co表）
      ↓
   检测结论（签字栏）
   ```

## 存在的问题及原因

### 1. 文字重叠问题
**原因：**
- 字体缩小后，行高(`lineHeight`)和间距(`y += value`)没有同步调整
- 使用固定像素值，不同DPI下显示不一致

**示例：**
```cpp
// 错误的做法
QFont font("SimSun", 10);  // 字体变小
y += 40;  // 但行高仍然是40像素 → 重叠！

// 正确的做法
QFont font("SimSun", 20);
int lineHeight = font.pixelSize() * 1.5;  // 行高 = 字体大小 × 1.5
y += lineHeight;
```

### 2. 表格对齐问题
**原因：**
- 使用 `drawText(x, y, text)` 直接绘制，基线定位不准确
- 列宽固定，不随内容自适应

**改进：**
```cpp
// 旧方法：基线定位
painter.drawText(100, y, "文本");

// 新方法：矩形区域居中
painter.drawText(QRect(100, y, 200, 50), Qt::AlignCenter, "文本");
```

### 3. 布局混乱
**原因：**
- 缺乏统一的布局网格系统
- 边距、间距不规范

## 优化方案（已实施）

### 1. 统一的布局系统
```cpp
// 定义页面布局常量
int leftMargin = 200;     // 左边距（像素）
int rightMargin = 200;    // 右边距
int topMargin = 200;      // 上边距
int lineHeight = 70;      // 标准行高
```

### 2. 响应式列宽
```cpp
// 根据页面宽度百分比分配列宽
int tableWidth = currentRect.width();
int colWidth[] = {
    int(tableWidth * 0.12),  // 项目列占12%
    int(tableWidth * 0.16),  // 数据列占16%
    // ...
};
```

### 3. 标签-内容分离布局
```cpp
// 使用两种字体：标签加粗，内容正常
QFont labelFont("SimSun", 20, QFont::Bold);
QFont contentFont("SimSun", 20);

// 每个字段分为两部分
painter.drawText(labelRect, "产品编号:");  // 标签
painter.drawText(contentRect, value);      // 内容
```

### 4. 表格规范绘制
```cpp
// 绘制单元格边框
painter.drawRect(x, y, width, height);

// 文本居中对齐
painter.drawText(
    QRect(x, y, width, height),
    Qt::AlignCenter,  // 水平垂直居中
    text
);
```

## 当前字体规格

| 区域 | 字体 | 大小 | 粗细 | 说明 |
|------|------|------|------|------|
| 标题 | SimHei | 48 | Bold | 黑体加粗 |
| 日期 | SimSun | 24 | Normal | 宋体正常 |
| 小标题 | SimSun | 28 | Bold | 章节标题 |
| 标签 | SimSun | 20 | Bold | 字段名称 |
| 内容 | SimSun | 20 | Normal | 字段值 |
| 表格标题 | SimSun | 18 | Bold | 表头 |
| 表格数据 | SimSun | 16 | Normal | 表格内容 |

## 如何进一步优化

### 建议1：增加表格边框
```cpp
// 在表格绘制时添加边框
QPen pen(Qt::black, 2);  // 2像素黑色边框
painter.setPen(pen);
painter.drawRect(x, y, width, height);
```

### 建议2：使用背景色区分
```cpp
// 表头背景色
painter.fillRect(headerRect, QColor(220, 220, 220));  // 浅灰色

// 斑马纹（隔行变色）
if (row % 2 == 0) {
    painter.fillRect(rowRect, QColor(245, 245, 245));
}
```

### 建议3：添加单位说明
```cpp
// 温度显示
QString("%1 °C").arg(temperature);

// 湿度显示  
QString("%1 %").arg(humidity);  // 注意：%需要转义为%%
```

## 测试检查清单

编译运行后，检查以下项目：

- [ ] 标题居中且不与其他内容重叠
- [ ] 设备信息双列对齐
- [ ] 检测结果表格列宽均匀，文字居中
- [ ] 详细数据表格数字对齐
- [ ] 签字栏有足够空间
- [ ] 整体布局协调，间距合理

## 常见问题排查

**Q: 字体还是重叠？**
A: 检查 `y += lineHeight` 的累加是否正确，确保每次绘制后都更新y坐标。

**Q: 表格超出页面？**
A: 检查 `colWidth` 数组的总和是否超过 `tableWidth`。

**Q: 文字显示不全？**
A: 增大 `QRect` 的高度参数，给文字更多垂直空间。

**Q: PDF中文乱码？**
A: 确保使用支持中文的字体（SimSun、SimHei、Microsoft YaHei等）。

## 下一步优化建议

1. **添加页眉页脚**：显示页码、单位名称
2. **支持多页**：当内容超过一页时自动分页
3. **添加水印**："检定证书"或单位LOGO
4. **打印预览**：导出前预览效果
5. **PDF元数据**：添加作者、标题、创建日期等信息
