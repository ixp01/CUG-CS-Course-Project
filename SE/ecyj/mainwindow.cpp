#include "mainwindow.h"
#include "databasehelper.h"
#include "adddialog.h"
#include "detaildatadialog.h"
#include "csvimporter.h"
#include "statisticsdialog.h"
#include "reportgenerator.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QButtonGroup>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tableView(nullptr)
    , m_currentModel(nullptr)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    
    statusBar()->showMessage("系统就绪");
    
    // 默认显示厂商信息
    onShowManufacturers();
}

MainWindow::~MainWindow()
{
    if (m_currentModel) {
        delete m_currentModel;
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("二次压降检测仪检定信息管理系统");
    resize(1200, 700);
    
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // 标题
    QLabel *titleLabel = new QLabel("二次压降检测仪检定信息管理系统", this);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 按钮组
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    m_btnManufacturers = new QPushButton("厂商信息", this);
    m_btnEquipment = new QPushButton("设备信息", this);
    m_btnTestRecords = new QPushButton("检测记录", this);
    m_btnTestItems = new QPushButton("检测结果项", this);
    
    m_btnManufacturers->setCheckable(true);
    m_btnEquipment->setCheckable(true);
    m_btnTestRecords->setCheckable(true);
    m_btnTestItems->setCheckable(true);
    
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_btnManufacturers);
    buttonGroup->addButton(m_btnEquipment);
    buttonGroup->addButton(m_btnTestRecords);
    buttonGroup->addButton(m_btnTestItems);
    buttonGroup->setExclusive(true);
    
    buttonLayout->addWidget(m_btnManufacturers);
    buttonLayout->addWidget(m_btnEquipment);
    buttonLayout->addWidget(m_btnTestRecords);
    buttonLayout->addWidget(m_btnTestItems);
    buttonLayout->addStretch();
    
    // 添加查看详细数据按钮（仅在检测记录页面时启用）
    m_btnViewDetail = new QPushButton("查看详细数据", this);
    m_btnViewDetail->setObjectName("btnViewDetail");
    m_btnViewDetail->setEnabled(false);
    m_btnViewDetail->setToolTip("请先切换到【检测记录】页面，并选中一条记录");
    buttonLayout->addWidget(m_btnViewDetail);
    connect(m_btnViewDetail, &QPushButton::clicked, this, &MainWindow::onViewDetailData);
    
    mainLayout->addLayout(buttonLayout);
    
    // 表格视图
    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // 支持多选
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    
    mainLayout->addWidget(m_tableView);
    
    setCentralWidget(centralWidget);
    
    // 连接信号
    connect(m_btnManufacturers, &QPushButton::clicked, this, &MainWindow::onShowManufacturers);
    connect(m_btnEquipment, &QPushButton::clicked, this, &MainWindow::onShowEquipment);
    connect(m_btnTestRecords, &QPushButton::clicked, this, &MainWindow::onShowTestRecords);
    connect(m_btnTestItems, &QPushButton::clicked, this, &MainWindow::onShowTestItems);
    
    m_btnManufacturers->setChecked(true);
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("导入CSV", this, &MainWindow::onImportCSV);
    fileMenu->addAction("导出PDF", this, &MainWindow::onExportPDF);
    fileMenu->addAction("打印报告", this, &MainWindow::onPrintReport);
    fileMenu->addSeparator();
    fileMenu->addAction("退出", this, &QWidget::close);
    
    // 数据菜单
    QMenu *dataMenu = menuBar()->addMenu("数据(&D)");
    dataMenu->addAction("添加记录", this, &MainWindow::onAddRecord);
    dataMenu->addAction("删除记录", this, &MainWindow::onDeleteRecord);
    dataMenu->addAction("刷新数据", this, &MainWindow::onRefreshData);
    dataMenu->addSeparator();
    dataMenu->addAction("清空数据库", this, &MainWindow::onClearDatabase);
    
    // 统计菜单
    QMenu *statsMenu = menuBar()->addMenu("统计(&S)");
    statsMenu->addAction("统计分析", this, &MainWindow::onStatistics);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于", this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("工具栏");
    toolBar->setMovable(false);
    
    toolBar->addAction("添加", this, &MainWindow::onAddRecord);
    toolBar->addAction("删除", this, &MainWindow::onDeleteRecord);
    toolBar->addAction("刷新", this, &MainWindow::onRefreshData);
    toolBar->addSeparator();
    toolBar->addAction("导入", this, &MainWindow::onImportCSV);
    toolBar->addAction("导出", this, &MainWindow::onExportPDF);
    toolBar->addSeparator();
    toolBar->addAction("统计", this, &MainWindow::onStatistics);
}

