#ifndef DLG_ADDDRIVER_H
#define DLG_ADDDRIVER_H

#include <QDialog>
#include"driversql.h"

namespace Ui {
class Dlg_AddDriver;
}

class Dlg_AddDriver : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_AddDriver(QWidget *parent = nullptr);
    ~Dlg_AddDriver();

    void setType(bool isAdd,Driversinfo dinfo={},Carsinfo cinfo={},Parkinginfo pinfo={},DriverCar dcinfo={});

    // 为出场对话框添加的设置车辆信息的函数
    void setCarInfo(int cno, const QString &clicense); // 注意：这个函数应该在dlg_out类中实现

private slots:
    void on_Btn_save_clicked();

    void on_Btn_cancel_clicked();

private:
    // 加载空闲停车位
    void loadFreeParking();

private:
    Ui::Dlg_AddDriver *ui;
    DriverSql *m_ptrDriverSql;
    bool m_isAdd; // 标志是新增还是修改
    Carsinfo m_currentCarInfo; // 保存当前车辆信息，用于修改模式

    // 存储当前编辑的车辆信息 (主要用于修改模式和出场对话框)
    int m_cno;           // 车辆编号
    QString m_clicense;  // 车牌号
};

#endif // DLG_ADDDRIVER_H
