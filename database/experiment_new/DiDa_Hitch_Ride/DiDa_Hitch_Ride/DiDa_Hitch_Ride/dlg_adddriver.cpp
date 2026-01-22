#include "dlg_adddriver.h"
#include "ui_dlg_adddriver.h"
#include<QMessageBox>
Dlg_AddDriver::Dlg_AddDriver(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dlg_AddDriver)
{
    ui->setupUi(this);

    m_ptrDriverSql = DriverSql::getinstance(); // 获取DriverSql实例

    // 加载空闲停车位
    loadFreeParking();
}

Dlg_AddDriver::~Dlg_AddDriver()
{
    delete ui;
}
//设置对话框样式
void Dlg_AddDriver::setType(bool isAdd, Driversinfo dinfo, Carsinfo cinfo, Parkinginfo pinfo, DriverCar dcinfo)
{
    m_isAdd = isAdd;
    m_currentCarInfo = cinfo; // 保存当前车辆信息，用于修改模式下的停车位比较

    // 清空并重新加载空闲停车位
    ui->comboBox_pno->clear();
    loadFreeParking(); // 加载所有空闲的停车位

    if (isAdd) {
        // 新增模式：清空所有输入框
        ui->line_dno->clear();
        ui->line_name->clear();
        ui->line_cno->clear();
        ui->line_clicense->clear();
        ui->line_cinHour->clear(); // 使用小时和分钟控件
        ui->line_cinMin->clear();
        // ui->line_cout不在此UI上，无需清空
        ui->line_username->clear(); // 清空保安号输入框

        // comboBox_pno默认显示第一个空闲停车位 (loadFreeParking已处理)
        // comboBox_cartype默认值或清空（根据需要）
        ui->comboBox_cartype->setCurrentIndex(-1);

        // 新增模式下，车辆编号可编辑（如果需要手动输入cno）
        ui->line_cno->setEnabled(true);

    } else {
        // 修改模式：填充现有数据到UI控件
        ui->line_dno->setText(QString::number(dinfo.m_dno));
        ui->line_name->setText(dinfo.m_dname);
        ui->line_cno->setText(QString::number(cinfo.m_cno));
        ui->line_clicense->setText(cinfo.m_clicense);

        // 填充入场时间小时和分钟
        ui->line_cinHour->setText(QString::number(cinfo.m_cinHour));
        ui->line_cinMin->setText(QString::number(cinfo.m_cinMin));

        // 出场时间没有对应UI控件，不在此设置

        // 设置车辆类型 (月卡:1->索引0, 临时:2->索引1)
        if (cinfo.m_ctype == 1) {
            ui->comboBox_cartype->setCurrentIndex(0);
        } else if (cinfo.m_ctype == 2) {
            ui->comboBox_cartype->setCurrentIndex(1);
        } else {
            ui->comboBox_cartype->setCurrentIndex(-1); // 未知类型
        }

        // 设置停车位
        // 确保当前车辆的停车位显示在comboBox中并被选中，即使它已经被占用了
        if (pinfo.m_pno != -1) {
            bool found = false;
            for (int i = 0; i < ui->comboBox_pno->count(); ++i) {
                if (ui->comboBox_pno->itemData(i).toInt() == pinfo.m_pno) {
                    ui->comboBox_pno->setCurrentIndex(i);
                    found = true;
                    break;
                }
            }
            // 如果当前车辆的停车位不在当前加载的空闲列表中，手动添加并选中
            if (!found) {
                ui->comboBox_pno->addItem(QString::number(pinfo.m_pno), pinfo.m_pno);
                ui->comboBox_pno->setCurrentText(QString::number(pinfo.m_pno));
            }
        } else {
             // 如果当前车辆没有停车位，comboBox_pno显示空或默认
            ui->comboBox_pno->setCurrentIndex(-1);
        }

        // 在修改模式下，车辆编号通常不可编辑
        ui->line_cno->setEnabled(false);

        // 设置保安号
        ui->line_username->setText(cinfo.m_username); // 设置保安号
    }
}