void MainWindow::loadTableData(const QString &tableName)
{
    m_currentTable = tableName;
    
    if (m_currentModel) {
        delete m_currentModel;
        m_currentModel = nullptr;
    }
    
    m_currentModel = new QSqlTableModel(this, DatabaseHelper::instance()->getDatabase());
    m_currentModel->setTable(tableName);
    m_currentModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_currentModel->select();
    
    // 设置中文表头
    if (tableName == "manufacturers") {
        m_currentModel->setHeaderData(0, Qt::Horizontal, "ID");
        m_currentModel->setHeaderData(1, Qt::Horizontal, "厂商名称");
        m_currentModel->setHeaderData(2, Qt::Horizontal, "联系人");
        m_currentModel->setHeaderData(3, Qt::Horizontal, "电话");
        m_currentModel->setHeaderData(4, Qt::Horizontal, "地址");
        m_currentModel->setHeaderData(5, Qt::Horizontal, "创建时间");
    } else if (tableName == "equipment_info") {
        m_currentModel->setHeaderData(0, Qt::Horizontal, "ID");
        m_currentModel->setHeaderData(1, Qt::Horizontal, "产品编号");
        m_currentModel->setHeaderData(2, Qt::Horizontal, "产品名称");
        m_currentModel->setHeaderData(3, Qt::Horizontal, "厂商ID");
        m_currentModel->setHeaderData(4, Qt::Horizontal, "生产日期");
        m_currentModel->setHeaderData(5, Qt::Horizontal, "产地");
        m_currentModel->setHeaderData(6, Qt::Horizontal, "产品型号");
        m_currentModel->setHeaderData(7, Qt::Horizontal, "创建时间");
    } else if (tableName == "test_records") {
        m_currentModel->setHeaderData(0, Qt::Horizontal, "ID");
        m_currentModel->setHeaderData(1, Qt::Horizontal, "设备ID");
        m_currentModel->setHeaderData(2, Qt::Horizontal, "检测日期");
        m_currentModel->setHeaderData(3, Qt::Horizontal, "检测员");
        m_currentModel->setHeaderData(4, Qt::Horizontal, "检测地点");
        m_currentModel->setHeaderData(5, Qt::Horizontal, "二次电压");
        m_currentModel->setHeaderData(6, Qt::Horizontal, "温度");
        m_currentModel->setHeaderData(7, Qt::Horizontal, "湿度");
        m_currentModel->setHeaderData(8, Qt::Horizontal, "计量点编号");
        m_currentModel->setHeaderData(9, Qt::Horizontal, "测试日期编号");
        m_currentModel->setHeaderData(10, Qt::Horizontal, "备注");
        m_currentModel->setHeaderData(11, Qt::Horizontal, "创建时间");
    } else if (tableName == "test_result_items") {
        m_currentModel->setHeaderData(0, Qt::Horizontal, "ID");
        m_currentModel->setHeaderData(1, Qt::Horizontal, "检测记录ID");
        m_currentModel->setHeaderData(2, Qt::Horizontal, "项目(PT1/PT2/CT1/CT2)");
        m_currentModel->setHeaderData(3, Qt::Horizontal, "档位");
        m_currentModel->setHeaderData(4, Qt::Horizontal, "百分比");
        m_currentModel->setHeaderData(5, Qt::Horizontal, "数据下限");
        m_currentModel->setHeaderData(6, Qt::Horizontal, "数据上限");
        m_currentModel->setHeaderData(7, Qt::Horizontal, "实测数据");
        m_currentModel->setHeaderData(8, Qt::Horizontal, "是否合格");
        m_currentModel->setHeaderData(9, Qt::Horizontal, "创建时间");
    } else if (tableName == "test_result_details") {
        m_currentModel->setHeaderData(0, Qt::Horizontal, "ID");
        m_currentModel->setHeaderData(1, Qt::Horizontal, "检测记录ID");
        m_currentModel->setHeaderData(2, Qt::Horizontal, "项目名称");
        m_currentModel->setHeaderData(3, Qt::Horizontal, "ao f(%)");
        m_currentModel->setHeaderData(4, Qt::Horizontal, "ao d(分)");
        m_currentModel->setHeaderData(5, Qt::Horizontal, "ao dU(%)");
        m_currentModel->setHeaderData(6, Qt::Horizontal, "ao Upt:U");
        m_currentModel->setHeaderData(7, Qt::Horizontal, "ao Uyb:U");
        m_currentModel->setHeaderData(8, Qt::Horizontal, "bo f(%)");
        m_currentModel->setHeaderData(9, Qt::Horizontal, "bo d(分)");
        m_currentModel->setHeaderData(10, Qt::Horizontal, "bo dU(%)");
        m_currentModel->setHeaderData(11, Qt::Horizontal, "bo Upt:U");
        m_currentModel->setHeaderData(12, Qt::Horizontal, "bo Uyb:U");
        m_currentModel->setHeaderData(13, Qt::Horizontal, "co f(%)");
        m_currentModel->setHeaderData(14, Qt::Horizontal, "co d(分)");
        m_currentModel->setHeaderData(15, Qt::Horizontal, "co dU(%)");
        m_currentModel->setHeaderData(16, Qt::Horizontal, "co Upt:U");
        m_currentModel->setHeaderData(17, Qt::Horizontal, "co Uyb:U");
        m_currentModel->setHeaderData(18, Qt::Horizontal, "PT侧备注");
        m_currentModel->setHeaderData(19, Qt::Horizontal, "r%");
        m_currentModel->setHeaderData(20, Qt::Horizontal, "测量结束");
        m_currentModel->setHeaderData(21, Qt::Horizontal, "创建时间");
    }
    
    m_tableView->setModel(m_currentModel);
    m_tableView->resizeColumnsToContents();
    
    // 重新连接选择信号（因为 model 改变后 selectionModel 也会改变）
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateDetailButtonState);
    
    // 初始化按钮状态
    updateDetailButtonState();
    
    statusBar()->showMessage(QString("当前表: %1 | 共 %2 条记录")
        .arg(tableName).arg(m_currentModel->rowCount()));
}

