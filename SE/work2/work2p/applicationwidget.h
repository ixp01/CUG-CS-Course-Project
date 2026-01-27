#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include "datamanager.h"

namespace Ui {
class ApplicationWidget;
}

class ApplicationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicationWidget(QWidget *parent = nullptr);
    ~ApplicationWidget();
    
    void refreshData();

private slots:
    void onAddApplication();
    void onApproveApplication();
    void onRejectApplication();
    void onFilterChanged(int index);
    void onTableSelectionChanged();

private:
    void setupUI();
    void loadApplications(const QString& filter = "全部");
    void updateButtonStates();
    
    Ui::ApplicationWidget *ui;
    QTableWidget *tableWidget;
    QPushButton *btnAdd;
    QPushButton *btnApprove;
    QPushButton *btnReject;
    QComboBox *filterCombo;
};

#endif // APPLICATIONWIDGET_H

