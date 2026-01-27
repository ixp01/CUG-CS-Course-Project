#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ecyj.db");
    
    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return 1;
    }
    
    qDebug() << "数据库连接成功！";
    
    // 准备插入语句
    QSqlQuery query(db);
    
    // 检查是否已有数据
    query.exec("SELECT COUNT(*) FROM test_result_details");
    if (query.next()) {
        int count = query.value(0).toInt();
        if (count > 0) {
            qDebug() << "警告: test_result_details 表中已有" << count << "条记录";
            qDebug() << "是否清空旧数据？按任意键继续，Ctrl+C取消...";
            getchar();
            query.exec("DELETE FROM test_result_details");
            qDebug() << "旧数据已清空";
        }
    }
    
    // 插入数据的结构
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
    
    query.prepare("INSERT INTO test_result_details ("
                  "test_record_id, item_name, "
                  "ao_f, ao_d, ao_du, ao_upt, ao_uyb, "
                  "bo_f, bo_d, bo_du, bo_upt, bo_uyb, "
                  "co_f, co_d, co_du, co_upt, co_uyb, "
                  "pt_check_note, r_percentage, measurement_result) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    
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
            qDebug() << "✓ 已插入记录" << data.recordId << "-" << data.itemName;
        } else {
            qDebug() << "✗ 插入失败:" << query.lastError().text();
        }
    }
    
    qDebug() << "\n导入完成！成功导入" << successCount << "/" << dataList.size() << "条记录";
    qDebug() << "现在可以在应用程序中查看详细数据了";
    
    db.close();
    return 0;
}
