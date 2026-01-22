#ifndef DRIVERSQL_H
#define DRIVERSQL_H

#include <QObject>
#include<QString>
#include <QSqlDatabase>
struct DriverInfo
{
    qint16 m_id;
    QString m_name;
    QString m_carColor;
    QString m_licenseNum;
    qint8 m_seatNum;
    QString m_seatPos;
    qint8 m_seatPrice;
    qint8 m_departTimeHour;
    qint8 m_departTimeMin;
    QString m_departPlace;
    QString m_arrivePlace;
    QString m_phone;

};

struct UserInfo
{
    QString m_username;
    QString m_password;
    QString phone;
};

class DriverSql : public QObject
{
    Q_OBJECT
public:
    static DriverSql *ptrDriverSql;
    static DriverSql *getinstance()
    {
        if(nullptr==ptrDriverSql)
        {
            ptrDriverSql=new DriverSql;
        }
        return ptrDriverSql;
    }

    explicit DriverSql(QObject *parent = nullptr);

    void init();

    //查询所有司机信息
    quint32 getDriverCnt();

    //查询第几页司机数据(页数，数量) ,从第0页开始
    QList<DriverInfo>getPageStu(quint32 page,quint32 uiCnt);

    //添加司机信息
    bool addDriver(DriverInfo info);

    //删除司机信息
    bool delDriver(int id);

    //清空司机表
    bool clearDriverTable();

    //修改司机信息
    bool UpdateDriverInfo(DriverInfo info);

    //查询所有顺风车
    QList<DriverInfo>getAllDriver();

    //判断用户是否存在
    bool isExist(QString strUser);

    //添加单个用户
    bool AddUser(UserInfo info);

    //删除单个用户
    bool delUser(QString username);

    //更新用户信息
    bool upUserInfo(UserInfo info);

    //判断用户密码是否正确
    bool checkPassword(const QString &username, const QString &password);

signals:
private:
    QSqlDatabase m_db;
};

#endif // DRIVERSQL_H
