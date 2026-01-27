#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogType {
        AddManufacturer,
        AddEquipment,
        AddTestRecord,
        AddTestItem
    };
    
    explicit AddDialog(DialogType type, QWidget *parent = nullptr);
    
    // 获取输入数据
    QMap<QString, QVariant> getData() const;

private:
    void setupManufacturerUI();
    void setupEquipmentUI();
    void setupTestRecordUI();
    void setupTestItemUI();
    
    DialogType m_type;
    
    // 厂商信息字段
    QLineEdit *m_manufacturerName;
    QLineEdit *m_contactPerson;
    QLineEdit *m_phone;
    QLineEdit *m_address;
    
    // 设备信息字段
    QLineEdit *m_productCode;
    QLineEdit *m_productName;
    QComboBox *m_manufacturerCombo;
    QDateEdit *m_productionDate;
    QLineEdit *m_productLocation;
    QLineEdit *m_productModel;
    
    // 检测记录字段
    QComboBox *m_equipmentCombo;
    QDateEdit *m_testDate;
    QLineEdit *m_testerName;
    QLineEdit *m_testLocation;
    QDoubleSpinBox *m_secondaryVoltage;
    QDoubleSpinBox *m_temperature;
    QDoubleSpinBox *m_humidity;
    QLineEdit *m_meteringPointCode;
    QLineEdit *m_testDateCode;
    QTextEdit *m_remarks;
    
    // 检测结果项字段
    QComboBox *m_testRecordCombo;
    QComboBox *m_itemNameCombo;  // PT1/PT2/CT1/CT2
    QLineEdit *m_gearPosition;
    QDoubleSpinBox *m_percentage;
    QDoubleSpinBox *m_dataLowerLimit;
    QDoubleSpinBox *m_dataUpperLimit;
    QDoubleSpinBox *m_measuredData;
    QCheckBox *m_isQualified;
};

#endif // ADDDIALOG_H
