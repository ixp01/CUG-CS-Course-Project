#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QDateEdit>
#include <QComboBox>

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr);

private slots:
    void onStatByDate();
    void onStatByManufacturer();
    void onStatByMonth();

private:
    void setupUI();
    
    QDateEdit *m_startDate;
    QDateEdit *m_endDate;
    QComboBox *m_statType;
    QTableWidget *m_resultTable;
};

#endif // STATISTICSDIALOG_H
