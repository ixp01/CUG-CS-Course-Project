#include "databasehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QVector>

DatabaseHelper* DatabaseHelper::m_instance = nullptr;

DatabaseHelper::DatabaseHelper(QObject *parent)
    : QObject(parent)
{
}

DatabaseHelper::~DatabaseHelper()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseHelper* DatabaseHelper::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseHelper();
    }
    return m_instance;
}

bool DatabaseHelper::initDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("ecyj_database.db");
    
    if (!m_database.open()) {
        qDebug() << "数据库打开失败:" << m_database.lastError().text();
        return false;
    }
    
    createTables();
    return true;
}

QSqlDatabase DatabaseHelper::getDatabase()
{
    return m_database;
}

void DatabaseHelper::createTables()
{
    QSqlQuery query(m_database);
    
    // 1. 厂商表
    query.exec("CREATE TABLE IF NOT EXISTS manufacturers ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "contact_person TEXT, "
               "phone TEXT, "
               "address TEXT, "
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    
    // 2. 设备信息表
    query.exec("CREATE TABLE IF NOT EXISTS equipment_info ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "product_code TEXT NOT NULL, "
               "product_name TEXT NOT NULL, "
               "manufacturer_id INTEGER, "
               "production_date DATE, "
               "product_location TEXT, "
               "product_model TEXT, "
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY (manufacturer_id) REFERENCES manufacturers(id))");
    
    // 3. 检测记录表
    query.exec("CREATE TABLE IF NOT EXISTS test_records ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "equipment_id INTEGER NOT NULL, "
               "test_date DATE NOT NULL, "
               "tester_name TEXT NOT NULL, "
               "test_location TEXT NOT NULL, "
               "secondary_voltage REAL, "
               "temperature REAL, "
               "humidity REAL, "
               "metering_point_code TEXT, "
               "test_date_code TEXT, "
               "remarks TEXT, "
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY (equipment_id) REFERENCES equipment_info(id))");
    
    // 4. 检测结果项目表（PT1, PT2, CT1, CT2）
    query.exec("CREATE TABLE IF NOT EXISTS test_result_items ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "test_record_id INTEGER NOT NULL, "
               "item_name TEXT NOT NULL, "      // PT1, PT2, CT1, CT2
               "gear_position TEXT, "            // 档位如100V, 5A
               "percentage REAL, "               // 百分比如20%, 100%
               "data_lower_limit REAL, "         // 数据下限
               "data_upper_limit REAL, "         // 数据上限
               "measured_data REAL, "            // 实测数据
               "is_qualified BOOLEAN, "          // 是否合格
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY (test_record_id) REFERENCES test_records(id) ON DELETE CASCADE)");
    
    // 5. 检测详细数据表（存储ao, bo, co, f%, d等详细测量数据）
    query.exec("CREATE TABLE IF NOT EXISTS test_result_details ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "test_record_id INTEGER NOT NULL, "
               "item_name TEXT NOT NULL, "       // PT侧 or CT侧
               "ao_f REAL, "                     // ao f(%)
               "ao_d REAL, "                     // ao d(分)
               "ao_du REAL, "                    // ao dU(%)
               "ao_upt REAL, "                   // ao Upt:U
               "ao_uyb REAL, "                   // ao Uyb:U
               "bo_f REAL, "                     // bo f(%)
               "bo_d REAL, "                     // bo d(分)
               "bo_du REAL, "                    // bo dU(%)
               "bo_upt REAL, "                   // bo Upt:U
               "bo_uyb REAL, "                   // bo Uyb:U
               "co_f REAL, "                     // co f(%)
               "co_d REAL, "                     // co d(分)
               "co_du REAL, "                    // co dU(%)
               "co_upt REAL, "                   // co Upt:U
               "co_uyb REAL, "                   // co Uyb:U
               "pt_check_note TEXT, "            // PT侧不带自校/按1测量按2储存
               "r_percentage REAL, "             // r%值
               "measurement_result TEXT, "       // 测量结束值
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY (test_record_id) REFERENCES test_records(id) ON DELETE CASCADE)");
    
    if (query.lastError().isValid()) {
        qDebug() << "创建表失败:" << query.lastError().text();
    } else {
        qDebug() << "数据库表创建成功";
    }
}

void DatabaseHelper::insertTestDetailsData()
{
    QSqlQuery query(m_database);
    
    // 检查是否已有数据
    query.exec("SELECT COUNT(*) FROM test_result_details");
    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "test_result_details 表已有数据，跳过插入";
        return;
    }
    
    qDebug() << "正在插入测试详细数据...";
    
    // 准备插入语句
    query.prepare("INSERT INTO test_result_details ("
                  "test_record_id, item_name, "
                  "ao_f, ao_d, ao_du, ao_upt, ao_uyb, "
                  "bo_f, bo_d, bo_du, bo_upt, bo_uyb, "
                  "co_f, co_d, co_du, co_upt, co_uyb, "
                  "pt_check_note, r_percentage, measurement_result) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    
    // 定义测试数据
    struct DetailData {
        int recordId;
        QString itemName;
        double ao_f, ao_d, ao_du, ao_upt, ao_uyb;
        double bo_f, bo_d, bo_du, bo_upt, bo_uyb;
        double co_f, co_d, co_du, co_upt, co_uyb;
        QString ptNote;
        double rPercent;
        QString result;
    };
    
    QVector<DetailData> dataList = {
        {1, "PT1", 50.02, 0.12, 0.15, 100.05, 99.95, 50.01, 0.11, 0.14, 100.03, 99.97, 49.99, 0.10, 0.13, 100.02, 99.98, "不带自校", 0.08, "合格"},
        {2, "CT1", 49.98, 0.15, 0.18, 100.08, 99.92, 50.03, 0.13, 0.16, 100.06, 99.94, 50.00, 0.12, 0.15, 100.04, 99.96, "按1测量", 0.09, "合格"},
        {3, "PT2", 50.05, 0.14, 0.17, 100.07, 99.93, 49.97, 0.16, 0.19, 100.09, 99.91, 50.01, 0.15, 0.18, 100.08, 99.92, "按2储存", 0.10, "合格"},
        {4, "CT2", 50.00, 0.11, 0.14, 100.05, 99.95, 50.02, 0.10, 0.13, 100.03, 99.97, 49.99, 0.12, 0.15, 100.04, 99.96, "不带自校", 0.07, "合格"},
        {5, "PT1", 49.96, 0.13, 0.16, 100.06, 99.94, 50.04, 0.14, 0.17, 100.07, 99.93, 50.01, 0.11, 0.14, 100.05, 99.95, "按1测量", 0.09, "合格"},
        {6, "PT2", 50.03, 0.12, 0.15, 100.04, 99.96, 49.98, 0.15, 0.18, 100.08, 99.92, 50.00, 0.13, 0.16, 100.06, 99.94, "按2储存", 0.08, "合格"},
        {7, "CT1", 50.01, 0.16, 0.19, 100.09, 99.91, 50.02, 0.11, 0.14, 100.05, 99.95, 49.99, 0.14, 0.17, 100.07, 99.93, "不带自校", 0.11, "合格"},
        {8, "PT1", 49.97, 0.10, 0.13, 100.03, 99.97, 50.05, 0.12, 0.15, 100.05, 99.95, 50.00, 0.15, 0.18, 100.08, 99.92, "按1测量", 0.08, "合格"},
        {9, "CT2", 50.04, 0.13, 0.16, 100.06, 99.94, 49.96, 0.14, 0.17, 100.07, 99.93, 50.02, 0.11, 0.14, 100.04, 99.96, "按2储存", 0.09, "合格"},
        {10, "PT2", 50.00, 0.15, 0.18, 100.08, 99.92, 50.01, 0.10, 0.13, 100.03, 99.97, 49.99, 0.12, 0.15, 100.05, 99.95, "不带自校", 0.10, "合格"},
        {11, "CT1", 50.08, 0.18, 0.22, 100.12, 99.88, 49.92, 0.19, 0.23, 100.13, 99.87, 50.01, 0.16, 0.20, 100.10, 99.90, "按1测量", 0.13, "需复检"},
        {12, "PT1", 50.02, 0.11, 0.14, 100.04, 99.96, 49.99, 0.13, 0.16, 100.06, 99.94, 50.01, 0.12, 0.15, 100.05, 99.95, "按2储存", 0.08, "合格"},
        {13, "CT2", 49.98, 0.14, 0.17, 100.07, 99.93, 50.03, 0.12, 0.15, 100.05, 99.95, 50.00, 0.10, 0.13, 100.03, 99.97, "不带自校", 0.07, "合格"}
    };
    
    int successCount = 0;
    for (const auto &data : dataList) {
        query.addBindValue(data.recordId);
        query.addBindValue(data.itemName);
        query.addBindValue(data.ao_f);
        query.addBindValue(data.ao_d);
        query.addBindValue(data.ao_du);
        query.addBindValue(data.ao_upt);
        query.addBindValue(data.ao_uyb);
        query.addBindValue(data.bo_f);
        query.addBindValue(data.bo_d);
        query.addBindValue(data.bo_du);
        query.addBindValue(data.bo_upt);
        query.addBindValue(data.bo_uyb);
        query.addBindValue(data.co_f);
        query.addBindValue(data.co_d);
        query.addBindValue(data.co_du);
        query.addBindValue(data.co_upt);
        query.addBindValue(data.co_uyb);
        query.addBindValue(data.ptNote);
        query.addBindValue(data.rPercent);
        query.addBindValue(data.result);
        
        if (query.exec()) {
            successCount++;
        } else {
            qDebug() << "插入失败:" << query.lastError().text();
        }
    }
    
    qDebug() << "成功插入" << successCount << "/" << dataList.size() << "条详细数据";
}

void DatabaseHelper::clearAllData()
{
    QSqlQuery query(m_database);
    
    qDebug() << "清理所有数据...";
    
    // 按顺序删除，遵循外键约束
    query.exec("DELETE FROM test_result_details");
    query.exec("DELETE FROM test_result_items");
    query.exec("DELETE FROM test_records");
    query.exec("DELETE FROM equipment_info");
    query.exec("DELETE FROM manufacturers");
    
    // 重置自增ID
    query.exec("DELETE FROM sqlite_sequence WHERE name='test_result_details'");
    query.exec("DELETE FROM sqlite_sequence WHERE name='test_result_items'");
    query.exec("DELETE FROM sqlite_sequence WHERE name='test_records'");
    query.exec("DELETE FROM sqlite_sequence WHERE name='equipment_info'");
    query.exec("DELETE FROM sqlite_sequence WHERE name='manufacturers'");
    
    qDebug() << "数据库已清空";
}
