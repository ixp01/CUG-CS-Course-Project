#include "adddialog.h"
#include "databasehelper.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSqlQuery>
#include <QDate>

AddDialog::AddDialog(DialogType type, QWidget *parent)
    : QDialog(parent)
    , m_type(type)
{
    setMinimumWidth(400);
    
    switch (type) {
    case AddManufacturer:
        setWindowTitle("添加厂商");
        setupManufacturerUI();
        break;
    case AddEquipment:
        setWindowTitle("添加设备");
        setupEquipmentUI();
        break;
    case AddTestRecord:
        setWindowTitle("添加检测记录");
        setupTestRecordUI();
        break;
    case AddTestItem:
        setWindowTitle("添加检测结果项");
        setupTestItemUI();
        break;
    }
}

void AddDialog::setupManufacturerUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    
    m_manufacturerName = new QLineEdit(this);
    m_contactPerson = new QLineEdit(this);
    m_phone = new QLineEdit(this);
    m_address = new QLineEdit(this);
    
    formLayout->addRow("厂商名称*:", m_manufacturerName);
    formLayout->addRow("联系人:", m_contactPerson);
    formLayout->addRow("电话:", m_phone);
    formLayout->addRow("地址:", m_address);
    
    layout->addLayout(formLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

void AddDialog::setupEquipmentUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    
    m_productCode = new QLineEdit(this);
    m_productName = new QLineEdit(this);
    m_manufacturerCombo = new QComboBox(this);
    m_productionDate = new QDateEdit(QDate::currentDate(), this);
    m_productionDate->setCalendarPopup(true);
    m_productLocation = new QLineEdit(this);
    m_productModel = new QLineEdit(this);
    
    // 加载厂商列表
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.exec("SELECT id, name FROM manufacturers ORDER BY name");
    while (query.next()) {
        m_manufacturerCombo->addItem(query.value(1).toString(), query.value(0));
    }
    
    formLayout->addRow("产品编号*:", m_productCode);
    formLayout->addRow("产品名称*:", m_productName);
    formLayout->addRow("厂商:", m_manufacturerCombo);
    formLayout->addRow("生产日期:", m_productionDate);
    formLayout->addRow("产地:", m_productLocation);
    formLayout->addRow("产品型号:", m_productModel);
    
    layout->addLayout(formLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

void AddDialog::setupTestRecordUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    
    m_equipmentCombo = new QComboBox(this);
    m_testDate = new QDateEdit(QDate::currentDate(), this);
    m_testDate->setCalendarPopup(true);
    m_testerName = new QLineEdit(this);
    m_testLocation = new QLineEdit(this);
    m_secondaryVoltage = new QDoubleSpinBox(this);
    m_secondaryVoltage->setRange(0, 1000);
    m_secondaryVoltage->setDecimals(2);
    m_temperature = new QDoubleSpinBox(this);
    m_temperature->setRange(-50, 100);
    m_temperature->setDecimals(1);
    m_humidity = new QDoubleSpinBox(this);
    m_humidity->setRange(0, 100);
    m_humidity->setDecimals(1);
    m_meteringPointCode = new QLineEdit(this);
    m_testDateCode = new QLineEdit(this);
    m_remarks = new QTextEdit(this);
    m_remarks->setMaximumHeight(60);
    
    // 加载设备列表
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.exec("SELECT id, product_code, product_name FROM equipment_info ORDER BY product_code");
    while (query.next()) {
        QString text = QString("%1 - %2").arg(query.value(1).toString(), query.value(2).toString());
        m_equipmentCombo->addItem(text, query.value(0));
    }
    
    formLayout->addRow("设备*:", m_equipmentCombo);
    formLayout->addRow("检测日期*:", m_testDate);
    formLayout->addRow("检测员*:", m_testerName);
    formLayout->addRow("检测地点*:", m_testLocation);
    formLayout->addRow("二次电压(U):", m_secondaryVoltage);
    formLayout->addRow("温度(°C):", m_temperature);
    formLayout->addRow("湿度(%):", m_humidity);
    formLayout->addRow("计量点编号:", m_meteringPointCode);
    formLayout->addRow("测试日期编号:", m_testDateCode);
    formLayout->addRow("备注:", m_remarks);
    
    layout->addLayout(formLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

void AddDialog::setupTestItemUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    
    m_testRecordCombo = new QComboBox(this);
    m_itemNameCombo = new QComboBox(this);
    m_itemNameCombo->addItems({"PT1", "PT2", "CT1", "CT2"});
    m_gearPosition = new QLineEdit(this);
    m_gearPosition->setPlaceholderText("例如: 100V 或 5A");
    m_percentage = new QDoubleSpinBox(this);
    m_percentage->setRange(0, 100);
    m_percentage->setSuffix("%");
    m_dataLowerLimit = new QDoubleSpinBox(this);
    m_dataLowerLimit->setRange(0, 1000);
    m_dataLowerLimit->setDecimals(2);
    m_dataUpperLimit = new QDoubleSpinBox(this);
    m_dataUpperLimit->setRange(0, 1000);
    m_dataUpperLimit->setDecimals(2);
    m_measuredData = new QDoubleSpinBox(this);
    m_measuredData->setRange(0, 1000);
    m_measuredData->setDecimals(2);
    m_isQualified = new QCheckBox("合格", this);
    
    // 加载检测记录列表
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    query.exec("SELECT tr.id, tr.test_date, ei.product_code "
               "FROM test_records tr "
               "JOIN equipment_info ei ON tr.equipment_id = ei.id "
               "ORDER BY tr.test_date DESC");
    while (query.next()) {
        QString text = QString("%1 - %2").arg(query.value(1).toString(), query.value(2).toString());
        m_testRecordCombo->addItem(text, query.value(0));
    }
    
    formLayout->addRow("检测记录*:", m_testRecordCombo);
    formLayout->addRow("项目名称*:", m_itemNameCombo);
    formLayout->addRow("档位:", m_gearPosition);
    formLayout->addRow("百分比:", m_percentage);
    formLayout->addRow("数据下限:", m_dataLowerLimit);
    formLayout->addRow("数据上限:", m_dataUpperLimit);
    formLayout->addRow("实测数据:", m_measuredData);
    formLayout->addRow("", m_isQualified);
    
    layout->addLayout(formLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

QMap<QString, QVariant> AddDialog::getData() const
{
    QMap<QString, QVariant> data;
    
    switch (m_type) {
    case AddManufacturer:
        data["name"] = m_manufacturerName->text();
        data["contact_person"] = m_contactPerson->text();
        data["phone"] = m_phone->text();
        data["address"] = m_address->text();
        break;
        
    case AddEquipment:
        data["product_code"] = m_productCode->text();
        data["product_name"] = m_productName->text();
        data["manufacturer_id"] = m_manufacturerCombo->currentData();
        data["production_date"] = m_productionDate->date().toString("yyyy-MM-dd");
        data["product_location"] = m_productLocation->text();
        data["product_model"] = m_productModel->text();
        break;
        
    case AddTestRecord:
        data["equipment_id"] = m_equipmentCombo->currentData();
        data["test_date"] = m_testDate->date().toString("yyyy-MM-dd");
        data["tester_name"] = m_testerName->text();
        data["test_location"] = m_testLocation->text();
        data["secondary_voltage"] = m_secondaryVoltage->value();
        data["temperature"] = m_temperature->value();
        data["humidity"] = m_humidity->value();
        data["metering_point_code"] = m_meteringPointCode->text();
        data["test_date_code"] = m_testDateCode->text();
        data["remarks"] = m_remarks->toPlainText();
        break;
        
    case AddTestItem:
        data["test_record_id"] = m_testRecordCombo->currentData();
        data["item_name"] = m_itemNameCombo->currentText();
        data["gear_position"] = m_gearPosition->text();
        data["percentage"] = m_percentage->value();
        data["data_lower_limit"] = m_dataLowerLimit->value();
        data["data_upper_limit"] = m_dataUpperLimit->value();
        data["measured_data"] = m_measuredData->value();
        data["is_qualified"] = m_isQualified->isChecked();
        break;
    }
    
    return data;
}


