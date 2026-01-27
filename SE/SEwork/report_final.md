# 目录

1. [第1章 引言](#第1章-引言)
2. [第2章 需求分析](#第2章-需求分析)
3. [第3章 结构化分析与设计](#第3章-结构化分析与设计)
4. [第4章 面向对象分析与设计](#第4章-面向对象分析与设计)
5. [第5章 用户界面设计](#第5章-用户界面设计)
6. [第6章 总结](#第6章-总结)

---

# 第1章 引言

## 1.1 背景意义

在电力计量领域，互感器的二次压降误差直接影响电能计量的准确性。传统的二次压降检定工作长期依赖人工操作，检测员需要手动记录检测仪器的读数，并人工计算误差，最后编写纸质报告。这种方式存在诸多弊端：首先，人工抄录数据效率低下且容易出现笔误；其次，纸质报告难以长期保存和快速检索；最后，缺乏有效的数据统计手段，难以对设备质量进行长期跟踪。

为了解决上述问题，开发一套“二次压降检测仪检定软件系统”具有重要的现实意义。该系统旨在通过计算机技术与检测仪器的结合，实现实验结果的自动采集（屏幕图像识别）、数据自动存储、报告自动生成以及信息的数字化管理，从而显著提升设备检定工作的工作效率与质量，确保电能计量的公正性。

## 1.2 软件需求描述

本软件系统主要服务于互感器检定实验室，核心需求如下：

*   **灵活的数据输入**：检测员能够方便地录入被检设备的基本信息（如产品编号、制造商、生产日期等）以及实验参数（如检测项目、档位、限值等）。
*   **自动化的结果采集**：系统需支持连接摄像头或直接读取设备屏幕图像，利用 OCR 技术自动识别仪器屏幕上的测量数据（如二次电压、角差、比差），减少人工干预。
*   **数据存储与管理**：所有实验数据需持久化存储在计算机中，支持对历史记录的查询、修改（仅限特定状态）和删除。
*   **智能报告生成**：系统应能根据录入的设备信息和采集的实验数据，自动生成符合规范的检测报告，并支持预览、打印和导出功能。
*   **统计分析**：提供基础的数据统计功能，如按时间段或制造商统计检测数量。

---

# 第2章 需求分析

## 2.1 整体关系框架

本系统的分析与设计遵循软件工程的标准流程，分为需求分析阶段和设计阶段。整体关系框架如图 2-1 所示。

**图 2-1 系统分析与设计整体框架**

*   **需求分析阶段**：通过用例图描述系统功能和参与者，通过活动图描述业务流程。
*   **设计阶段**：
    *   **结构化设计**：包含数据模型（ER图）、功能模型（数据流图）和行为模型（状态图）。
    *   **面向对象设计**：包含类图（静态结构）和时序图（动态交互）。

(注：此处为文字描述，具体图表见后续章节)

---

# 第3章 结构化分析与设计

本章采用结构化分析方法，从数据、功能和行为三个维度对系统进行建模。

## 3.1 数据模型 (E-R图)

数据模型用于描述系统中的实体及其相互关系。本系统主要包含设备信息、检测记录、实验数据、检测结果和采集图像五个核心实体。

**图 3-1 系统数据模型 (E-R图)**

```plantuml
@startuml 数据模型
' 使用实体关系图
hide circle
skinparam linetype ortho

entity "设备信息" as device {
  * id : string <<PK>>
  --
  * 产品编号 : string
  * 产品名称 : string
  * 制造商 : string
  * 生产日期 : date
  * 送检日期 : date
  产地 : string
  产品型号 : string
  备注 : string
}

entity "检测记录" as detection {
  * id : string <<PK>>
  --
  * device_id : string <<FK>>
  * 检测时间 : timestamp
  状态 : enum
}

entity "实验数据" as experiment {
  * id : string <<PK>>
  --
  * detection_id : string <<FK>>
  * 项目 : string
  * 档位 : string
  * 百分比 : string
  * 数据下限 : string
  * 数据上限 : string
  实测数据 : string
}

entity "检测结果" as result {
  * id : string <<PK>>
  --
  * detection_id : string <<FK>>
  * 二次电压 : string
  * 计量点编号 : string
  * 测试日期 : string
  * tanφ : string
  * 温度 : float
  * 湿度 : float
  * 测量数据 : json
  * r值 : string
}

entity "采集图像" as image {
  * id : string <<PK>>
  --
  * detection_id : string <<FK>>
  * 图像路径 : string
  * 采集时间 : timestamp
  * OCR状态 : enum
}

device ||--o{ detection : "包含"
detection ||--o{ experiment : "包含"
detection ||--|| result : "产生"
detection ||--o| image : "采集"

note right of detection
  一次检测包含：
  - 设备信息
  - 多个实验项目
  - 一份检测结果
  - 可能的采集图像
end note
@enduml
```

## 3.2 功能模型 (数据流图 DFD)

数据流图展示了数据在系统各模块间的流动过程。

**图 3-2 系统 0 层数据流图**

```plantuml
@startuml 数据流图
' 0层数据流图
skinparam componentStyle rectangle

actor "检测员" as inspector
actor "检测仪器" as device

rectangle "互感器二次压降检测仪检定系统" as system #LightBlue
database "检测数据库" as db

inspector --> system : 设备信息\n实验数据
device --> system : 屏幕图像
system --> inspector : 检测报告\n统计信息
system --> db : 存储数据
db --> system : 查询结果
@enduml
```

**图 3-3 系统 1 层数据流图**

```plantuml
@startuml 数据流图-1层
skinparam componentStyle rectangle

actor "检测员" as inspector
actor "检测仪器" as device

rectangle "1.0\n设备信息管理" as P1 #LightGreen
rectangle "2.0\n实验数据管理" as P2 #LightGreen
rectangle "3.0\n结果采集处理" as P3 #LightGreen
rectangle "4.0\n报告生成" as P4 #LightGreen
rectangle "5.0\n数据查询统计" as P5 #LightGreen

database "D1: 设备信息表" as D1
database "D2: 实验数据表" as D2
database "D3: 检测结果表" as D3
database "D4: 检测记录表" as D4

inspector --> P1 : 录入设备信息
P1 --> D1 : 保存设备信息
D1 --> P2 : 读取设备信息

inspector --> P2 : 输入实验数据
P2 --> D2 : 保存实验数据

device --> P3 : 屏幕图像
P3 --> P3 : OCR识别
inspector --> P3 : 确认/修正
P3 --> D3 : 保存结果

D1 --> P4 : 设备信息
D2 --> P4 : 实验数据
D3 --> P4 : 检测结果
P4 --> D4 : 生成记录
P4 --> inspector : 检测报告

inspector --> P5 : 查询条件
D4 --> P5 : 检测记录
P5 --> inspector : 统计结果
@enduml
```

## 3.3 行为模型 (状态图)

状态图描述了检测记录从创建到归档的全生命周期状态变化，明确了不同阶段的操作权限。

**图 3-4 检测记录状态转换图**

```plantuml
@startuml 状态图
hide empty description

[*] --> 待录入 : 创建检测
待录入 --> 设备信息已录入 : 保存设备信息
设备信息已录入 --> 实验数据已录入 : 保存实验数据
实验数据已录入 --> 结果采集中 : 开始采集
结果采集中 --> 识别中 : 图像上传成功
识别中 --> 结果待确认 : OCR识别完成
结果待确认 --> 结果采集中 : 重新采集
结果待确认 --> 检测完成 : 确认结果
检测完成 --> 报告已生成 : 生成报告
报告已生成 --> 已打印 : 打印报告
报告已生成 --> 已导出 : 导出数据
报告已生成 --> 已归档 : 归档
已打印 --> 已归档
已导出 --> 已归档
已归档 --> [*]

note right of 结果待确认
  检测员可以修正
  OCR识别的数据
end note

note right of 检测完成
  进入此状态后
  数据不可修改
end note
@enduml
```

## 3.4 模块结构图

模块结构图展示了系统的软件架构分层，明确了表现层、业务逻辑层、数据访问层和数据层的职责。

**图 3-5 系统模块结构图**

```plantuml
@startuml 模块结构图
skinparam packageStyle rectangle

package "互感器二次压降检测仪检定系统" {
    package "表现层 (Presentation Layer)" #LightBlue {
        [App 主应用]
        [DeviceInfoInput\n设备信息录入]
        [ExperimentDataInput\n实验数据录入]
        [ResultCapture\n结果采集]
        [ReportView\n报告查看]
        [DataQuery\n数据查询]
    }
    
    package "业务逻辑层 (Business Logic Layer)" #LightGreen {
        [ReportService\n报告服务]
        [OCRService\nOCR识别服务]
        [ValidationService\n数据验证服务]
        [StatisticsService\n统计分析服务]
    }
    
    package "数据访问层 (Data Access Layer)" #LightYellow {
        [StorageService\n存储服务]
        [DataSerializer\n数据序列化]
    }
    
    package "数据层 (Data Layer)" #LightGray {
        database "LocalStorage\n本地存储" as storage
    }
}

[App 主应用] --> [DeviceInfoInput\n设备信息录入]
[App 主应用] --> [ExperimentDataInput\n实验数据录入]
[App 主应用] --> [ResultCapture\n结果采集]
[App 主应用] --> [ReportView\n报告查看]
[App 主应用] --> [DataQuery\n数据查询]

[DeviceInfoInput\n设备信息录入] --> [ValidationService\n数据验证服务]
[ExperimentDataInput\n实验数据录入] --> [ValidationService\n数据验证服务]
[ResultCapture\n结果采集] --> [OCRService\nOCR识别服务]
[ReportView\n报告查看] --> [ReportService\n报告服务]
[DataQuery\n数据查询] --> [StatisticsService\n统计分析服务]

[ReportService\n报告服务] --> [StorageService\n存储服务]
[OCRService\nOCR识别服务] --> [StorageService\n存储服务]
[StatisticsService\n统计分析服务] --> [StorageService\n存储服务]

[StorageService\n存储服务] --> [DataSerializer\n数据序列化]
[DataSerializer\n数据序列化] --> storage

note right of "表现层 (Presentation Layer)"
  负责用户界面展示
  和用户交互处理
end note

note right of "业务逻辑层 (Business Logic Layer)"
  负责核心业务逻辑
  数据处理和转换
end note

note right of "数据访问层 (Data Access Layer)"
  封装数据存储操作
  提供统一访问接口
end note
@enduml
```

---

# 第4章 面向对象分析与设计

本章采用面向对象方法，通过用例图、活动图、类图和时序图对系统进行详细设计。

## 4.1 用例图

用例图明确了系统的参与者（检测员、审核员）及其可执行的功能操作。

**图 4-1 系统用例图**

```plantuml
@startuml 用例图
left to right direction
skinparam packageStyle rectangle

actor "检测员" as inspector
actor "审核员" as reviewer

rectangle "互感器二次压降检测仪检定系统" {
  usecase "录入设备信息" as UC1
  usecase "输入实验数据" as UC2
  usecase "采集检测结果" as UC3
  usecase "OCR图像识别" as UC4
  usecase "生成检测报告" as UC5
  usecase "打印报告" as UC6
  usecase "导出报告数据" as UC7
  usecase "查询检测记录" as UC8
  usecase "统计分析数据" as UC9
  usecase "删除检测记录" as UC10
}

inspector --> UC1
inspector --> UC2
inspector --> UC3
inspector --> UC6
inspector --> UC7
inspector --> UC8
inspector --> UC9
inspector --> UC10

reviewer --> UC5
reviewer --> UC8

UC3 ..> UC4 : <<include>>
UC5 ..> UC1 : <<include>>
UC5 ..> UC2 : <<include>>
UC5 ..> UC3 : <<include>>

note right of UC4
  自动从检测仪屏幕
  采集图像并识别数据
end note

note right of UC5
  需要完成前置步骤后
  才能生成报告
end note
@enduml
```

## 4.2 活动图

活动图描述了检测员从设备接入到最终完成报告打印的完整业务流程，包括了数据校验和修正的决策逻辑。

**图 4-2 检定业务流程活动图**

```plantuml
@startuml 活动图
|检测员|
start
:接入被检测设备;
:录入设备信息\n(产品编号、制造商等);
:输入实验数据\n(项目、档位、限值等);
:进行检测实验;
:从检测仪屏幕\n采集结果图像;

|系统|
:OCR图像识别;
:提取测量数据;

|检测员|
:确认或修正数据;
if (数据是否正确?) then (否)
  :手动修正数据;
else (是)
endif
:保存检测结果;

|系统|
:生成检测报告;

|检测员|
:预览报告;
if (是否需要打印?) then (是)
  :打印报告;
else (否)
endif

if (是否需要导出?) then (是)
  :导出报告数据;
else (否)
endif

:完成检测;
if (是否继续检测?) then (是)
  :开始新检测;
  detach
else (否)
  stop
endif
@enduml
```

## 4.3 类图

类图展示了系统的静态结构，包括实体类（如 Detection, DeviceInfo）、边界类（如 DeviceInfoInput, ReportView）和控制/服务类（如 StorageService, OCRService）。

**图 4-3 系统类图**

```plantuml
@startuml 类图
skinparam classAttributeIconSize 0

' 主应用
class App {
  - activeTab: TabType
  - currentDetection: Detection
  - detections: Detection[]
  + saveDetection(data: Partial<Detection>): void
  + render(): JSX.Element
}

' 设备信息
class DeviceInfo {
  - productCode: string
  - productName: string
  - manufacturer: string
  - productionDate: string
  - inspectionDate: string
  - origin: string
  - modelNumber: string
  - notes: string
  + validate(): boolean
}

' 实验数据
class ExperimentData {
  - rows: ExperimentRow[]
  + addRow(): void
  + deleteRow(id: string): void
  + updateRow(id: string, field: string, value: string): void
  + validate(): boolean
}

class ExperimentRow {
  - id: string
  - item: string
  - gear: string
  - percentage: string
  - lowerLimit: string
  - upperLimit: string
  - measuredValue: string
}

' 检测结果
class ResultData {
  - secondaryVoltage: string
  - meteringPoint: string
  - testDate: string
  - tanPhi: string
  - temperature: string
  - humidity: string
  - measurements: Measurements
  - rValue: string
  + parseFromOCR(imageData: string): ResultData
}

class Measurements {
  - ao: PhaseData
  - bo: PhaseData
  - co: PhaseData
}

class PhaseData {
  - f: string
  - d: string
  - dU: string
  - Upt: string
  - Uyb: string
}

' 检测记录
class Detection {
  - id: string
  - deviceInfo: DeviceInfo
  - experimentData: ExperimentData
  - resultData: ResultData
  - timestamp: number
  + toJSON(): string
  + fromJSON(json: string): Detection
}

' 组件
class DeviceInfoInput {
  - data: DeviceInfo
  - onSave: Function
  + handleSubmit(): void
  + render(): JSX.Element
}

class ExperimentDataInput {
  - data: ExperimentData
  - onSave: Function
  - onBack: Function
  + handleSubmit(): void
  + render(): JSX.Element
}

class ResultCapture {
  - capturedImage: string
  - resultData: ResultData
  - onCapture: Function
  + simulateCapture(): void
  + render(): JSX.Element
}

class ReportView {
  - detection: Detection
  - onBack: Function
  + handlePrint(): void
  + handleExport(): void
  + render(): JSX.Element
}

class DataQuery {
  - detections: Detection[]
  - searchTerm: string
  - dateFilter: string
  + filterDetections(): Detection[]
  + calculateStats(): Statistics
  + render(): JSX.Element
}

' 服务类
class StorageService {
  + {static} save(key: string, data: any): void
  + {static} load(key: string): any
  + {static} delete(key: string): void
}

class OCRService {
  + {static} recognize(image: string): Promise<ResultData>
  + {static} extractText(image: string): string
}

class ReportService {
  + {static} generate(detection: Detection): Report
  + {static} print(report: Report): void
  + {static} export(report: Report): void
}

' 关系
App "1" *-- "0..*" Detection : manages
Detection "1" *-- "1" DeviceInfo : contains
Detection "1" *-- "1" ExperimentData : contains
Detection "1" *-- "1" ResultData : contains
ExperimentData "1" *-- "1..*" ExperimentRow : contains
ResultData "1" *-- "1" Measurements : contains
Measurements "1" *-- "3" PhaseData : contains

App --> DeviceInfoInput : uses
App --> ExperimentDataInput : uses
App --> ResultCapture : uses
App --> ReportView : uses
App --> DataQuery : uses

DeviceInfoInput ..> DeviceInfo : creates
ExperimentDataInput ..> ExperimentData : creates
ResultCapture ..> ResultData : creates
ResultCapture ..> OCRService : uses
ReportView ..> ReportService : uses
App ..> StorageService : uses

note right of OCRService
  负责图像识别
  和数据提取
end note

note right of StorageService
  使用LocalStorage
  持久化数据
end note
@enduml
```

## 4.4 打印检测报告的时序图

时序图详细描述了“打印检测报告”这一核心功能的动态调用过程，展示了视图层、服务层、领域对象和底层 API 之间的交互。

**图 4-4 打印检测报告时序图**

```plantuml
@startuml 打印检测报告时序图
actor "检测员" as inspector
participant "ReportView" as view
participant "ReportService" as service
participant "Detection" as detection
participant "Browser API" as browser
database "LocalStorage" as storage

inspector -> view : 点击"打印报告"按钮
activate view

view -> view : 验证报告数据完整性
alt 数据不完整
    view -> inspector : 提示错误信息
else 数据完整
    view -> detection : 获取检测数据
    activate detection
    detection -> storage : 读取完整记录
    activate storage
    storage --> detection : 返回数据
    deactivate storage
    detection --> view : 返回Detection对象
    deactivate detection

    view -> service : generate(detection)
    activate service
    service -> service : 格式化设备信息
    service -> service : 格式化实验数据
    service -> service : 格式化检测结果
    service -> service : 生成HTML报告
    service --> view : 返回格式化报告
    deactivate service

    view -> browser : window.print()
    activate browser
    browser -> browser : 渲染打印预览
    browser -> browser : 应用打印样式\n(@media print)
    browser -> inspector : 显示打印对话框
    deactivate browser

    inspector -> browser : 确认打印
    activate browser
    browser -> browser : 发送到打印机
    browser --> inspector : 打印完成
    deactivate browser

    view -> storage : 更新打印状态
    activate storage
    storage --> view : 更新成功
    deactivate storage

    view --> inspector : 显示成功提示
end
deactivate view

note right of service
  ReportService负责：
  1. 数据格式化
  2. 报告模板渲染
  3. 导出功能
end note

note right of browser
  浏览器打印API：
  - window.print()
  - @media print CSS
  - 页面设置
end note
@enduml
```

---

# 第5章 用户界面设计

(注：此处应插入您之前提到的 UI 设计截图或 Figma 导出图，并附上简单的界面说明)

## 5.1 界面设计原则

本系统界面设计遵循“简洁、高效、清晰”的原则，采用蓝白为主色调，符合工业软件的专业感。操作流程采用向导式设计，引导用户分步完成检测。

---

# 第6章 总结

本文设计并实现了一套二次压降检测仪检定软件系统。通过结构化分析和面向对象设计方法，明确了系统的需求和架构。系统实现了从设备信息录入、图像采集识别到报告生成的全流程自动化，解决了传统人工检定效率低、易出错的问题。通过引入 OCR 技术和数字化存储，极大地提升了检定工作的智能化水平。未来工作将集中在进一步提高 OCR 识别在复杂光线下的准确率，以及开发移动端应用以支持远程数据查询。
