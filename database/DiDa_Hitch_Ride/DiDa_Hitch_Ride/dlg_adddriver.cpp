#include "dlg_adddriver.h"
#include "ui_dlg_adddriver.h"
#include<QMessageBox>
Dlg_AddDriver::Dlg_AddDriver(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dlg_AddDriver)
{
    ui->setupUi(this);
}

Dlg_AddDriver::~Dlg_AddDriver()
{
    delete ui;
}
//设置对话框样式
void Dlg_AddDriver::setType(bool isAdd, DriverInfo info)
{
    m_isAdd=isAdd;
    m_info =info;
    ui->line_name->setText(info.m_name);
    ui->comboBox_arrive->addItem(info.m_arrivePlace);
    ui->line_carColor->setText(info.m_carColor);
    ui->comboBox_depart->addItem(info.m_departPlace);
    ui->line_dapartTimeHour->setText(QString::number(info.m_departTimeHour));
    ui->line_dapartTimeMIn->setText(QString::number(info.m_departTimeMin));
    ui->line_licenseNum->setText(info.m_licenseNum);
    ui->line_phone->setText(info.m_phone);
    ui->line_seatNum->setText(QString::number(info.m_seatNum));
    ui->line_seatPos->setText(info.m_seatPos);
    ui->line_seatPrice->setText(QString::number(info.m_seatPrice));
}

void Dlg_AddDriver::on_Btn_save_clicked()
{
    DriverInfo info;
    auto ptr=DriverSql::getinstance();
    info.m_id=m_info.m_id;
    info.m_name=ui->line_name->text();
    info.m_arrivePlace=ui->comboBox_arrive->currentText();
    info.m_carColor=ui->line_carColor->text();
    info.m_departPlace=ui->comboBox_depart->currentText();
    info.m_departTimeHour=ui->line_dapartTimeHour->text().toInt();
    info.m_departTimeMin=ui->line_dapartTimeMIn->text().toInt();
    info.m_licenseNum=ui->line_licenseNum->text();
    info.m_phone=ui->line_phone->text();
    info.m_seatNum=ui->line_seatNum->text().toInt();
    info.m_seatPos=ui->line_seatPos->text();
    info.m_seatPrice=ui->line_seatPrice->text().toInt();
    if(m_isAdd)
    {
        ptr->addDriver(info);
        QMessageBox::information(nullptr,"信息","存储成功");
    }
    else
    {
        ptr->UpdateDriverInfo(info);
        QMessageBox::information(nullptr,"信息","修改成功");
    }
}




void Dlg_AddDriver::on_Btn_cancel_clicked()
{
    this->hide();
}

