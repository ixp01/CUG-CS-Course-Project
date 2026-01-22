#include "dlg_out.h"
#include "ui_dlg_out.h"
#include "driversql.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QVariant>
#include <QString>

dlg_out::dlg_out(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dlg_out)
    , m_cno(-1)
{
    ui->setupUi(this);
}

dlg_out::~dlg_out()
{
    delete ui;
}

void dlg_out::setCarInfo(int cno, const QString &clicense)
{
    m_cno = cno;
    m_clicense = clicense;
    ui->line_cno->setText(QString::number(cno));
    ui->line_cno->setReadOnly(true);  // 设置只读

    // 清空出场时间输入框
    ui->line_outHour->clear();
    ui->line_outMin->clear();
}

void dlg_out::on_Btn_sure_clicked()
{
    // 1. 获取界面数据
    int coutHour = ui->line_outHour->text().toInt();
    int coutMin = ui->line_outMin->text().toInt();
    QString cout = QString("%1:%2").arg(coutHour, 2, 10, QChar('0')).arg(coutMin, 2, 10, QChar('0'));

    // 2. 更新数据库中的出场时间
    auto ptr = DriverSql::getinstance();
    bool updateOk = ptr->addCars2(m_cno, cout, coutHour, coutMin);

    if (updateOk) {
        // 3. 计算停车金额
        QSqlQuery query(QSqlDatabase::database());
        QString feeQuery = QString("SELECT CASE WHEN Ctype = 1 THEN 0 ELSE CASE WHEN (JULIANDAY(Cout) - JULIANDAY(Cin)) * 24 <= 1 THEN 10 WHEN (JULIANDAY(Cout) - JULIANDAY(Cin)) * 24 <= 2 THEN 20 ELSE 30 END END AS Cmoney FROM Cars WHERE Cno = %1").arg(m_cno);

        if (query.exec(feeQuery) && query.next()) {
            int money = query.value(0).toInt();
            QMessageBox::information(this, "停车费用", QString("车牌号为 %1 的车辆停车费用为：%2 元").arg(m_clicense).arg(money));
        } else {
            QMessageBox::warning(this, "错误", "计算停车费用失败。");
        }

        // 4. 关闭对话框
        this->accept();

    } else {
        QMessageBox::warning(this, "错误", "更新出场时间失败，请检查车辆编号。");
    }
}

void dlg_out::on_Btn_cacel_clicked()
{
    this->hide();
}

