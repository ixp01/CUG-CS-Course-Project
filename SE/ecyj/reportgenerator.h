#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include <QPrinter>
#include <QTextDocument>

class ReportGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ReportGenerator(QObject *parent = nullptr);
    
    // 生成检测报告
    bool generateTestReport(int testRecordId, const QString &outputPath);
    
    // 打印检测报告
    bool printTestReport(int testRecordId, QPrinter *printer);
    
private:
    // 生成HTML报告内容
    QString generateReportHTML(int testRecordId);

private:
    void drawReportHeader(QPainter &painter, const QRect &rect, const QString &title);
    void drawEquipmentInfo(QPainter &painter, QRect &currentRect, int testRecordId);
    void drawTestItems(QPainter &painter, QRect &currentRect, int testRecordId);
    void drawDetailData(QPainter &painter, QRect &currentRect, int testRecordId);
    void drawConclusion(QPainter &painter, QRect &currentRect, int testRecordId);
};

#endif // REPORTGENERATOR_H
