#ifndef APPLICATIONDIALOG_H
#define APPLICATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QMessageBox>
#include "datamanager.h"

class ApplicationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplicationDialog(QWidget *parent = nullptr);
    ~ApplicationDialog();

private slots:
    void onSubmit();

private:
    void setupUI();
    bool validateInput();

    QLineEdit *schoolEdit;
    QLineEdit *applicantEdit;
    QLineEdit *contactEdit;
    QDoubleSpinBox *amountSpin;
    QTextEdit *purposeEdit;
    QPushButton *submitBtn;
    QPushButton *cancelBtn;
};

#endif // APPLICATIONDIALOG_H