void Dlg_AddDriver::loadFreeParking()
{
    // 清空现有的停车位选项
    ui->comboBox_pno->clear();

    // 获取所有空闲停车位信息
    QList<Parkinginfo> freeParkingList = m_ptrDriverSql->getAllFreeParkingInfo();

    // 将空闲停车位添加到 comboBox
    for (const auto& parking : freeParkingList) {
        ui->comboBox_pno->addItem(QString::number(parking.m_pno), parking.m_pno); // 显示pno，存储pno为用户数据
    }
}

void Dlg_AddDriver::on_Btn_save_clicked()
{
    // 获取UI控件中的数据
    Driversinfo dinfo;
    Carsinfo cinfo;
    DriverCar dcinfo;

    // 从UI控件获取数据填充结构体
    dinfo.m_dno = ui->line_dno->text().toInt();
    dinfo.m_dname = ui->line_name->text();

    cinfo.m_cno = ui->line_cno->text().toInt();
    cinfo.m_clicense = ui->line_clicense->text();
    // 从comboBox_pno获取选中的停车位号 (用户数据)
    cinfo.m_pno = ui->comboBox_pno->currentData().toInt();

    // 从小时和分钟控件获取入场时间
    cinfo.m_cinHour = ui->line_cinHour->text().toInt();
    cinfo.m_cinMin = ui->line_cinMin->text().toInt();
    // 组合成字符串格式的入场时间（如果需要的话）
    cinfo.m_cin = QString("%1:%2").arg(cinfo.m_cinHour, 2, 10, QChar('0')).arg(cinfo.m_cinMin, 2, 10, QChar('0'));

    // 出场时间信息 (m_cout, m_coutHour, m_coutMin) 不在此UI上获取，保留结构体默认值或从数据库获取的值
    // cinfo.m_cout = ...;
    // cinfo.m_coutHour = ...;
    // cinfo.m_coutMin = ...;

    // 车辆类型从comboBox_cartype获取 (月卡:0->1, 临时:1->2)
    cinfo.m_ctype = ui->comboBox_cartype->currentIndex() + 1;

    // 获取保安号
    cinfo.m_username = ui->line_username->text(); // 从line_username获取保安号

    // DriverCar信息直接关联dno和cno
    dcinfo.m_dno = dinfo.m_dno;
    dcinfo.m_cno = cinfo.m_cno;

    auto ptr = DriverSql::getinstance();
    bool success = false;

    if (m_isAdd) {
        // 新增模式
        // 检查车主是否已存在，不存在则添加
        if (ptr->getDriverInfoByDno(dinfo.m_dno).m_dno == -1) {
             ptr->addDrivers(dinfo);
        }
        // 添加车辆信息
        success = ptr->addCars1(cinfo);
        if (success) {
             // 添加车主-车辆关联信息
            ptr->addDriverCar(dcinfo);
            // 设置选中的停车位状态为占用
            if (cinfo.m_pno != -1) {
                ptr->setParkingOccupied(cinfo.m_pno);
            }
        }

    } else {
        // 修改模式
        // 更新车主信息
        ptr->updateDrivers(dinfo);

        // 检查停车位是否更改
        int oldPno = m_currentCarInfo.m_pno; // 原来的停车位 (从保存的旧信息中获取)
        int newPno = cinfo.m_pno; // 新选择的停车位 (从UI获取)

        if (oldPno != newPno) {
            // 如果停车位更改了
            // 1. 将原停车位状态设为 FREE (如果原来有停车位)
            if (oldPno != -1) {
                ptr->setParkingFree(oldPno);
            }
            // 2. 将新选择的停车位状态设为 OCCUPIED (如果选择了新的停车位)
            if (newPno != -1) {
                 ptr->setParkingOccupied(newPno);
            }
        }

        // 更新车辆信息 (注意：出场时间等没有对应UI控件，不会被修改)
        success = ptr->updateCars(cinfo);

        // 更新车主-车辆关联信息 (如果需要，根据您的业务逻辑判断是否允许修改关联)
        // ptr->updateDriverCar(dcinfo);
    }

    if (success) {
        QMessageBox::information(this, "成功", m_isAdd ? "添加成功！" : "修改成功！");
        accept(); // 关闭对话框并返回 Accepted
    } else {
        QMessageBox::warning(this, "失败", m_isAdd ? "添加失败！" : "修改失败！");
    }
}

void Dlg_AddDriver::on_Btn_cancel_clicked()
{
    this->hide();
}

