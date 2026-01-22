#include "driversql.h"
#include<QMessageBox>
#include <QSqlError>
#include<QSqlQuery>
#include<QtDebug>
#include<QCoreApplication>

DriverSql *DriverSql::ptrDriverSql=nullptr;

DriverSql::DriverSql(QObject *parent)
    : QObject{parent}
{
    //初始化
    init();
}
//连接数据库
void DriverSql::init()
{
    if (QSqlDatabase::drivers().isEmpty())
    qDebug()<<"No database drivers found";

    //创建数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");
 #if 0
    //获取当前路径
    auto str=QCoreApplication::applicationDirPath()+"data.db";
    qDebug()<<str;
 #endif
    //设置数据库路径
    m_db.setDatabaseName("D:\\CS class\\database\\experiment_new\\DiDa_Hitch_Ride\\DiDa_Hitch_Ride\\DiDa_Hitch_Ride\\data.db");

    if (!m_db.open())
        qDebug()<<"db not open";
}


//实现表中数据进数据库 DBMS到DB-----------------------------------------------
// 查询所有车主信息
QList<Driversinfo> DriverSql::getAllDriversInfo()
{
    QList<Driversinfo> driverList;
    QSqlQuery sql(m_db);
    sql.exec("SELECT dno, dname FROM Drivers");
    while (sql.next()) {
        Driversinfo info;
        info.m_dno = sql.value("dno").toInt();
        info.m_dname = sql.value("dname").toString();
        driverList.append(info);
    }
    return driverList;
}

// 查询所有车辆信息
QList<Carsinfo> DriverSql::getAllCarsInfo()
{
    QList<Carsinfo> carList;
    QSqlQuery sql(m_db);
    sql.exec("SELECT cno, clicense, pno, username, cin, cinHour, cinMin, cout, coutHour, coutMin, ctype FROM Cars");
    while (sql.next()) {
        Carsinfo info;
        info.m_cno = sql.value("cno").toInt();
        info.m_clicense = sql.value("clicense").toString();
        info.m_pno = sql.value("pno").toInt();
        info.m_username = sql.value("username").toString();
        info.m_cin = sql.value("cin").toString();
        info.m_cinHour = sql.value("cinHour").toInt();
        info.m_cinMin = sql.value("cinMin").toInt();
        info.m_cout = sql.value("cout").toString();
        info.m_coutHour = sql.value("coutHour").toInt();
        info.m_coutMin = sql.value("coutMin").toInt();
        info.m_ctype = sql.value("ctype").toInt();
        carList.append(info);
    }
    return carList;
}

// 查询所有停车位信息
QList<Parkinginfo> DriverSql::getAllParkingInfo()
{
    QList<Parkinginfo> parkingList;
    QSqlQuery sql(m_db);
    sql.exec("SELECT pno, parea, pstatus FROM Parking");
    while (sql.next()) {
        Parkinginfo info;
        info.m_pno = sql.value("pno").toInt();
        info.m_parea = sql.value("parea").toString();
        info.m_pstatus = sql.value("pstatus").toString();
        parkingList.append(info);
    }
    return parkingList;
}

// 查询所有车主-车辆关联信息
QList<DriverCar> DriverSql::getAllDriverCarInfo()
{
    QList<DriverCar> driverCarList;
    QSqlQuery sql(m_db);
    sql.exec("SELECT dno, cno FROM DriverCar");
    while (sql.next()) {
        DriverCar info;
        info.m_dno = sql.value("dno").toInt();
        info.m_cno = sql.value("cno").toInt();
        driverCarList.append(info);
    }
    return driverCarList;
}


//添加车主信息
bool DriverSql::addDrivers(Driversinfo info)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("insert into Drivers values(%1,'%2')")
                         .arg(info.m_dno)
                         .arg(info.m_dname);
    return sql.exec(strSql);
}


//添加车辆信息1
bool DriverSql::addCars1(Carsinfo info)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("insert into Cars (cno, clicense, pno, username, cin, cinHour, cinMin, ctype) values(%1,'%2',%3,'%4','%5',%6,%7,%8)")
                         .arg(info.m_cno)
                         .arg(info.m_clicense)
                         .arg(info.m_pno)
                         .arg(info.m_username)
                         .arg(info.m_cin)
                         .arg(info.m_cinHour)
                         .arg(info.m_cinMin)
                         .arg(info.m_ctype);
    return sql.exec(strSql);
}

//添加车辆信息2
bool DriverSql::addCars2(int cno, QString cout, int coutHour, int coutMin)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("update Cars set cout='%1', coutHour=%2, coutMin=%3 where cno=%4")
                         .arg(cout)
                         .arg(coutHour)
                         .arg(coutMin)
                         .arg(cno);
    return sql.exec(strSql);
}

