#include "reportgenerator.h"
#include "databasehelper.h"
#include <QTextDocument>
#include <QPdfWriter>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

ReportGenerator::ReportGenerator(QObject *parent)
    : QObject(parent)
{
}

bool ReportGenerator::generateTestReport(int testRecordId, const QString &outputPath)
{
    // 生成 HTML 内容
    QString htmlContent = generateReportHTML(testRecordId);
    
    // 创建 QTextDocument
    QTextDocument document;
    document.setHtml(htmlContent);
    document.setPageSize(QSizeF(210, 297));  // A4 尺寸（mm）
    
    // 配置 PDF 打印机
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    
    // 渲染到 PDF
    document.print(&printer);
    
    return true;
}

bool ReportGenerator::printTestReport(int testRecordId, QPrinter *printer)
{
    // 生成 HTML 内容
    QString htmlContent = generateReportHTML(testRecordId);
    
    // 创建 QTextDocument 并打印
    QTextDocument document;
    document.setHtml(htmlContent);
    document.print(printer);
    
    return true;
}

QString ReportGenerator::generateReportHTML(int testRecordId)
{
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    // 查询基本信息
    query.prepare("SELECT ei.product_code, ei.product_name, ei.product_model, ei.production_date, "
                  "m.name, tr.test_date, tr.tester_name, tr.test_location, "
                  "tr.secondary_voltage, tr.temperature, tr.humidity, tr.metering_point_code, "
                  "tr.test_date_code, tr.remarks "
                  "FROM test_records tr "
                  "JOIN equipment_info ei ON tr.equipment_id = ei.id "
                  "LEFT JOIN manufacturers m ON ei.manufacturer_id = m.id "
                  "WHERE tr.id = ?");
    query.addBindValue(testRecordId);
    
    if (!query.exec()) {
        qDebug() << "查询失败:" << query.lastError().text();
        return QString("<html><body><h1>错误：查询失败</h1><p>%1</p></body></html>").arg(query.lastError().text());
    }
    
    if (!query.next()) {
        qDebug() << "找不到检测记录 ID:" << testRecordId;
        return QString("<html><body><h1>错误：找不到检测记录</h1><p>检测记录 ID: %1</p></body></html>").arg(testRecordId);
    }
    
    QString productCode = query.value(0).toString();
    QString productName = query.value(1).toString();
    QString productModel = query.value(2).toString();
    QString productionDate = query.value(3).toString();
    QString manufacturer = query.value(4).toString();
    QString testDate = query.value(5).toString();
    QString testerName = query.value(6).toString();
    QString testLocation = query.value(7).toString();
    QString secondaryVoltage = query.value(8).toString();
    QString temperature = query.value(9).toString();
    QString humidity = query.value(10).toString();
    QString testPointCode = query.value(11).toString();
    QString testDateCode = query.value(12).toString();
    QString remarks = query.value(13).toString();
    
    // 构建检测结果项表格
    QString testItemsRows;
    query.prepare("SELECT item_name, gear_position, percentage, data_lower_limit, "
                  "data_upper_limit, measured_data, is_qualified "
                  "FROM test_result_items WHERE test_record_id = ? ORDER BY id");
    query.addBindValue(testRecordId);
    
    if (query.exec()) {
        while (query.next()) {
            QString qualified = query.value(6).toBool() ? "合格" : "不合格";
            QString rowStyle = query.value(6).toBool() ? "" : "background-color: #ffe6e6;";
            
            testItemsRows += QString(
                "<tr style='%1'>"
                "<td>%2</td>"
                "<td>%3</td>"
                "<td>%4%</td>"
                "<td>%5</td>"
                "<td>%6</td>"
                "<td>%7</td>"
                "<td><b>%8</b></td>"
                "</tr>"
            ).arg(rowStyle)
             .arg(query.value(0).toString())
             .arg(query.value(1).toString())
             .arg(QString::number(query.value(2).toDouble(), 'f', 1))
             .arg(QString::number(query.value(3).toDouble(), 'f', 2))
             .arg(QString::number(query.value(4).toDouble(), 'f', 2))
             .arg(QString::number(query.value(5).toDouble(), 'f', 2))
             .arg(qualified);
        }
    }
    
    // 构建详细测量数据表格
    QString detailDataRows;
    query.prepare("SELECT ao_f, ao_d, ao_du, ao_upt, ao_uyb, "
                  "bo_f, bo_d, bo_du, bo_upt, bo_uyb, "
                  "co_f, co_d, co_du, co_upt, co_uyb "
                  "FROM test_result_details WHERE test_record_id = ?");
    query.addBindValue(testRecordId);
    
    if (!query.exec()) {
        qDebug() << "查询详细数据失败:" << query.lastError().text();
    }
    
    if (query.next()) {
        // ao 行
        detailDataRows += QString(
            "<tr>"
            "<td><b>ao</b></td>"
            "<td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>"
            "</tr>"
        ).arg(QString::number(query.value(0).toDouble(), 'f', 2))
         .arg(QString::number(query.value(1).toDouble(), 'f', 2))
         .arg(QString::number(query.value(2).toDouble(), 'f', 2))
         .arg(QString::number(query.value(3).toDouble(), 'f', 2))
         .arg(QString::number(query.value(4).toDouble(), 'f', 2));
        
        // bo 行
        detailDataRows += QString(
            "<tr>"
            "<td><b>bo</b></td>"
            "<td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>"
            "</tr>"
        ).arg(QString::number(query.value(5).toDouble(), 'f', 2))
         .arg(QString::number(query.value(6).toDouble(), 'f', 2))
         .arg(QString::number(query.value(7).toDouble(), 'f', 2))
         .arg(QString::number(query.value(8).toDouble(), 'f', 2))
         .arg(QString::number(query.value(9).toDouble(), 'f', 2));
        
        // co 行
        detailDataRows += QString(
            "<tr>"
            "<td><b>co</b></td>"
            "<td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>"
            "</tr>"
        ).arg(QString::number(query.value(10).toDouble(), 'f', 2))
         .arg(QString::number(query.value(11).toDouble(), 'f', 2))
         .arg(QString::number(query.value(12).toDouble(), 'f', 2))
         .arg(QString::number(query.value(13).toDouble(), 'f', 2))
         .arg(QString::number(query.value(14).toDouble(), 'f', 2));
    } else {
        qDebug() << "警告：检测记录 ID" << testRecordId << "没有详细测量数据";
        detailDataRows = "<tr><td colspan='6' style='text-align:center; color:#999;'>暂无详细测量数据</td></tr>";
    }
    
    // 判断总体结论
    query.prepare("SELECT COUNT(*) FROM test_result_items WHERE test_record_id = ? AND is_qualified = 0");
    query.addBindValue(testRecordId);
    bool allQualified = true;
    if (query.exec() && query.next()) {
        allQualified = (query.value(0).toInt() == 0);
    }
    QString conclusion = allQualified ? "合格" : "不合格";
    QString conclusionColor = allQualified ? "#28a745" : "#dc3545";
    
    // 生成完整 HTML
    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: "SimSun", "STSong", serif;
            margin: 10px;
            font-size: 6pt;
        }
        
        .title {
            text-align: center;
            font-size: 14pt;
            font-weight: bold;
            margin: 8px 0 6px 0;
            font-family: "SimHei", "STHeiti", sans-serif;
        }
        
        .subtitle {
            text-align: center;
            font-size: 6pt;
            color: #666;
            margin-bottom: 8px;
        }
        
        /* 基础信息栏 */
        .info-section {
            margin-bottom: 6px;
        }
        
        .info-table {
            width: 100%%;
            border-collapse: collapse;
            margin-bottom: 6px;
        }
        
        .info-table td {
            padding: 1px 4px;
            border: none;
            font-size: 6pt;
            white-space: nowrap;
            line-height: 1.3;
        }
        
        .info-label {
            font-weight: bold;
            width: 80px;
        }
        
        .info-content {
            border-bottom: 1px solid #ddd;
        }
        
        /* 分隔线 */
        .divider {
            border-top: 2px solid #333;
            margin: 20px 0;
        }
        
        /* 章节标题 */
        .section-title {
            font-size: 8pt;
            font-weight: bold;
            margin: 6px 0 4px 0;
            padding-bottom: 2px;
            border-bottom: 1px solid #999;
        }
        
        /* 数据表格 */
        .data-table {
            width: 100%%;
            border-collapse: collapse;
            margin: 4px 0;
        }
        
        .data-table th,
        .data-table td {
            border: 1px solid #333;
            padding: 2px 3px;
            text-align: center;
            font-size: 6pt;
            line-height: 1.2;
        }
        
        .data-table th {
            background-color: #f0f0f0;
            font-weight: bold;
        }
        
        .data-table tbody tr:nth-child(even) {
            background-color: #fafafa;
        }
        
        /* 结论区 */
        .conclusion-section {
            margin-top: 10px;
        }
        
        .conclusion {
            font-size: 7pt;
            font-weight: bold;
            margin: 6px 0;
        }
        
        .conclusion-value {
            color: %1;
            font-size: 8pt;
        }
        
        /* 签字区 */
        .signature-table {
            width: 100%%;
            margin-top: 20px;
            border: none;
        }
        
        .signature-table td {
            padding: 8px 6px;
            border: none;
            vertical-align: bottom;
            font-size: 6pt;
        }
        
        .signature-line {
            display: inline-block;
            width: 80px;
            border-bottom: 1px solid #333;
            margin: 0 5px;
        }
        
        /* 页脚 */
        .footer {
            text-align: center;
            font-size: 6pt;
            color: #999;
            margin-top: 10px;
            padding-top: 5px;
            border-top: 1px solid #ddd;
        }
    </style>
