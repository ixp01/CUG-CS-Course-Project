#include "datamanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

DataManager& DataManager::instance()
{
    static DataManager instance;
    return instance;
}

DataManager::DataManager()
    : nextDonationId(1)
    , nextApplicationId(1)
    , nextFinanceId(1)
{
    // 程序启动时自动加载数据
    loadFromFile();
}

DataManager::~DataManager()
{
    // 程序退出时自动保存数据
    saveToFile();
}

// ==================== 捐助管理 ====================

int DataManager::addDonation(const DonationRecord& record)
{
    DonationRecord newRecord = record;
    newRecord.id = nextDonationId++;
    newRecord.status = "待审核";
    newRecord.submitTime = QDateTime::currentDateTime();
    donations.append(newRecord);
    return newRecord.id;
}

bool DataManager::updateDonationStatus(int id, const QString& status, const QString& certificateNo)
{
    for (int i = 0; i < donations.size(); ++i) {
        if (donations[i].id == id) {
            donations[i].status = status;
            donations[i].approveTime = QDateTime::currentDateTime();
            donations[i].certificateNo = certificateNo;
            
            // 如果审核通过，添加收入记录
            if (status == "已通过") {
                FinanceRecord finance;
                finance.type = "收入";
                finance.amount = donations[i].amount;
                finance.description = QString("捐助收入 - %1").arg(donations[i].donorName);
                finance.relatedId = id;
                addFinanceRecord(finance);
            }
            
            return true;
        }
    }
    return false;
}

QList<DonationRecord> DataManager::getAllDonations() const
{
    return donations;
}

QList<DonationRecord> DataManager::getDonationsByStatus(const QString& status) const
{
    QList<DonationRecord> result;
    for (const auto& record : donations) {
        if (record.status == status) {
            result.append(record);
        }
    }
    return result;
}

DonationRecord DataManager::getDonationById(int id) const
{
    for (const auto& record : donations) {
        if (record.id == id) {
            return record;
        }
    }
    return DonationRecord();
}

// ==================== 用款申请管理 ====================

int DataManager::addApplication(const ApplicationRecord& record)
{
    ApplicationRecord newRecord = record;
    newRecord.id = nextApplicationId++;
    newRecord.status = "待审核";
    newRecord.submitTime = QDateTime::currentDateTime();
    applications.append(newRecord);
    return newRecord.id;
}

bool DataManager::updateApplicationStatus(int id, const QString& status, const QString& comment)
{
    for (int i = 0; i < applications.size(); ++i) {
        if (applications[i].id == id) {
            applications[i].status = status;
            applications[i].approveTime = QDateTime::currentDateTime();
            applications[i].approveComment = comment;
            
            // 如果审核通过，添加支出记录
            if (status == "已通过") {
                FinanceRecord finance;
                finance.type = "支出";
                finance.amount = applications[i].amount;
                finance.description = QString("用款支出 - %1").arg(applications[i].schoolName);
                finance.relatedId = id;
                addFinanceRecord(finance);
            }
            
            return true;
        }
    }
    return false;
}

QList<ApplicationRecord> DataManager::getAllApplications() const
{
    return applications;
}

QList<ApplicationRecord> DataManager::getApplicationsByStatus(const QString& status) const
{
    QList<ApplicationRecord> result;
    for (const auto& record : applications) {
        if (record.status == status) {
            result.append(record);
        }
    }
    return result;
}

ApplicationRecord DataManager::getApplicationById(int id) const
{
    for (const auto& record : applications) {
        if (record.id == id) {
            return record;
        }
    }
    return ApplicationRecord();
}

// ==================== 财务记录 ====================

int DataManager::addFinanceRecord(const FinanceRecord& record)
{
    FinanceRecord newRecord = record;
    newRecord.id = nextFinanceId++;
    newRecord.recordTime = QDateTime::currentDateTime();
    financeRecords.append(newRecord);
    return newRecord.id;
}

QList<FinanceRecord> DataManager::getFinanceRecordsByMonth(int year, int month) const
{
    QList<FinanceRecord> result;
    for (const auto& record : financeRecords) {
        if (record.recordTime.date().year() == year && 
            record.recordTime.date().month() == month) {
            result.append(record);
        }
    }
    return result;
}

