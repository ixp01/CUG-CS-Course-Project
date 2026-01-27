#ifndef DONATIONDIALOG_H
#define DONATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QMessageBox>
#include "datamanager.h"

class DonationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DonationDialog(QWidget *parent = nullptr);
    ~DonationDialog();

private slots:
    void onDonorTypeChanged(int index);
    void onSubmit();

private:
    void setupUI();
    bool validateInput();

    QComboBox *typeCombo;
    QLineEdit *nameEdit;
    QLineEdit *idEdit;
    QLineEdit *contactEdit;
    QDoubleSpinBox *amountSpin;
    QTextEdit *descEdit;
    QPushButton *submitBtn;
    QPushButton *cancelBtn;
};

#endif // DONATIONDIALOG_H

