# 二次压降检测仪检定信息管理系统

## 系统概述
本系统用于管理互感器二次压降检测仪的检定数据，实现数据输入、管理、统计和报告生成等功能。

## 核心功能

### 1. 数据管理
- **厂商信息管理**: 记录设备制造商信息
- **设备信息管理**: 管理被检测设备的基本信息
- **检测记录管理**: 记录每次检测的基础信息（日期、检测员、环境条件等）
- **检测结果项管理**: 管理PT1/PT2/CT1/CT2检测项目及其结果

### 2. 详细测量数据
- 点击检测记录后，可查看/编辑详细测量数据
- 包含ao/bo/co三个测量点的完整数据
- 数据包括：f(%)、d(分)、dU(%)、Upt:U、Uyb:U

### 3. 数据输入
- **手动添加**: 通过对话框逐条添加各类记录
- **批量导入**: 支持CSV文件批量导入设备信息

### 4. 统计分析
- 按时间段统计检测数量
- 按厂商统计合格率
- 按月统计工作量

### 5. 报告生成
- 生成专业的PDF检测报告
- 包含设备信息、检测结果项、详细测量数据
- 支持直接打印

## 数据库设计

### manufacturers（厂商表）
- id: 主键
- name: 厂商名称
- contact_person: 联系人
- phone: 电话
- address: 地址

### equipment_info（设备信息表）
- id: 主键
- product_code: 产品编号
- product_name: 产品名称
- manufacturer_id: 厂商ID（外键）
- production_date: 生产日期
- product_location: 产地
- product_model: 产品型号

### test_records（检测记录表）
- id: 主键
- equipment_id: 设备ID（外键）
- test_date: 检测日期
- tester_name: 检测员
- test_location: 检测地点
- secondary_voltage: 二次电压
- temperature: 温度
- humidity: 湿度
- metering_point_code: 计量点编号
- test_date_code: 测试日期编号

### test_result_items（检测结果项表）
- id: 主键
- test_record_id: 检测记录ID（外键）
- item_name: 项目名称（PT1/PT2/CT1/CT2）
- gear_position: 档位（如100V、5A）
- percentage: 百分比
- data_lower_limit: 数据下限
- data_upper_limit: 数据上限
- measured_data: 实测数据
- is_qualified: 是否合格

### test_result_details（详细测量数据表）
- id: 主键
- test_record_id: 检测记录ID（外键）
- item_name: 项目名称（PT侧/CT侧）
- ao_f, ao_d, ao_du, ao_upt, ao_uyb: ao点数据
- bo_f, bo_d, bo_du, bo_upt, bo_uyb: bo点数据
- co_f, co_d, co_du, co_upt, co_uyb: co点数据
- pt_check_note: PT侧备注
- r_percentage: r%
- measurement_result: 测量结束值

## 使用说明

### 基本操作流程
1. 添加厂商信息
2. 添加设备信息（选择厂商）
3. 添加检测记录（选择设备）
4. 添加检测结果项（PT1/PT2/CT1/CT2）
5. 查看/编辑详细测量数据
6. 生成PDF报告或打印

### 快捷键
- F5: 刷新数据
- Ctrl+N: 添加记录
- Delete: 删除选中记录

## 技术栈
- 开发框架: Qt 6.8.3
- 数据库: SQLite
- 编程语言: C++17
- PDF生成: QPdfWriter
- 打印支持: QPrinter

## 文件结构
```
ecyj/
├── main.cpp                   # 程序入口
├── mainwindow.h/cpp           # 主窗口
├── databasehelper.h/cpp       # 数据库管理
├── adddialog.h/cpp            # 添加数据对话框
├── detaildatadialog.h/cpp     # 详细数据对话框
├── csvimporter.h/cpp          # CSV导入
├── statisticsdialog.h/cpp     # 统计对话框
├── reportgenerator.h/cpp      # 报告生成
└── ecyj.pro                   # Qt项目文件
```

## 编译运行
1. 打开Qt Creator
2. 打开ecyj.pro项目文件
3. 选择合适的Kit（MinGW 64-bit）
4. 点击运行按钮

## 注意事项
- 删除检测记录时，相关的检测结果项和详细数据会级联删除
- CSV导入时请确保格式正确
- 生成PDF报告前请确保已添加完整的检测数据
