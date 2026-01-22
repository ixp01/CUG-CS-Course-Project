#ifndef DLG_OUT_H
#define DLG_OUT_H

#include <QDialog>

namespace Ui {
class dlg_out;
}

class dlg_out : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_out(QWidget *parent = nullptr);
    ~dlg_out();

    void setCarInfo(int cno, const QString &clicense);

private slots:
    void on_Btn_sure_clicked();

    void on_Btn_cacel_clicked();

private:
    Ui::dlg_out *ui;
    int m_cno;           // 车辆编号
    QString m_clicense;  // 车牌号
};

#endif // DLG_OUT_H
