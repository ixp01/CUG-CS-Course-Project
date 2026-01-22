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

    void setType(bool isAdd,DriverInfo info={});

private slots:
    void on_Btn_save_clicked();

    void on_Btn_cancel_clicked();

private:
    Ui::Dlg_AddDriver *ui;
    bool m_isAdd;
    DriverInfo m_info;
};

#endif // DLG_ADDDRIVER_H