</head>
<body>
    <div class="title">二次压降检测仪检定报告</div>
    <div class="subtitle">日期：%2</div>
    
    <div class="info-section">
        <table class="info-table">
            <tr>
                <td class="info-label">产品编号：</td>
                <td class="info-content">%3</td>
                <td class="info-label">产品名称：</td>
                <td class="info-content">%4</td>
            </tr>
            <tr>
                <td class="info-label">产品型号：</td>
                <td class="info-content">%5</td>
                <td class="info-label">生产日期：</td>
                <td class="info-content">%6</td>
            </tr>
            <tr>
                <td class="info-label">制造商：</td>
                <td class="info-content">%7</td>
                <td class="info-label">检测日期：</td>
                <td class="info-content">%8</td>
            </tr>
            <tr>
                <td class="info-label">检测员：</td>
                <td class="info-content">%9</td>
                <td class="info-label">检测地点：</td>
                <td class="info-content">%10</td>
            </tr>
            <tr>
                <td class="info-label">二次电压：</td>
                <td class="info-content">%11 U</td>
                <td class="info-label">温度/湿度：</td>
                <td class="info-content">%12 °C / %13 %%</td>
            </tr>
            <tr>
                <td class="info-label">计量点编号：</td>
                <td class="info-content">%14</td>
                <td class="info-label">测试日期编号：</td>
                <td class="info-content">%15</td>
            </tr>
        </table>
    </div>
    
    <div class="divider"></div>
    
    <div class="section-title">检测结果项目</div>
    <table class="data-table">
        <thead>
            <tr>
                <th>项目</th>
                <th>档位</th>
                <th>百分比</th>
                <th>数据下限</th>
                <th>数据上限</th>
                <th>实测数据</th>
                <th>是否合格</th>
            </tr>
        </thead>
        <tbody>
            %16
        </tbody>
    </table>
    
    <div class="section-title">详细测量数据</div>
    <table class="data-table">
        <thead>
            <tr>
                <th>档位</th>
                <th>f(%%)</th>
                <th>d(分)</th>
                <th>dU(%%)</th>
                <th>Upt:U</th>
                <th>Uyb:U</th>
            </tr>
        </thead>
        <tbody>
            %17
        </tbody>
    </table>
    
    <div class="conclusion-section">
        <div class="conclusion">
            检测结论：<span class="conclusion-value">%18</span>
        </div>
        
        <table class="signature-table">
            <tr>
                <td>检测员签字：<span class="signature-line"></span></td>
                <td>复核员签字：<span class="signature-line"></span></td>
                <td>日期：<span class="signature-line"></span></td>
            </tr>
        </table>
    </div>
    
    <div class="footer">
        本报告由二次压降检测仪检定信息管理系统自动生成
    </div>
</body>
</html>
)")
    .arg(conclusionColor)           // %1
    .arg(testDate)                  // %2
    .arg(productCode)               // %3
    .arg(productName)               // %4
    .arg(productModel)              // %5
    .arg(productionDate)            // %6
    .arg(manufacturer)              // %7
    .arg(testDate)                  // %8
    .arg(testerName)                // %9
    .arg(testLocation)              // %10
    .arg(secondaryVoltage)          // %11
    .arg(temperature)               // %12
    .arg(humidity)                  // %13
    .arg(testPointCode)             // %14
    .arg(testDateCode)              // %15
    .arg(testItemsRows)             // %16
    .arg(detailDataRows)            // %17
    .arg(conclusion);               // %18
    
    return html;
}