// 槽函数实现
void MainWindow::onShowManufacturers()
{
    loadTableData("manufacturers");
}

void MainWindow::onShowEquipment()
{
    loadTableData("equipment_info");
}

void MainWindow::onShowTestRecords()
{
    loadTableData("test_records");
}

void MainWindow::onShowTestItems()
{
    loadTableData("test_result_items");
}

void MainWindow::onViewDetailData()
{
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择一条检测记录");
        return;
    }
    
    int recordId = m_currentModel->data(m_currentModel->index(selection[0].row(), 0)).toInt();
    
    DetailDataDialog dialog(recordId, this);
    dialog.exec();
}

void MainWindow::onAddRecord()
{
    AddDialog::DialogType type;
    
    if (m_currentTable == "manufacturers") {
        type = AddDialog::AddManufacturer;
    } else if (m_currentTable == "equipment_info") {
        type = AddDialog::AddEquipment;
    } else if (m_currentTable == "test_records") {
        type = AddDialog::AddTestRecord;
    } else if (m_currentTable == "test_result_items") {
        type = AddDialog::AddTestItem;
    } else {
        QMessageBox::warning(this, "提示", "请先选择要添加记录的表");
        return;
    }
    
    AddDialog dialog(type, this);
    if (dialog.exec() == QDialog::Accepted) {
        QMap<QString, QVariant> data = dialog.getData();
        
        // 构建插入语句
        QStringList fields, placeholders;
        for (auto it = data.begin(); it != data.end(); ++it) {
            fields << it.key();
            placeholders << "?";
        }
        
        QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
            .arg(m_currentTable, fields.join(", "), placeholders.join(", "));
        
        QSqlQuery query(DatabaseHelper::instance()->getDatabase());
        query.prepare(sql);
        for (const QVariant &value : data) {
            query.addBindValue(value);
        }
        
        if (query.exec()) {
            QMessageBox::information(this, "成功", "记录已添加");
            onRefreshData();
        } else {
            QMessageBox::critical(this, "错误", "添加失败: " + query.lastError().text());
        }
    }
}

void MainWindow::onDeleteRecord()
{
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要删除的记录");
        return;
    }
    
    int ret = QMessageBox::question(this, "确认", "确定要删除选中的记录吗？\n相关的子记录也将被删除。",
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    int id = m_currentModel->data(m_currentModel->index(selection[0].row(), 0)).toInt();
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.prepare(QString("DELETE FROM %1 WHERE id = ?").arg(m_currentTable));
    query.addBindValue(id);
    
    if (query.exec()) {
        QMessageBox::information(this, "成功", "记录已删除");
        onRefreshData();
    } else {
        QMessageBox::critical(this, "错误", "删除失败: " + query.lastError().text());
    }
}

void MainWindow::onRefreshData()
{
    if (!m_currentTable.isEmpty()) {
        loadTableData(m_currentTable);
        statusBar()->showMessage("数据已刷新", 2000);
    }
}

