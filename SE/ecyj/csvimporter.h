#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QObject>
#include <QString>

class CSVImporter : public QObject
{
    Q_OBJECT

public:
    explicit CSVImporter(QObject *parent = nullptr);
    
    // 导入厂商信息
    bool importManufacturers(const QString &filePath);
    
    // 导入设备信息
    bool importEquipment(const QString &filePath);
    
    // 导入检测记录
    bool importTestRecords(const QString &filePath);
    
    // 导入检测结果项
    bool importTestResultItems(const QString &filePath);
    
signals:
    void progress(int value, const QString &message);
    void finished(bool success, const QString &message);

private:
    QStringList parseLine(const QString &line);
};

#endif // CSVIMPORTER_H