QList<FinanceRecord> DataManager::getAllFinanceRecords() const
{
    return financeRecords;
}

// ==================== 统计查询 ====================

double DataManager::getTotalIncome() const
{
    double total = 0.0;
    for (const auto& record : financeRecords) {
        if (record.type == "收入") {
            total += record.amount;
        }
    }
    return total;
}

double DataManager::getTotalExpense() const
{
    double total = 0.0;
    for (const auto& record : financeRecords) {
        if (record.type == "支出") {
            total += record.amount;
        }
    }
    return total;
}

double DataManager::getBalance() const
{
    return getTotalIncome() - getTotalExpense();
}

double DataManager::getMonthlyIncome(int year, int month) const
{
    double total = 0.0;
    for (const auto& record : financeRecords) {
        if (record.type == "收入" &&
            record.recordTime.date().year() == year && 
            record.recordTime.date().month() == month) {
            total += record.amount;
        }
    }
    return total;
}

double DataManager::getMonthlyExpense(int year, int month) const
{
    double total = 0.0;
    for (const auto& record : financeRecords) {
        if (record.type == "支出" &&
            record.recordTime.date().year() == year && 
            record.recordTime.date().month() == month) {
            total += record.amount;
        }
    }
    return total;
}

// ==================== 数据持久化（TXT文件）====================

bool DataManager::saveToFile(const QString& filename)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + filename;
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法保存文件:" << filePath << file.errorString();
        return false;
    }
    
    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    
    // 写入文件头
    out << "# 教育基金会管理系统数据文件\n";
    out << "# 自动生成，请勿手动编辑\n";
    out << "# 格式：字段间用|分隔，时间格式为ISO 8601\n\n";
    
    // 写入捐助记录
    out << "[DONATIONS]\n";
    out << "# id|donorType|donorName|idNumber|contact|amount|description|status|submitTime|approveTime|certificateNo\n";
    for (const auto& record : donations) {
        out << encodeRecord(record) << "\n";
    }
    out << "\n";
    
    // 写入用款申请记录
    out << "[APPLICATIONS]\n";
    out << "# id|schoolName|applicant|contact|amount|purpose|status|submitTime|approveTime|approveComment\n";
    for (const auto& record : applications) {
        out << encodeRecord(record) << "\n";
    }
    out << "\n";
    
    // 写入财务记录
    out << "[FINANCE]\n";
    out << "# id|type|amount|description|recordTime|relatedId\n";
    for (const auto& record : financeRecords) {
        out << encodeRecord(record) << "\n";
    }
    out << "\n";
    
    // 写入ID计数器
    out << "[COUNTERS]\n";
    out << nextDonationId << "|" << nextApplicationId << "|" << nextFinanceId << "\n";
    
    file.close();
    qDebug() << "数据已保存到:" << filePath;
    return true;
}

bool DataManager::loadFromFile(const QString& filename)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + filename;
    QFile file(filePath);
    
    if (!file.exists()) {
        qDebug() << "数据文件不存在，使用空数据:" << filePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法读取文件:" << filePath << file.errorString();
        return false;
    }
    
    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif
    
    clearAllData();  // 清空当前数据
    
    QString currentSection;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // 跳过空行和注释
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        
        // 识别section标记
        if (line.startsWith("[") && line.endsWith("]")) {
            currentSection = line.mid(1, line.length() - 2);
            continue;
        }
        
        // 根据当前section解析数据
        if (currentSection == "DONATIONS") {
            DonationRecord record = decodeDonationRecord(line);
            if (record.id > 0) {
                donations.append(record);
                if (record.id >= nextDonationId) {
                    nextDonationId = record.id + 1;
                }
            }
        } else if (currentSection == "APPLICATIONS") {
            ApplicationRecord record = decodeApplicationRecord(line);
            if (record.id > 0) {
                applications.append(record);
                if (record.id >= nextApplicationId) {
                    nextApplicationId = record.id + 1;
                }
            }
        } else if (currentSection == "FINANCE") {
            FinanceRecord record = decodeFinanceRecord(line);
            if (record.id > 0) {
                financeRecords.append(record);
                if (record.id >= nextFinanceId) {
                    nextFinanceId = record.id + 1;
                }
            }
        } else if (currentSection == "COUNTERS") {
            QStringList parts = line.split("|");
            if (parts.size() >= 3) {
                nextDonationId = parts[0].toInt();
                nextApplicationId = parts[1].toInt();
                nextFinanceId = parts[2].toInt();
            }
        }
    }
    
    file.close();
    qDebug() << "数据已加载:" << filePath;
    qDebug() << "  捐助记录:" << donations.size();
    qDebug() << "  用款记录:" << applications.size();
    qDebug() << "  财务记录:" << financeRecords.size();
    return true;
}

