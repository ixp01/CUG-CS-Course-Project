#include "databasemanager.h"
#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    initDatabase();
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initDatabase()
{
    // 使用SQLite数据库
    db = QSqlDatabase::addDatabase("QSQL3");
    
    // 设置数据库文件路径
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dbPath)) {
        dir.mkpath(dbPath);
    }
    
    db.setDatabaseName(dbPath + "/transformer_inspection.db");
    
    if (!db.open()) {
        logError("无法打开数据库: " + db.lastError().text());
        return false;
    }
    
    return createTables();
}

bool DatabaseManager::isOpen() const
{
    return db.isOpen();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(db);
    
    // 创建设备信息表
    QString createDeviceTable = R"(
        CREATE TABLE IF NOT EXISTS devices (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_number TEXT NOT NULL,
            product_name TEXT,
            manufacturer TEXT,
            production_date TEXT,
            inspection_date TEXT,
            origin TEXT,
            product_model TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createDeviceTable)) {
        logError("创建设备表失败: " + query.lastError().text());
        return false;
    }
    
    // 创建实验数据表
    QString createExperimentTable = R"(
        CREATE TABLE IF NOT EXISTS experiments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id INTEGER NOT NULL,
            experiment_date TEXT,
            selected_items TEXT,
            experiment_params TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
        )
    )";
    
    if (!query.exec(createExperimentTable)) {
        logError("创建实验表失败: " + query.lastError().text());
        return false;
    }
    
    // 创建检测结果表
    QString createResultTable = R"(
        CREATE TABLE IF NOT EXISTS test_results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            experiment_id INTEGER NOT NULL,
            project_name TEXT,
            gear TEXT,
            percentage TEXT,
            lower_limit REAL,
            upper_limit REAL,
            measured_value REAL,
            is_qualified INTEGER,
            detailed_result TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (experiment_id) REFERENCES experiments(id) ON DELETE CASCADE
        )
    )";
    
    if (!query.exec(createResultTable)) {
        logError("创建结果表失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

void DatabaseManager::logError(const QString &message)
{
    qDebug() << message;
    emit databaseError(message);
}

// ==================== 设备信息操作 ====================

int DatabaseManager::insertDevice(const DeviceInfo &device)
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO devices (product_number, product_name, manufacturer, 
                           production_date, inspection_date, origin, product_model)
        VALUES (:number, :name, :manufacturer, :prod_date, :insp_date, :origin, :model)
    )");
    
    query.bindValue(":number", device.productNumber);
    query.bindValue(":name", device.productName);
    query.bindValue(":manufacturer", device.manufacturer);
    query.bindValue(":prod_date", device.productionDate);
    query.bindValue(":insp_date", device.inspectionDate);
    query.bindValue(":origin", device.origin);
    query.bindValue(":model", device.productModel);
    
    if (!query.exec()) {
        logError("插入设备失败: " + query.lastError().text());
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateDevice(const DeviceInfo &device)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE devices SET 
            product_number = :number,
            product_name = :name,
            manufacturer = :manufacturer,
            production_date = :prod_date,
            inspection_date = :insp_date,
            origin = :origin,
            product_model = :model
        WHERE id = :id
    )");
    
    query.bindValue(":id", device.id);
    query.bindValue(":number", device.productNumber);
    query.bindValue(":name", device.productName);
    query.bindValue(":manufacturer", device.manufacturer);
    query.bindValue(":prod_date", device.productionDate);
    query.bindValue(":insp_date", device.inspectionDate);
    query.bindValue(":origin", device.origin);
    query.bindValue(":model", device.productModel);
    
    if (!query.exec()) {
        logError("更新设备失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteDevice(int deviceId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", deviceId);
    
    if (!query.exec()) {
        logError("删除设备失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

DeviceInfo DatabaseManager::getDevice(int deviceId)
{
    DeviceInfo device;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM devices WHERE id = :id");
    query.bindValue(":id", deviceId);
    
    if (query.exec() && query.next()) {
        device.id = query.value("id").toInt();
        device.productNumber = query.value("product_number").toString();
        device.productName = query.value("product_name").toString();
        device.manufacturer = query.value("manufacturer").toString();
        device.productionDate = query.value("production_date").toString();
        device.inspectionDate = query.value("inspection_date").toString();
        device.origin = query.value("origin").toString();
        device.productModel = query.value("product_model").toString();
    }
    
    return device;
}

QList<DeviceInfo> DatabaseManager::getAllDevices()
{
    QList<DeviceInfo> devices;
    QSqlQuery query("SELECT * FROM devices ORDER BY id DESC", db);
    
    while (query.next()) {
        DeviceInfo device;
        device.id = query.value("id").toInt();
        device.productNumber = query.value("product_number").toString();
        device.productName = query.value("product_name").toString();
        device.manufacturer = query.value("manufacturer").toString();
        device.productionDate = query.value("production_date").toString();
        device.inspectionDate = query.value("inspection_date").toString();
        device.origin = query.value("origin").toString();
        device.productModel = query.value("product_model").toString();
        devices.append(device);
    }
    
    return devices;
}

QList<DeviceInfo> DatabaseManager::searchDevices(const QString &keyword)
{
    QList<DeviceInfo> devices;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT * FROM devices 
        WHERE product_number LIKE :keyword 
           OR product_name LIKE :keyword 
           OR manufacturer LIKE :keyword
           OR product_model LIKE :keyword
        ORDER BY id DESC
    )");
    query.bindValue(":keyword", "%" + keyword + "%");
    
    if (query.exec()) {
        while (query.next()) {
            DeviceInfo device;
            device.id = query.value("id").toInt();
            device.productNumber = query.value("product_number").toString();
            device.productName = query.value("product_name").toString();
            device.manufacturer = query.value("manufacturer").toString();
            device.productionDate = query.value("production_date").toString();
            device.inspectionDate = query.value("inspection_date").toString();
            device.origin = query.value("origin").toString();
            device.productModel = query.value("product_model").toString();
            devices.append(device);
        }
    }
    
    return devices;
}

// ==================== 实验数据操作 ====================

int DatabaseManager::insertExperiment(const ExperimentData &experiment)
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO experiments (device_id, experiment_date, selected_items, experiment_params)
        VALUES (:device_id, :exp_date, :items, :params)
    )");
    
    query.bindValue(":device_id", experiment.deviceId);
    query.bindValue(":exp_date", experiment.experimentDate);
    query.bindValue(":items", experiment.selectedItems);
    query.bindValue(":params", experiment.experimentParams);
    
    if (!query.exec()) {
        logError("插入实验失败: " + query.lastError().text());
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateExperiment(const ExperimentData &experiment)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE experiments SET 
            device_id = :device_id,
            experiment_date = :exp_date,
            selected_items = :items,
            experiment_params = :params
        WHERE id = :id
    )");
    
    query.bindValue(":id", experiment.id);
    query.bindValue(":device_id", experiment.deviceId);
    query.bindValue(":exp_date", experiment.experimentDate);
    query.bindValue(":items", experiment.selectedItems);
    query.bindValue(":params", experiment.experimentParams);
    
    if (!query.exec()) {
        logError("更新实验失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteExperiment(int experimentId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM experiments WHERE id = :id");
    query.bindValue(":id", experimentId);
    
    if (!query.exec()) {
        logError("删除实验失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

ExperimentData DatabaseManager::getExperiment(int experimentId)
{
    ExperimentData experiment;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM experiments WHERE id = :id");
    query.bindValue(":id", experimentId);
    
    if (query.exec() && query.next()) {
        experiment.id = query.value("id").toInt();
        experiment.deviceId = query.value("device_id").toInt();
        experiment.experimentDate = query.value("experiment_date").toString();
        experiment.selectedItems = query.value("selected_items").toString();
        experiment.experimentParams = query.value("experiment_params").toString();
    }
    
    return experiment;
}

QList<ExperimentData> DatabaseManager::getExperimentsByDevice(int deviceId)
{
    QList<ExperimentData> experiments;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM experiments WHERE device_id = :device_id ORDER BY id DESC");
    query.bindValue(":device_id", deviceId);
    
    if (query.exec()) {
        while (query.next()) {
            ExperimentData experiment;
            experiment.id = query.value("id").toInt();
            experiment.deviceId = query.value("device_id").toInt();
            experiment.experimentDate = query.value("experiment_date").toString();
            experiment.selectedItems = query.value("selected_items").toString();
            experiment.experimentParams = query.value("experiment_params").toString();
            experiments.append(experiment);
        }
    }
    
    return experiments;
}

QList<ExperimentData> DatabaseManager::getAllExperiments()
{
    QList<ExperimentData> experiments;
    QSqlQuery query("SELECT * FROM experiments ORDER BY id DESC", db);
    
    while (query.next()) {
        ExperimentData experiment;
        experiment.id = query.value("id").toInt();
        experiment.deviceId = query.value("device_id").toInt();
        experiment.experimentDate = query.value("experiment_date").toString();
        experiment.selectedItems = query.value("selected_items").toString();
        experiment.experimentParams = query.value("experiment_params").toString();
        experiments.append(experiment);
    }
    
    return experiments;
}

// ==================== 检测结果操作 ====================

int DatabaseManager::insertTestResult(const TestResult &result)
{
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO test_results (experiment_id, project_name, gear, percentage,
                                 lower_limit, upper_limit, measured_value, 
                                 is_qualified, detailed_result)
        VALUES (:exp_id, :project, :gear, :percentage, :lower, :upper, 
                :measured, :qualified, :detailed)
    )");
    
    query.bindValue(":exp_id", result.experimentId);
    query.bindValue(":project", result.projectName);
    query.bindValue(":gear", result.gear);
    query.bindValue(":percentage", result.percentage);
    query.bindValue(":lower", result.lowerLimit);
    query.bindValue(":upper", result.upperLimit);
    query.bindValue(":measured", result.measuredValue);
    query.bindValue(":qualified", result.isQualified ? 1 : 0);
    query.bindValue(":detailed", result.detailedResult);
    
    if (!query.exec()) {
        logError("插入结果失败: " + query.lastError().text());
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateTestResult(const TestResult &result)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE test_results SET 
            experiment_id = :exp_id,
            project_name = :project,
            gear = :gear,
            percentage = :percentage,
            lower_limit = :lower,
            upper_limit = :upper,
            measured_value = :measured,
            is_qualified = :qualified,
            detailed_result = :detailed
        WHERE id = :id
    )");
    
    query.bindValue(":id", result.id);
    query.bindValue(":exp_id", result.experimentId);
    query.bindValue(":project", result.projectName);
    query.bindValue(":gear", result.gear);
    query.bindValue(":percentage", result.percentage);
    query.bindValue(":lower", result.lowerLimit);
    query.bindValue(":upper", result.upperLimit);
    query.bindValue(":measured", result.measuredValue);
    query.bindValue(":qualified", result.isQualified ? 1 : 0);
    query.bindValue(":detailed", result.detailedResult);
    
    if (!query.exec()) {
        logError("更新结果失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteTestResult(int resultId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM test_results WHERE id = :id");
    query.bindValue(":id", resultId);
    
    if (!query.exec()) {
        logError("删除结果失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

TestResult DatabaseManager::getTestResult(int resultId)
{
    TestResult result;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM test_results WHERE id = :id");
    query.bindValue(":id", resultId);
    
    if (query.exec() && query.next()) {
        result.id = query.value("id").toInt();
        result.experimentId = query.value("experiment_id").toInt();
        result.projectName = query.value("project_name").toString();
        result.gear = query.value("gear").toString();
        result.percentage = query.value("percentage").toString();
        result.lowerLimit = query.value("lower_limit").toDouble();
        result.upperLimit = query.value("upper_limit").toDouble();
        result.measuredValue = query.value("measured_value").toDouble();
        result.isQualified = query.value("is_qualified").toInt() == 1;
        result.detailedResult = query.value("detailed_result").toString();
    }
    
    return result;
}

QList<TestResult> DatabaseManager::getResultsByExperiment(int experimentId)
{
    QList<TestResult> results;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM test_results WHERE experiment_id = :exp_id");
    query.bindValue(":exp_id", experimentId);
    
    if (query.exec()) {
        while (query.next()) {
            TestResult result;
            result.id = query.value("id").toInt();
            result.experimentId = query.value("experiment_id").toInt();
            result.projectName = query.value("project_name").toString();
            result.gear = query.value("gear").toString();
            result.percentage = query.value("percentage").toString();
            result.lowerLimit = query.value("lower_limit").toDouble();
            result.upperLimit = query.value("upper_limit").toDouble();
            result.measuredValue = query.value("measured_value").toDouble();
            result.isQualified = query.value("is_qualified").toInt() == 1;
            result.detailedResult = query.value("detailed_result").toString();
            results.append(result);
        }
    }
    
    return results;
}

QList<TestResult> DatabaseManager::getAllTestResults()
{
    QList<TestResult> results;
    QSqlQuery query("SELECT * FROM test_results ORDER BY id DESC", db);
    
    while (query.next()) {
        TestResult result;
        result.id = query.value("id").toInt();
        result.experimentId = query.value("experiment_id").toInt();
        result.projectName = query.value("project_name").toString();
        result.gear = query.value("gear").toString();
        result.percentage = query.value("percentage").toString();
        result.lowerLimit = query.value("lower_limit").toDouble();
        result.upperLimit = query.value("upper_limit").toDouble();
        result.measuredValue = query.value("measured_value").toDouble();
        result.isQualified = query.value("is_qualified").toInt() == 1;
        result.detailedResult = query.value("detailed_result").toString();
        results.append(result);
    }
    
    return results;
}

// ==================== 统计查询 ====================

int DatabaseManager::getTotalDeviceCount()
{
    QSqlQuery query("SELECT COUNT(*) FROM devices", db);
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::getTotalExperimentCount()
{
    QSqlQuery query("SELECT COUNT(*) FROM experiments", db);
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QList<ExperimentData> DatabaseManager::getExperimentsByDateRange(const QString &startDate, const QString &endDate)
{
    QList<ExperimentData> experiments;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT * FROM experiments 
        WHERE experiment_date BETWEEN :start AND :end
        ORDER BY experiment_date DESC
    )");
    query.bindValue(":start", startDate);
    query.bindValue(":end", endDate);
    
    if (query.exec()) {
        while (query.next()) {
            ExperimentData experiment;
            experiment.id = query.value("id").toInt();
            experiment.deviceId = query.value("device_id").toInt();
            experiment.experimentDate = query.value("experiment_date").toString();
            experiment.selectedItems = query.value("selected_items").toString();
            experiment.experimentParams = query.value("experiment_params").toString();
            experiments.append(experiment);
        }
    }
    
    return experiments;
}
