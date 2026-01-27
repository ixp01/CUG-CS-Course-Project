#include "csvimporter.h"
#include "databasehelper.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CSVImporter::CSVImporter(QObject *parent)
    : QObject(parent)
{
}

QStringList CSVImporter::parseLine(const QString &line)
{
    QStringList result;
    QString field;
    bool inQuotes = false;
    
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result << field.trimmed();
            field.clear();
        } else {
            field += c;
        }
    }
    result << field.trimmed();
    
    return result;
}

bool CSVImporter::importEquipment(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit finished(false, "无法打开文件: " + filePath);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // 跳过表头
    if (!in.atEnd()) {
        in.readLine();
    }
    
    int count = 0;
    int line = 1;
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    while (!in.atEnd()) {
        QString lineText = in.readLine();
        line++;
        
        if (lineText.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = parseLine(lineText);
        
        if (fields.size() < 6) {
            emit progress(0, QString("第%1行: 字段数量不足").arg(line));
            continue;
        }
        
        query.prepare("INSERT INTO equipment_info "
                      "(product_code, product_name, manufacturer_id, production_date, product_location, product_model) "
                      "VALUES (?, ?, ?, ?, ?, ?)");
        
        query.addBindValue(fields[0]);  // 产品编号
        query.addBindValue(fields[1]);  // 产品名称
        query.addBindValue(fields[2].isEmpty() ? QVariant() : fields[2].toInt());  // 厂商ID
        query.addBindValue(fields[3]);  // 生产日期
        query.addBindValue(fields[4]);  // 产地
        query.addBindValue(fields[5]);  // 型号
        
        if (query.exec()) {
            count++;
            emit progress(count, QString("已导入%1条记录").arg(count));
        } else {
            emit progress(0, QString("第%1行: %2").arg(line).arg(query.lastError().text()));
        }
    }
    
    file.close();
    emit finished(true, QString("成功导入%1条设备记录").arg(count));
    return true;
}

bool CSVImporter::importManufacturers(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit finished(false, "无法打开文件: " + filePath);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // 跳过表头
    if (!in.atEnd()) {
        in.readLine();
    }
    
    int count = 0;
    int line = 1;
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    while (!in.atEnd()) {
        QString lineText = in.readLine();
        line++;
        
        if (lineText.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = parseLine(lineText);
        
        if (fields.size() < 4) {
            emit progress(0, QString("第%1行: 字段数量不足").arg(line));
            continue;
        }
        
        query.prepare("INSERT INTO manufacturers "
                      "(name, contact_person, phone, address) "
                      "VALUES (?, ?, ?, ?)");
        
        query.addBindValue(fields[0]);  // 厂商名称
        query.addBindValue(fields[1]);  // 联系人
        query.addBindValue(fields[2]);  // 电话
        query.addBindValue(fields[3]);  // 地址
        
        if (query.exec()) {
            count++;
            emit progress(count, QString("已导入%1条记录").arg(count));
        } else {
            emit progress(0, QString("第%1行: %2").arg(line).arg(query.lastError().text()));
        }
    }
    
    file.close();
    emit finished(true, QString("成功导入%1条厂商记录").arg(count));
    return true;
}

bool CSVImporter::importTestRecords(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit finished(false, "无法打开文件: " + filePath);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // 跳过表头
    if (!in.atEnd()) {
        in.readLine();
    }
    
    int count = 0;
    int line = 1;
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    while (!in.atEnd()) {
        QString lineText = in.readLine();
        line++;
        
        if (lineText.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = parseLine(lineText);
        
        if (fields.size() < 10) {
            emit progress(0, QString("第%1行: 字段数量不足").arg(line));
            continue;
        }
        
        query.prepare("INSERT INTO test_records "
                      "(equipment_id, test_date, tester_name, test_location, "
                      "secondary_voltage, temperature, humidity, metering_point_code, test_date_code, remarks) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        
        query.addBindValue(fields[0].toInt());          // 设备ID
        query.addBindValue(fields[1]);                  // 检测日期
        query.addBindValue(fields[2]);                  // 检测员
        query.addBindValue(fields[3]);                  // 检测地点
        query.addBindValue(fields[4].toDouble());       // 二次电压
        query.addBindValue(fields[5].toDouble());       // 温度
        query.addBindValue(fields[6].toDouble());       // 湿度
        query.addBindValue(fields[7]);                  // 计量点编号
        query.addBindValue(fields[8]);                  // 测试日期编号
        query.addBindValue(fields[9]);                  // 备注
        
        if (query.exec()) {
            count++;
            emit progress(count, QString("已导入%1条记录").arg(count));
        } else {
            emit progress(0, QString("第%1行: %2").arg(line).arg(query.lastError().text()));
        }
    }
    
    file.close();
    emit finished(true, QString("成功导入%1条检测记录").arg(count));
    return true;
}

bool CSVImporter::importTestResultItems(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit finished(false, "无法打开文件: " + filePath);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // 跳过表头
    if (!in.atEnd()) {
        in.readLine();
    }
    
    int count = 0;
    int line = 1;
    
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    while (!in.atEnd()) {
        QString lineText = in.readLine();
        line++;
        
        if (lineText.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = parseLine(lineText);
        
        if (fields.size() < 8) {
            emit progress(0, QString("第%1行: 字段数量不足").arg(line));
            continue;
        }
        
        query.prepare("INSERT INTO test_result_items "
                      "(test_record_id, item_name, gear_position, percentage, "
                      "data_lower_limit, data_upper_limit, measured_data, is_qualified) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        
        query.addBindValue(fields[0].toInt());          // 检测记录ID
        query.addBindValue(fields[1]);                  // 项目名称
        query.addBindValue(fields[2]);                  // 档位
        query.addBindValue(fields[3].toDouble());       // 百分比
        query.addBindValue(fields[4].toDouble());       // 数据下限
        query.addBindValue(fields[5].toDouble());       // 数据上限
        query.addBindValue(fields[6].toDouble());       // 实测数据
        query.addBindValue(fields[7].toInt() == 1);     // 是否合格
        
        if (query.exec()) {
            count++;
            emit progress(count, QString("已导入%1条记录").arg(count));
        } else {
            emit progress(0, QString("第%1行: %2").arg(line).arg(query.lastError().text()));
        }
    }
    
    file.close();
    emit finished(true, QString("成功导入%1条检测结果项").arg(count));
    return true;
}


void MainWindow::onPrintReport()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要打印的检测记录");
        return;
    }

    int recordId = m_currentModel->data(
        m_currentModel->index(selected.first().row(), 0)).toInt();

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);

    if (printDialog.exec() == QDialog::Accepted) {
        ReportGenerator generator;
        generator.printTestReport(recordId, &printer);
        QMessageBox::information(this, "成功", "打印完成");
    }
}








