#ifndef PAGE_REGISTER_H
#define PAGE_REGISTER_H

#include <QWidget>
#include"driversql.h"

namespace Ui {
class page_register;
}

class page_register : public QWidget
{
    Q_OBJECT

public:
    explicit page_register(QWidget *parent = nullptr);
    ~page_register();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::page_register *ui;
    DriverSql *m_ptrDriverSql;

};

#endif // PAGE_REGISTER_H
