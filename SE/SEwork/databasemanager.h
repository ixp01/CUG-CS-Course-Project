#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>

// 设备信息结构体
struct DeviceInfo {
    int id;
    QString productNumber;      // 产品编号
    QString productName;        // 产品名称
    QString manufacturer;       // 制造商
    QString productionDate;     // 生产日期
    QString inspectionDate;     // 送检日期
    QString origin;             // 产地
    QString productModel;       // 产品型号
    
    DeviceInfo() : id(-1) {}
};

// 实验数据结构体
struct ExperimentData {
    int id;
    int deviceId;               // 关联的设备ID
    QString experimentDate;     // 实验日期
    QString selectedItems;      // 选择的项目 (PT1,PT2,CT1,CT2)
    QString experimentParams;   // 实验参数(JSON格式存储)
    
    ExperimentData() : id(-1), deviceId(-1) {}
};

// 检测结果结构体
struct TestResult {
    int id;
    int experimentId;           // 关联的实验ID
    QString projectName;        // 项目名称 (PT1/PT2/CT1/CT2)
    QString gear;               // 档位
    QString percentage;         // 百分比
    double lowerLimit;          // 数据下限
    double upperLimit;          // 数据上限
    double measuredValue;       // 实测数据
    bool isQualified;           // 是否合格
    QString detailedResult;     // 详细结果(JSON格式)
    
    TestResult() : id(-1), experimentId(-1), lowerLimit(0), upperLimit(0), 
                   measuredValue(0), isQualified(false) {}
};

class DatabaseManager : public QObject
{
    Q_OBJECT
    
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // 数据库初始化
    bool initDatabase();
    bool isOpen() const;
    
    // 设备信息操作
    int insertDevice(const DeviceInfo &device);
    bool updateDevice(const DeviceInfo &device);
    bool deleteDevice(int deviceId);
    DeviceInfo getDevice(int deviceId);
    QList<DeviceInfo> getAllDevices();
    QList<DeviceInfo> searchDevices(const QString &keyword);
    
    // 实验数据操作
    int insertExperiment(const ExperimentData &experiment);
    bool updateExperiment(const ExperimentData &experiment);
    bool deleteExperiment(int experimentId);
    ExperimentData getExperiment(int experimentId);
    QList<ExperimentData> getExperimentsByDevice(int deviceId);
    QList<ExperimentData> getAllExperiments();
    
    // 检测结果操作
    int insertTestResult(const TestResult &result);
    bool updateTestResult(const TestResult &result);
    bool deleteTestResult(int resultId);
    TestResult getTestResult(int resultId);
    QList<TestResult> getResultsByExperiment(int experimentId);
    QList<TestResult> getAllTestResults();
    
    // 统计查询
    int getTotalDeviceCount();
    int getTotalExperimentCount();
    QList<ExperimentData> getExperimentsByDateRange(const QString &startDate, const QString &endDate);
    
signals:
    void databaseError(const QString &error);
    
private:
    QSqlDatabase db;
    
    bool createTables();
    void logError(const QString &message);
};

#endif // DATABASEMANAGER_H
