#ifndef ADDFLIGHTDIALOG_H
#define ADDFLIGHTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include "data.h"

class AddFlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFlightDialog(QWidget *parent = nullptr);
    explicit AddFlightDialog(const Flight& flight, QWidget *parent = nullptr); // 编辑模式
    ~AddFlightDialog();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onDepartureCityChanged();
    void onArrivalCityChanged();

private:
    void setupUI();
    void setupConnections();
    void loadCities();
    void loadFlightData(const Flight& flight); // 加载航班数据到界面
    bool validateInput();
    Flight getFlightData();
    void validateCitySelection(); // 验证城市选择

    // UI组件
    QLineEdit *flightNumberEdit;
    QComboBox *airlineCombo;
    QComboBox *departureCityCombo;
    QComboBox *arrivalCityCombo;
    QDateTimeEdit *departureTimeEdit;
    QDateTimeEdit *arrivalTimeEdit;
    QSpinBox *totalSeatsSpinBox;
    QSpinBox *availableSeatsSpinBox;
    QDoubleSpinBox *priceSpinBox;
    QComboBox *statusCombo;
    QLineEdit *stopoverEdit;
    
    QPushButton *saveButton;
    QPushButton *cancelButton;
    
    // 编辑模式相关
    bool isEditMode;
    QString originalFlightNumber;
};

#endif // ADDFLIGHTDIALOG_H 