//实现数据库功能类
#ifndef DRIVERSQL_H
#define DRIVERSQL_H
#include <QObject>
#include<QString>
#include <QSqlDatabase>
//车信息
struct Carsinfo
{
    qint32 m_cno;        // 车辆编号
    QString m_clicense;  // 车牌号
    qint32 m_pno;        // 停车位编号
    QString m_username;  // 管理员用户名
    QString m_cin;       // 入场时间
    int m_cinHour;
    int m_cinMin;
    QString m_cout;      // 出场时间
    int m_coutHour;      // 出场时间小时
    int m_coutMin;       // 出场时间分钟
    qint8 m_ctype;       // 车辆类型（1:月卡车, 2:临时车）
};

//车主信息
struct Driversinfo
{
    qint32 m_dno;        // 车主编号
    QString m_dname;     // 车主姓名
};

//停车位信息
struct Parkinginfo
{
    qint32 m_pno;        // 停车位编号
    QString m_parea;     // 停车区域
    QString m_pstatus;   // 停车位状态（空闲/占用）
};

//车主和车信息
struct DriverCar
{
    qint32 m_dno;        // 车主编号
    qint32 m_cno;        // 车辆编号
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

    //主界面功能-------------------------------------------------


    //实现管理系统表的填写  从DB到DBMS-----------
    // 查询所有车主信息
    QList<Driversinfo> getAllDriversInfo();

    // 查询所有车辆信息
    QList<Carsinfo> getAllCarsInfo();

    // 查询所有停车位信息
    QList<Parkinginfo> getAllParkingInfo();

    // 查询所有车主-车辆关联信息
    QList<DriverCar> getAllDriverCarInfo();


    //实现表中数据进数据库 DBMS到DB--------------
    //添加车主信息进数据库
    bool addDrivers(Driversinfo info);

    //添加车辆信息（入场）
    bool addCars1(Carsinfo info);
    //添加车辆信息（出场）
    bool addCars2(int cno, QString cout, int coutHour, int coutMin);

    //添加车主-车辆关联信息
    bool addDriverCar(DriverCar info);


    //清空功能------------------------------
    //删除车辆信息
    bool delCar(int id);

    //清空所有表
    bool clearCarTable();

    //注册和登录界面功能-----------------------
    //判断用户是否存在
    bool isExist(QString strUser);

    //添加单个用户
    bool AddUser(UserInfo info);

    // //删除单个用户
    // bool delUser(QString username);

    // //更新用户信息
    // bool upUserInfo(UserInfo info);


    //实现DBMS的查询功能，ui界面上的功能-----------
    // 根据车辆编号查询车辆信息
    Carsinfo getCarInfoByCno(int cno);

    // 根据车主编号查询车主信息
    Driversinfo getDriverInfoByDno(int dno);

    // 根据停车位编号查询停车位信息
    Parkinginfo getParkingInfoByPno(int pno);

    // 根据车辆编号查询车主-车辆关联信息
    DriverCar getDriverCarByCno(int cno);


    //控制停车位的一对函数----------------
    // 设置停车位状态为
    bool setParkingFree(int pno);

    //设置停车位状态为占用
    bool setParkingOccupied(int pno);

    //用于修改车辆界面---------------
    // 更新车辆信息
    bool updateCars(Carsinfo info);

    // 更新车主信息
    bool updateDrivers(Driversinfo info);

    // 更新车主-车辆关联信息
    bool updateDriverCar(DriverCar info);

    //bool clearCarTable();

    //注册和登录界面------------------------------------------------

    //判断用户密码是否正确
    bool checkPassword(const QString &username, const QString &password);


    //一些功能
    // 获取保安的电话号码
    QString getSecurityPhone(const QString& username);

    // 获取所有空闲停车位信息
    QList<Parkinginfo> getAllFreeParkingInfo();

signals:
    void signalLoginSuccess();
    void signalRegisterSuccess();
    void signalLoginName(QString name);

private:
    QSqlDatabase m_db;//建立数据库
};

#endif // DRIVERSQL_H
