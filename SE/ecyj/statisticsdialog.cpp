#include "statisticsdialog.h"
#include "databasehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QSqlQuery>
#include <QDate>

StatisticsDialog::StatisticsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("统计分析");
    setMinimumSize(800, 600);
    
    setupUI();
}

void StatisticsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 查询条件
    QHBoxLayout *condLayout = new QHBoxLayout();
    
    m_startDate = new QDateEdit(QDate::currentDate().addMonths(-1), this);
    m_startDate->setCalendarPopup(true);
    m_endDate = new QDateEdit(QDate::currentDate(), this);
    m_endDate->setCalendarPopup(true);
    
    m_statType = new QComboBox(this);
    m_statType->addItems({"按时间段统计", "按厂商统计合格率", "按月统计工作量"});
    
    QPushButton *btnQuery = new QPushButton("查询", this);
    
    condLayout->addWidget(new QLabel("开始日期:"));
    condLayout->addWidget(m_startDate);
    condLayout->addWidget(new QLabel("结束日期:"));
    condLayout->addWidget(m_endDate);
    condLayout->addWidget(new QLabel("统计类型:"));
    condLayout->addWidget(m_statType);
    condLayout->addWidget(btnQuery);
    condLayout->addStretch();
    
    mainLayout->addLayout(condLayout);
    
    // 结果表格
    m_resultTable = new QTableWidget(this);
    m_resultTable->setAlternatingRowColors(true);
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    mainLayout->addWidget(m_resultTable);
    
    // 连接信号
    connect(btnQuery, &QPushButton::clicked, this, [this]() {
        switch (m_statType->currentIndex()) {
        case 0:
            onStatByDate();
            break;
        case 1:
            onStatByManufacturer();
            break;
        case 2:
            onStatByMonth();
            break;
        }
    });
}

void StatisticsDialog::onStatByDate()
{
    m_resultTable->clear();
    m_resultTable->setColumnCount(3);
    m_resultTable->setHorizontalHeaderLabels({"日期", "检测数量", "合格数量"});
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.prepare("SELECT test_date, COUNT(*) as total, "
                  "SUM(CASE WHEN EXISTS("
                  "    SELECT 1 FROM test_result_items "
                  "    WHERE test_record_id = test_records.id AND is_qualified = 0"
                  ") THEN 0 ELSE 1 END) as qualified "
                  "FROM test_records "
                  "WHERE test_date BETWEEN ? AND ? "
                  "GROUP BY test_date "
                  "ORDER BY test_date");
    query.addBindValue(m_startDate->date().toString("yyyy-MM-dd"));
    query.addBindValue(m_endDate->date().toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        m_resultTable->setRowCount(0);
        while (query.next()) {
            int row = m_resultTable->rowCount();
            m_resultTable->insertRow(row);
            m_resultTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            m_resultTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            m_resultTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        }
    }
}

void StatisticsDialog::onStatByManufacturer()
{
    m_resultTable->clear();
    m_resultTable->setColumnCount(4);
    m_resultTable->setHorizontalHeaderLabels({"厂商", "检测数量", "合格数量", "合格率"});
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.prepare("SELECT m.name, COUNT(*) as total, "
                  "SUM(CASE WHEN EXISTS("
                  "    SELECT 1 FROM test_result_items "
                  "    WHERE test_record_id = tr.id AND is_qualified = 0"
                  ") THEN 0 ELSE 1 END) as qualified "
                  "FROM test_records tr "
                  "JOIN equipment_info ei ON tr.equipment_id = ei.id "
                  "JOIN manufacturers m ON ei.manufacturer_id = m.id "
                  "WHERE tr.test_date BETWEEN ? AND ? "
                  "GROUP BY m.id, m.name "
                  "ORDER BY m.name");
    query.addBindValue(m_startDate->date().toString("yyyy-MM-dd"));
    query.addBindValue(m_endDate->date().toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        m_resultTable->setRowCount(0);
        while (query.next()) {
            int row = m_resultTable->rowCount();
            m_resultTable->insertRow(row);
            QString name = query.value(0).toString();
            int total = query.value(1).toInt();
            int qualified = query.value(2).toInt();
            double rate = total > 0 ? (qualified * 100.0 / total) : 0;
            
            m_resultTable->setItem(row, 0, new QTableWidgetItem(name));
            m_resultTable->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
            m_resultTable->setItem(row, 2, new QTableWidgetItem(QString::number(qualified)));
            m_resultTable->setItem(row, 3, new QTableWidgetItem(QString::number(rate, 'f', 2) + "%"));
        }
    }
}

void StatisticsDialog::onStatByMonth()
{
    m_resultTable->clear();
    m_resultTable->setColumnCount(2);
    m_resultTable->setHorizontalHeaderLabels({"月份", "检测数量"});
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.prepare("SELECT strftime('%Y-%m', test_date) as month, COUNT(*) as total "
                  "FROM test_records "
                  "WHERE test_date BETWEEN ? AND ? "
                  "GROUP BY month "
                  "ORDER BY month");
    query.addBindValue(m_startDate->date().toString("yyyy-MM-dd"));
    query.addBindValue(m_endDate->date().toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        m_resultTable->setRowCount(0);
        while (query.next()) {
            int row = m_resultTable->rowCount();
            m_resultTable->insertRow(row);
            m_resultTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            m_resultTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        }
    }
}
