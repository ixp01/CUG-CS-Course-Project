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
    // DriverInfo d;
    // d.m_name="徐可";
    // d.m_carColor="绿色";
    // d.m_licenseNum="鄂A M9Q85";

    // d.m_seatPos="后座";
    // for(int i=0;i<5;i++)
    // {addDriver(d);}

    //getPageStu(2,3);

    // DriverInfo d;
    // d.m_id=6;
    // d.m_name="dd";
    // delDriver(7);
    // UpdateDriverInfo(d);
    // UserInfo info;
    // info.m_username="xiaomin";
    // info.m_password="123456";
    // info.phone="13846794679";

    // AddUser(info);
    // AddUser(info);
    // qDebug()<<isExist("xiaomin");
    // info.m_password="666";
    // upUserInfo(info);
    // delUser("xiaomin");

}

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
    m_db.setDatabaseName("D:\\CS class\\QT\\课设\\QTwork\\DiDa_Hitch_Ride\\data.db");

    if (!m_db.open())
        qDebug()<<"db not open";
}

quint32 DriverSql::getDriverCnt()
{
    QSqlQuery sql(m_db);
    sql.exec("select count(id) from Driver;");
    quint32 uiCnt=0;
    while(sql.next())
    {
        uiCnt=sql.value(0).toUInt();
    }
    return uiCnt;
}

QList<DriverInfo> DriverSql::getPageStu(quint32 page, quint32 uiCnt)
{
    QList<DriverInfo> l;
    QSqlQuery sql(m_db);
    QString strsql=QString ("select * from Driver order by id limit %1 offset %2;").
                     arg(uiCnt).arg(page*uiCnt);
    sql.exec(strsql);
    DriverInfo info;
    while(sql.next())
    {
        info.m_id=sql.value(0).toUInt();
        info.m_name=sql.value(1).toString();
        info.m_carColor=sql.value(2).toString();
        info.m_licenseNum=sql.value(3).toString();
        info.m_seatNum=sql.value(4).toUInt();
        info.m_seatPos=sql.value(5).toString();
        info.m_seatPrice=sql.value(6).toUInt();
        info.m_departTimeHour=sql.value(7).toUInt();
        info.m_departTimeMin=sql.value(8).toUInt();
        info.m_departPlace=sql.value(9).toString();
        info.m_arrivePlace=sql.value(10).toString();
        info.m_phone=sql.value(11).toString();
        l.push_back(info);
    }
    return l;
}

bool DriverSql::addDriver(DriverInfo info)
{
    QSqlQuery sql(m_db);
    //范例：(NULL,'张三','红色','鄂A M9Q85','2','后排','6','7','30','南望山','未来城','13722657777')
    QString strQql = QString("insert into Driver values(null,'%1','%2','%3',%4,'%5',%6,%7,%8,'%9','%10','%11')")
                         .arg(info.m_name)
                         .arg(info.m_carColor)
                         .arg(info.m_licenseNum)
                         .arg(info.m_seatNum)
                         .arg(info.m_seatPos)
                         .arg(info.m_seatPrice)
                         .arg(info.m_departTimeHour)
                         .arg(info.m_departTimeMin)
                         .arg(info.m_departPlace)
                         .arg(info.m_arrivePlace)
                         .arg(info.m_phone);

    return sql.exec(strQql);
}

bool DriverSql::delDriver(int id)
{
    QSqlQuery sql(m_db);
    return sql.exec(QString ("delete from Driver where id =%1").arg(id));
}

bool DriverSql::clearDriverTable()
{
    QSqlQuery sql(m_db);
    sql.exec("delete from Driver");
    return sql.exec("delete from sqlite_sequence where name='Driver'");
}

bool DriverSql::UpdateDriverInfo(DriverInfo info)
{
    QSqlQuery sql(m_db);
    //范例：(NULL,'张三','红色','鄂A M9Q85','2','后排','6','7','30','南望山','未来城','13722657777')
    QString strSql = QString("UPDATE Driver SET "
                             "name = '%1', "
                             "carColor = '%2', "
                             "licenseNum = '%3', "
                             "seatNum = %4, " // 去掉了引号
                             "seatPos = '%5', " // 假设seatPos是字符串
                             "seatPrice = %6, " // 去掉了引号
                             "departTimeHour = %7, " // 假设是数值类型
                             "departTimeMin = %8, " // 假设是数值类型
                             "departPlace = '%9', " // 假设是字符串
                             "arrivePlace = '%10', " // 假设是字符串
                             "phone = '%11' " // 假设是字符串
                             "WHERE id = %12")
                         .arg(info.m_name)
                         .arg(info.m_carColor)
                         .arg(info.m_licenseNum)
                         .arg(info.m_seatNum) // 数值类型，去掉引号
                         .arg(info.m_seatPos) // 字符串类型，保留引号
                         .arg(info.m_seatPrice) // 数值类型，去掉引号
                         .arg(info.m_departTimeHour) // 数值类型
                         .arg(info.m_departTimeMin) // 数值类型
                         .arg(info.m_departPlace) // 字符串类型，保留引号
                         .arg(info.m_arrivePlace) // 字符串类型，保留引号
                         .arg(info.m_phone) // 字符串类型，保留引号
                         .arg(info.m_id); // WHERE条件
    bool ret = sql.exec(strSql);
    QSqlError e=sql.lastError();
    if(e.isValid())
    {
        qDebug()<<e.text();
    }
    return ret;
}

bool DriverSql::isExist(QString strUser)
{
     QSqlQuery sql(m_db);

    sql.exec(QString("select *from username where username='%1'").arg(strUser));
    return sql.next();
}

bool DriverSql::AddUser(UserInfo info)
{
    QSqlQuery sql(m_db);
    QString strQql = QString("insert into username values ('%1', '%2', '%3');")
                         .arg(info.m_username)
                         .arg(info.m_password)
                         .arg(info.phone);

    return sql.exec(strQql);
}

bool DriverSql::delUser(QString StrUserName)
{
    QSqlQuery sql(m_db);

    return sql.exec(QString("delete from username where username='%1'").arg(StrUserName));
}

bool DriverSql::upUserInfo(UserInfo info)
{
    QSqlQuery sql(m_db);
    //范例：(NULL,'张三','红色','鄂A M9Q85','2','后排','6','7','30','南望山','未来城','13722657777')
    QString strQql = QString("update username set  password='%1', phone='%2'  where username='%3'")
                         .arg(info.m_password)
                         .arg(info.phone)
                         .arg(info.m_username);
    bool ret= sql.exec(strQql);
    QSqlError e=sql.lastError();
    if(e.isValid())
    {
        qDebug()<<e.text();
    }
    return ret;
}

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
