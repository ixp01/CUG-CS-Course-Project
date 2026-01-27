#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QList>
#include <QString>
#include <QDateTime>

// 捐助记录结构
struct DonationRecord {
    int id;
    QString donorType;      // "企业" 或 "个人"
    QString donorName;
    QString idNumber;       // 身份证号或营业执照号
    QString contact;
    double amount;
    QString description;
    QString status;         // "待审核", "已通过", "已拒绝"
    QDateTime submitTime;
    QDateTime approveTime;
    QString certificateNo;  // 证书编号
};

// 用款申请记录结构
struct ApplicationRecord {
    int id;
    QString schoolName;
    QString applicant;
    QString contact;
    double amount;
    QString purpose;
    QString status;         // "待审核", "已通过", "已拒绝"
    QDateTime submitTime;
    QDateTime approveTime;
    QString approveComment;
};

// 财务记录结构
struct FinanceRecord {
    int id;
    QString type;           // "收入" 或 "支出"
    double amount;
    QString description;
    QDateTime recordTime;
    int relatedId;          // 关联的捐助或申请ID
};

// 数据管理类（单例模式，内存存储）
class DataManager
{
public:
    static DataManager& instance();
    
    // 捐助管理
    int addDonation(const DonationRecord& record);
    bool updateDonationStatus(int id, const QString& status, const QString& certificateNo = "");
    QList<DonationRecord> getAllDonations() const;
    QList<DonationRecord> getDonationsByStatus(const QString& status) const;
    DonationRecord getDonationById(int id) const;
    
    // 用款申请管理
    int addApplication(const ApplicationRecord& record);
    bool updateApplicationStatus(int id, const QString& status, const QString& comment = "");
    QList<ApplicationRecord> getAllApplications() const;
    QList<ApplicationRecord> getApplicationsByStatus(const QString& status) const;
    ApplicationRecord getApplicationById(int id) const;
    
    // 财务记录
    int addFinanceRecord(const FinanceRecord& record);
    QList<FinanceRecord> getFinanceRecordsByMonth(int year, int month) const;
    QList<FinanceRecord> getAllFinanceRecords() const;
    
    // 统计查询
    double getTotalIncome() const;
    double getTotalExpense() const;
    double getBalance() const;
    double getMonthlyIncome(int year, int month) const;
    double getMonthlyExpense(int year, int month) const;
    
    // 数据持久化（TXT文件）
    bool saveToFile(const QString& filename = "data.txt");
    bool loadFromFile(const QString& filename = "data.txt");
    void clearAllData();  // 清空所有数据
    
private:
    DataManager();
    ~DataManager();  // 析构时自动保存
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    
    QList<DonationRecord> donations;
    QList<ApplicationRecord> applications;
    QList<FinanceRecord> financeRecords;
    
    int nextDonationId;
    int nextApplicationId;
    int nextFinanceId;
    
    // 文件操作辅助方法
    QString encodeRecord(const DonationRecord& record) const;
    QString encodeRecord(const ApplicationRecord& record) const;
    QString encodeRecord(const FinanceRecord& record) const;
    DonationRecord decodeDonationRecord(const QString& line) const;
    ApplicationRecord decodeApplicationRecord(const QString& line) const;
    FinanceRecord decodeFinanceRecord(const QString& line) const;
};

#endif // DATAMANAGER_H