void DataManager::clearAllData()
{
    donations.clear();
    applications.clear();
    financeRecords.clear();
    nextDonationId = 1;
    nextApplicationId = 1;
    nextFinanceId = 1;
}

// ==================== 编码/解码辅助方法 ====================

QString DataManager::encodeRecord(const DonationRecord& record) const
{
    return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
        .arg(record.id)
        .arg(record.donorType)
        .arg(record.donorName)
        .arg(record.idNumber)
        .arg(record.contact)
        .arg(record.amount, 0, 'f', 2)
        .arg(record.description)
        .arg(record.status)
        .arg(record.submitTime.toString(Qt::ISODate))
        .arg(record.approveTime.toString(Qt::ISODate))
        .arg(record.certificateNo);
}

QString DataManager::encodeRecord(const ApplicationRecord& record) const
{
    return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10")
        .arg(record.id)
        .arg(record.schoolName)
        .arg(record.applicant)
        .arg(record.contact)
        .arg(record.amount, 0, 'f', 2)
        .arg(record.purpose)
        .arg(record.status)
        .arg(record.submitTime.toString(Qt::ISODate))
        .arg(record.approveTime.toString(Qt::ISODate))
        .arg(record.approveComment);
}

QString DataManager::encodeRecord(const FinanceRecord& record) const
{
    return QString("%1|%2|%3|%4|%5|%6")
        .arg(record.id)
        .arg(record.type)
        .arg(record.amount, 0, 'f', 2)
        .arg(record.description)
        .arg(record.recordTime.toString(Qt::ISODate))
        .arg(record.relatedId);
}

DonationRecord DataManager::decodeDonationRecord(const QString& line) const
{
    DonationRecord record;
    QStringList parts = line.split("|");
    
    if (parts.size() >= 11) {
        record.id = parts[0].toInt();
        record.donorType = parts[1];
        record.donorName = parts[2];
        record.idNumber = parts[3];
        record.contact = parts[4];
        record.amount = parts[5].toDouble();
        record.description = parts[6];
        record.status = parts[7];
        record.submitTime = QDateTime::fromString(parts[8], Qt::ISODate);
        if (!parts[9].isEmpty()) {
            record.approveTime = QDateTime::fromString(parts[9], Qt::ISODate);
        }
        record.certificateNo = parts[10];
    }
    
    return record;
}

ApplicationRecord DataManager::decodeApplicationRecord(const QString& line) const
{
    ApplicationRecord record;
    QStringList parts = line.split("|");
    
    if (parts.size() >= 10) {
        record.id = parts[0].toInt();
        record.schoolName = parts[1];
        record.applicant = parts[2];
        record.contact = parts[3];
        record.amount = parts[4].toDouble();
        record.purpose = parts[5];
        record.status = parts[6];
        record.submitTime = QDateTime::fromString(parts[7], Qt::ISODate);
        if (!parts[8].isEmpty()) {
            record.approveTime = QDateTime::fromString(parts[8], Qt::ISODate);
        }
        record.approveComment = parts[9];
    }
    
    return record;
}

FinanceRecord DataManager::decodeFinanceRecord(const QString& line) const
{
    FinanceRecord record;
    QStringList parts = line.split("|");
    
    if (parts.size() >= 6) {
        record.id = parts[0].toInt();
        record.type = parts[1];
        record.amount = parts[2].toDouble();
        record.description = parts[3];
        record.recordTime = QDateTime::fromString(parts[4], Qt::ISODate);
        record.relatedId = parts[5].toInt();
    }
    
    return record;
}


