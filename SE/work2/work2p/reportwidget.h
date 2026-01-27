#ifndef REPORTWIDGET_H
#define REPORTWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include "datamanager.h"

namespace Ui {
class ReportWidget;
}

class ReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportWidget(QWidget *parent = nullptr);
    ~ReportWidget();
    
    void refreshData();

private slots:
    void onMonthChanged();
    void onExportReport();

private:
    void setupUI();
    void loadMonthlyReport();
    void updateStatistics();
    
    Ui::ReportWidget *ui;
    
    // 统计信息标签
    QLabel *totalIncomeLabel;
    QLabel *totalExpenseLabel;
    QLabel *balanceLabel;
    QLabel *monthIncomeLabel;
    QLabel *monthExpenseLabel;
    
    // 月份选择
    QComboBox *yearCombo;
    QComboBox *monthCombo;
    
    // 数据表格
    QTableWidget *tableWidget;
    
    QPushButton *exportBtn;
};

#endif // REPORTWIDGET_H

