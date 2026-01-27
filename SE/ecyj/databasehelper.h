#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DatabaseHelper : public QObject
{
    Q_OBJECT
public:
    static DatabaseHelper* instance();
    bool initDatabase();
    QSqlDatabase getDatabase();
    void insertTestDetailsData();
    void clearAllData();
    
private:
    explicit DatabaseHelper(QObject *parent = nullptr);
    ~DatabaseHelper();
    
    void createTables();
    
    static DatabaseHelper* m_instance;
    QSqlDatabase m_database;
};

#endif // DATABASEHELPER_H