//添加车主-车辆关联信息
bool DriverSql::addDriverCar(DriverCar info)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("insert into DriverCar values(%1,%2)")
                         .arg(info.m_dno)
                         .arg(info.m_cno);
    return sql.exec(strSql);
}


 //清空功能--------------------------------
//删除数据库内的数据
bool DriverSql::delCar(int cno)
{
    QSqlQuery sql(m_db);
    bool ok = true;
    
    // 开始事务
    m_db.transaction();
    
    try {
        // 1. 删除DriverCar关联记录
        ok &= sql.exec(QString("DELETE FROM DriverCar WHERE cno = %1").arg(cno));
        
        // 2. 删除Cars记录
        ok &= sql.exec(QString("DELETE FROM Cars WHERE cno = %1").arg(cno));
        
        if (ok) {
            m_db.commit();
        } else {
            m_db.rollback();
        }
    } catch (...) {
        m_db.rollback();
        ok = false;
    }
    
    return ok;
}

//清除所有数据
bool DriverSql::clearCarTable()
{
    QSqlQuery sql(m_db);
    bool ok = true;
    
    // 开始事务
    m_db.transaction();
    
    try {
        // 1. 清空所有表
        ok &= sql.exec("DELETE FROM DriverCar");
        ok &= sql.exec("DELETE FROM Cars");
        ok &= sql.exec("DELETE FROM Drivers");
        ok &= sql.exec("DELETE FROM Parking");
        
        // 2. 重置自增序列
        ok &= sql.exec("DELETE FROM sqlite_sequence WHERE name IN ('DriverCar', 'Cars', 'Drivers', 'Parking')");
        
        if (ok) {
            m_db.commit();
        } else {
            m_db.rollback();
        }
    } catch (...) {
        m_db.rollback();
        ok = false;
    }
    
    return ok;
}



//用于登录注册的代码------------------------------
//检查对应账号是否存在
bool DriverSql::isExist(QString strUser)
{
     QSqlQuery sql(m_db);

    sql.exec(QString("select *from username where username='%1'").arg(strUser));
    return sql.next();
}
//向username表内添加数据
bool DriverSql::AddUser(UserInfo info)
{
    QSqlQuery sql(m_db);
    QString strQql = QString("insert into username values ('%1', '%2', '%3');")
                         .arg(info.m_username)
                         .arg(info.m_password)
                         .arg(info.phone);

    return sql.exec(strQql);
}
// //删除对应的用户数据 未被使用到
// bool DriverSql::delUser(QString StrUserName)
// {
//     QSqlQuery sql(m_db);

//     return sql.exec(QString("delete from username where username='%1'").arg(StrUserName));
// }

// //更新用户数据  没有被使用
// bool DriverSql::upUserInfo(UserInfo info)
// {
//     QSqlQuery sql(m_db);
//     QString strQql = QString("update username set  password='%1', phone='%2'  where username='%3'")
//                          .arg(info.m_password)
//                          .arg(info.phone)
//                          .arg(info.m_username);
//     bool ret= sql.exec(strQql);
//     QSqlError e=sql.lastError();
//     if(e.isValid())
//     {
//         qDebug()<<e.text();
//     }
//     return ret;
// }

//检查密码
bool DriverSql::checkPassword(const QString &username, const QString &password)
{
    QSqlQuery sql(m_db);
    // 准备查询语句，获取用户名对应的密码
    QString strSql = QString("SELECT password FROM username WHERE username='%1';").arg(username);
    sql.prepare(strSql);

    if (!sql.exec())
    {
        qDebug() << "Error preparing or executing query:" << sql.lastError().text();
        return false;
    }

    // 遍历查询结果
    while (sql.next())
    {
        // 获取查询到的密码
        QString storedPassword = sql.value(0).toString();

        // 比较输入的密码和数据库中的密码
        if (storedPassword == password)
        {
            return true; // 密码匹配
        }
    }

    return false; // 密码不匹配或用户名不存在
}

bool DriverSql::setParkingOccupied(int pno)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("UPDATE Parking SET pstatus='占用' WHERE pno=%1").arg(pno);
    return sql.exec(strSql);
}

// 设置停车位状态为 free
bool DriverSql::setParkingFree(int pno)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("UPDATE Parking SET pstatus='空闲' WHERE pno=%1").arg(pno);
    return sql.exec(strSql);
}

// 获取所有空闲停车位信息
QList<Parkinginfo> DriverSql::getAllFreeParkingInfo()
{
    QList<Parkinginfo> l;
    QSqlQuery query(QSqlDatabase::database());
    QString sql = QString("SELECT * FROM Parking WHERE Pstatus = '空闲'");
    query.exec(sql);
    while(query.next())
    {
        Parkinginfo info;
        info.m_pno = query.value(0).toInt();
        info.m_parea = query.value(1).toString();
        info.m_pstatus = query.value(2).toString();
        l.append(info);
    }
    return l;
}