void MainWindow::onImportCSV()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择CSV文件", "", "CSV Files (*.csv)");
    if (filePath.isEmpty()) {
        return;
    }
    
    CSVImporter *importer = new CSVImporter(this);
    connect(importer, &CSVImporter::finished, this, [this](bool success, const QString &message) {
        QMessageBox::information(this, success ? "成功" : "失败", message);
        if (success) {
            onRefreshData();
        }
    });
    
    // 根据当前表选择导入类型
    if (m_currentTable == "manufacturers") {
        importer->importManufacturers(filePath);
    } else if (m_currentTable == "equipment_info") {
        importer->importEquipment(filePath);
    } else if (m_currentTable == "test_records") {
        importer->importTestRecords(filePath);
    } else if (m_currentTable == "test_result_items") {
        importer->importTestResultItems(filePath);
    } else {
        QMessageBox::warning(this, "提示", 
            QString("请先点击对应的标签页按钮！\n\n"
                    "当前页面: %1\n\n"
                    "导入说明：\n"
                    "• 厂商信息 → 点击「厂商信息」按钮\n"
                    "• 设备信息 → 点击「设备信息」按钮\n"
                    "• 检测记录 → 点击「检测记录」按钮\n"
                    "• 检测结果项 → 点击「检测结果项」按钮").arg(m_currentTable.isEmpty() ? "未选择" : m_currentTable));
        delete importer;
    }
}

void MainWindow::onExportPDF()
{
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    if (selection.isEmpty() || m_currentTable != "test_records") {
        QMessageBox::warning(this, "提示", "请先在检测记录页面选择一条或多条记录");
        return;
    }
    
    // 如果只选择了一条记录
    if (selection.size() == 1) {
        int recordId = m_currentModel->data(m_currentModel->index(selection[0].row(), 0)).toInt();
        
        QString filePath = QFileDialog::getSaveFileName(this, "保存PDF", "", "PDF Files (*.pdf)");
        if (filePath.isEmpty()) {
            return;
        }
        
        ReportGenerator generator;
        if (generator.generateTestReport(recordId, filePath)) {
            QMessageBox::information(this, "成功", "PDF报告已生成");
        } else {
            QMessageBox::critical(this, "错误", "生成PDF失败");
        }
    } else {
        // 多条记录，选择目录
        QString dirPath = QFileDialog::getExistingDirectory(this, "选择保存目录");
        if (dirPath.isEmpty()) {
            return;
        }
        
        ReportGenerator generator;
        int successCount = 0;
        
        for (const QModelIndex &index : selection) {
            int recordId = m_currentModel->data(m_currentModel->index(index.row(), 0)).toInt();
            QString fileName = QString("检测报告_%1.pdf").arg(recordId);
            QString filePath = dirPath + "/" + fileName;
            
            if (generator.generateTestReport(recordId, filePath)) {
                successCount++;
            }
        }
        
        QMessageBox::information(this, "完成", 
            QString("成功生成 %1/%2 份PDF报告").arg(successCount).arg(selection.size()));
    }
}

void MainWindow::onPrintReport()
{
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();
    if (selection.isEmpty() || m_currentTable != "test_records") {
        QMessageBox::warning(this, "提示", "请先在检测记录页面选择一条记录");
        return;
    }
    
    int recordId = m_currentModel->data(m_currentModel->index(selection[0].row(), 0)).toInt();
    
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        ReportGenerator generator;
        if (generator.printTestReport(recordId, &printer)) {
            QMessageBox::information(this, "成功", "报告已发送到打印机");
        } else {
            QMessageBox::critical(this, "错误", "打印失败");
        }
    }
}

void MainWindow::onStatistics()
{
    StatisticsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "关于",
        "二次压降检测仪检定信息管理系统 v1.0\n\n"
        "本系统用于管理互感器二次压降检测仪的检定数据。\n"
        "支持数据输入、查询、统计和报告生成功能。");
}

void MainWindow::onClearDatabase()
{
    int ret = QMessageBox::warning(this, "警告", 
        "确定要清空所有数据吗？\n\n此操作将删除：\n" 
        "- 所有厂商信息\n"
        "- 所有设备信息\n"
        "- 所有检测记录\n"
        "- 所有检测结果项\n"
        "- 所有详细测量数据\n\n"
        "此操作不可恢复！",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        DatabaseHelper::instance()->clearAllData();
        onRefreshData();
        QMessageBox::information(this, "完成", "数据库已清空");
    }
}

void MainWindow::updateDetailButtonState()
{
    bool hasSelection = m_tableView->selectionModel()->hasSelection();
    bool isTestRecords = m_currentTable == "test_records";
    m_btnViewDetail->setEnabled(hasSelection && isTestRecords);
    
    // 更新提示信息
    if (!isTestRecords) {
        m_btnViewDetail->setToolTip("请先切换到【检测记录】页面");
    } else if (!hasSelection) {
        m_btnViewDetail->setToolTip("请在检测记录表格中选中一条记录");
    } else {
        m_btnViewDetail->setToolTip("点击查看该检测记录的详细测量数据（ao/bo/co数据）");
    }
}
