#ifndef DONATIONWIDGET_H
#define DONATIONWIDGET_H

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
class DonationWidget;
}

class DonationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DonationWidget(QWidget *parent = nullptr);
    ~DonationWidget();
    
    void refreshData();

private slots:
    void onAddDonation();
    void onApproveDonation();
    void onRejectDonation();
    void onViewCertificate();
    void onFilterChanged(int index);
    void onTableSelectionChanged();

private:
    void setupUI();
    void loadDonations(const QString& filter = "全部");
    void updateButtonStates();
    
    Ui::DonationWidget *ui;
    QTableWidget *tableWidget;
    QPushButton *btnAdd;
    QPushButton *btnApprove;
    QPushButton *btnReject;
    QPushButton *btnCertificate;
    QComboBox *filterCombo;
};

#endif // DONATIONWIDGET_H