// 根据车辆编号查询车辆信息
Carsinfo DriverSql::getCarInfoByCno(int cno)
{
    Carsinfo info;
    QSqlQuery sql(m_db);
    sql.prepare("SELECT cno, clicense, pno, username, cin, cinHour, cinMin, cout, coutHour, coutMin, ctype FROM Cars WHERE cno = :cno");
    sql.bindValue(":cno", cno);
    if (sql.exec() && sql.next()) {
        info.m_cno = sql.value("cno").toInt();
        info.m_clicense = sql.value("clicense").toString();
        info.m_pno = sql.value("pno").toInt();
        info.m_username = sql.value("username").toString();
        info.m_cin = sql.value("cin").toString();
        info.m_cinHour = sql.value("cinHour").toInt();
        info.m_cinMin = sql.value("cinMin").toInt();
        info.m_cout = sql.value("cout").toString();
        info.m_coutHour = sql.value("coutHour").toInt();
        info.m_coutMin = sql.value("coutMin").toInt();
        info.m_ctype = sql.value("ctype").toInt();
    } else {
        // 返回一个默认或无效的结构体
        info.m_cno = -1;
    }
    return info;
}

// 根据车主编号查询车主信息
Driversinfo DriverSql::getDriverInfoByDno(int dno)
{
    Driversinfo info;
    QSqlQuery sql(m_db);
    sql.prepare("SELECT dno, dname FROM Drivers WHERE dno = :dno");
    sql.bindValue(":dno", dno);
    if (sql.exec() && sql.next()) {
        info.m_dno = sql.value("dno").toInt();
        info.m_dname = sql.value("dname").toString();
    } else {
        info.m_dno = -1;
    }
    return info;
}

// 根据停车位编号查询停车位信息
Parkinginfo DriverSql::getParkingInfoByPno(int pno)
{
    Parkinginfo info;
    QSqlQuery sql(m_db);
    sql.prepare("SELECT pno, parea, pstatus FROM Parking WHERE pno = :pno");
    sql.bindValue(":pno", pno);
    if (sql.exec() && sql.next()) {
        info.m_pno = sql.value("pno").toInt();
        info.m_parea = sql.value("parea").toString();
        info.m_pstatus = sql.value("pstatus").toString();
    } else {
        info.m_pno = -1;
    }
    return info;
}

// 根据车辆编号查询车主-车辆关联信息
DriverCar DriverSql::getDriverCarByCno(int cno)
{
    DriverCar info;
    QSqlQuery sql(m_db);
    sql.prepare("SELECT dno, cno FROM DriverCar WHERE cno = :cno");
    sql.bindValue(":cno", cno);
    if (sql.exec() && sql.next()) {
        info.m_dno = sql.value("dno").toInt();
        info.m_cno = sql.value("cno").toInt();
    } else {
        info.m_dno = -1;
        info.m_cno = -1;
    }
    return info;
}

// 更新车辆信息
bool DriverSql::updateCars(Carsinfo info)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("UPDATE Cars SET clicense='%1', pno=%2, username='%3', cin='%4', cinHour=%5, cinMin=%6, cout='%7', coutHour=%8, coutMin=%9, ctype=%10 WHERE cno=%11")
                         .arg(info.m_clicense)
                         .arg(info.m_pno)
                         .arg(info.m_username)
                         .arg(info.m_cin)
                         .arg(info.m_cinHour)
                         .arg(info.m_cinMin)
                         .arg(info.m_cout) // 更新时可能还没有出场时间，需要考虑
                         .arg(info.m_coutHour) // 更新时可能还没有出场时间，需要考虑
                         .arg(info.m_coutMin) // 更新时可能还没有出场时间，需要考虑
                         .arg(info.m_ctype)
                         .arg(info.m_cno);
    return sql.exec(strSql);
}

// 更新车主信息
bool DriverSql::updateDrivers(Driversinfo info)
{
    QSqlQuery sql(m_db);
    QString strSql = QString("UPDATE Drivers SET dname='%1' WHERE dno=%2")
                         .arg(info.m_dname)
                         .arg(info.m_dno);
    return sql.exec(strSql);
}

// 更新车主-车辆关联信息
bool DriverSql::updateDriverCar(DriverCar info)
{
    QSqlQuery sql(m_db);
    // 根据cno更新dno，假设cno是唯一的
    QString strSql = QString("UPDATE DriverCar SET dno=%1 WHERE cno=%2")
                         .arg(info.m_dno)
                         .arg(info.m_cno);
    return sql.exec(strSql);
}

// 获取保安的电话号码
QString DriverSql::getSecurityPhone(const QString& username)
{
    QSqlQuery sql(m_db);
    sql.prepare("SELECT phone FROM username WHERE username = :username");
    sql.bindValue(":username", username);

    if (!sql.exec()) {
        return QString(); // 查询失败返回空字符串
    }

    if (sql.next()) {
        return sql.value(0).toString();
    }
    return QString(); // 没找到返回空字符串
}


