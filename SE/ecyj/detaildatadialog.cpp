#include "detaildatadialog.h"
#include "databasehelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

DetailDataDialog::DetailDataDialog(int testRecordId, QWidget *parent)
    : QDialog(parent)
    , m_testRecordId(testRecordId)
{
    setWindowTitle("详细测量数据");
    setMinimumSize(900, 700);
    
    setupUI();
    loadData();
}

void DetailDataDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 基础信息区域（只读）
    QGroupBox *basicGroup = new QGroupBox("基础信息（来自检测记录）", this);
    QGridLayout *basicLayout = new QGridLayout();
    
    m_secondaryVoltage = new QLineEdit(this);
    m_secondaryVoltage->setReadOnly(true);
    m_tanPhi = new QLineEdit(this);
    m_tanPhi->setReadOnly(true);
    m_meteringPoint = new QLineEdit(this);
    m_meteringPoint->setReadOnly(true);
    
    m_temperature = new QLineEdit(this);
    m_temperature->setReadOnly(true);
    m_testDateCode = new QLineEdit(this);
    m_testDateCode->setReadOnly(true);
    m_humidity = new QLineEdit(this);
    m_humidity->setReadOnly(true);
    
    basicLayout->addWidget(new QLabel("二次电压:"), 0, 0);
    basicLayout->addWidget(m_secondaryVoltage, 0, 1);
    basicLayout->addWidget(new QLabel("tanφ:"), 0, 2);
    basicLayout->addWidget(m_tanPhi, 0, 3);
    basicLayout->addWidget(new QLabel("计量点编号:"), 0, 4);
    basicLayout->addWidget(m_meteringPoint, 0, 5);
    
    basicLayout->addWidget(new QLabel("温度(°C):"), 1, 0);
    basicLayout->addWidget(m_temperature, 1, 1);
    basicLayout->addWidget(new QLabel("测试日期:"), 1, 2);
    basicLayout->addWidget(m_testDateCode, 1, 3);
    basicLayout->addWidget(new QLabel("湿度(%):"), 1, 4);
    basicLayout->addWidget(m_humidity, 1, 5);
    
    basicGroup->setLayout(basicLayout);
    mainLayout->addWidget(basicGroup);
    
    // 项目选择
    QHBoxLayout *itemLayout = new QHBoxLayout();
    itemLayout->addWidget(new QLabel("测量项目:"));
    m_itemNameCombo = new QComboBox(this);
    m_itemNameCombo->addItems({"PT侧", "CT侧"});
    itemLayout->addWidget(m_itemNameCombo);
    itemLayout->addStretch();
    mainLayout->addLayout(itemLayout);
    
    // 测量数据表格
    QGroupBox *dataGroup = new QGroupBox("测量数据表", this);
    QGridLayout *dataLayout = new QGridLayout();
    
    // 表头
    dataLayout->addWidget(new QLabel("<b>档位</b>"), 0, 0);
    dataLayout->addWidget(new QLabel("<b>f(%)</b>"), 0, 1);
    dataLayout->addWidget(new QLabel("<b>d(分)</b>"), 0, 2);
    dataLayout->addWidget(new QLabel("<b>dU(%)</b>"), 0, 3);
    dataLayout->addWidget(new QLabel("<b>Upt:U</b>"), 0, 4);
    dataLayout->addWidget(new QLabel("<b>Uyb:U</b>"), 0, 5);
    
    // ao行
    dataLayout->addWidget(new QLabel("ao"), 1, 0);
    m_aoF = new QDoubleSpinBox(this);
    m_aoF->setRange(-1000, 1000);
    m_aoF->setDecimals(4);
    m_aoD = new QDoubleSpinBox(this);
    m_aoD->setRange(-1000, 1000);
    m_aoD->setDecimals(3);
    m_aoDU = new QDoubleSpinBox(this);
    m_aoDU->setRange(-1000, 1000);
    m_aoDU->setDecimals(4);
    m_aoUpt = new QDoubleSpinBox(this);
    m_aoUpt->setRange(0, 10000);
    m_aoUpt->setDecimals(3);
    m_aoUyb = new QDoubleSpinBox(this);
    m_aoUyb->setRange(0, 10000);
    m_aoUyb->setDecimals(3);
    
    dataLayout->addWidget(m_aoF, 1, 1);
    dataLayout->addWidget(m_aoD, 1, 2);
    dataLayout->addWidget(m_aoDU, 1, 3);
    dataLayout->addWidget(m_aoUpt, 1, 4);
    dataLayout->addWidget(m_aoUyb, 1, 5);
    
    // bo行
    dataLayout->addWidget(new QLabel("bo"), 2, 0);
    m_boF = new QDoubleSpinBox(this);
    m_boF->setRange(-1000, 1000);
    m_boF->setDecimals(4);
    m_boD = new QDoubleSpinBox(this);
    m_boD->setRange(-1000, 1000);
    m_boD->setDecimals(3);
    m_boDU = new QDoubleSpinBox(this);
    m_boDU->setRange(-1000, 1000);
    m_boDU->setDecimals(4);
    m_boUpt = new QDoubleSpinBox(this);
    m_boUpt->setRange(0, 10000);
    m_boUpt->setDecimals(3);
    m_boUyb = new QDoubleSpinBox(this);
    m_boUyb->setRange(0, 10000);
    m_boUyb->setDecimals(3);
    
    dataLayout->addWidget(m_boF, 2, 1);
    dataLayout->addWidget(m_boD, 2, 2);
    dataLayout->addWidget(m_boDU, 2, 3);
    dataLayout->addWidget(m_boUpt, 2, 4);
    dataLayout->addWidget(m_boUyb, 2, 5);
    
    // co行
    dataLayout->addWidget(new QLabel("co"), 3, 0);
    m_coF = new QDoubleSpinBox(this);
    m_coF->setRange(-1000, 1000);
    m_coF->setDecimals(4);
    m_coD = new QDoubleSpinBox(this);
    m_coD->setRange(-1000, 1000);
    m_coD->setDecimals(3);
    m_coDU = new QDoubleSpinBox(this);
    m_coDU->setRange(-1000, 1000);
    m_coDU->setDecimals(4);
    m_coUpt = new QDoubleSpinBox(this);
    m_coUpt->setRange(0, 10000);
    m_coUpt->setDecimals(3);
    m_coUyb = new QDoubleSpinBox(this);
    m_coUyb->setRange(0, 10000);
    m_coUyb->setDecimals(3);
    
    dataLayout->addWidget(m_coF, 3, 1);
    dataLayout->addWidget(m_coD, 3, 2);
    dataLayout->addWidget(m_coDU, 3, 3);
    dataLayout->addWidget(m_coUpt, 3, 4);
    dataLayout->addWidget(m_coUyb, 3, 5);
    
    dataGroup->setLayout(dataLayout);
    mainLayout->addWidget(dataGroup);
    
    // 其他信息
    QFormLayout *otherLayout = new QFormLayout();
    m_ptCheckNote = new QLineEdit(this);
    m_ptCheckNote->setPlaceholderText("例如: PT侧不带自校/按1测量按2储存");
    m_rPercentage = new QDoubleSpinBox(this);
    m_rPercentage->setRange(0, 100);
    m_rPercentage->setDecimals(3);
    m_measurementResult = new QLineEdit(this);
    
    otherLayout->addRow("PT侧备注:", m_ptCheckNote);
    otherLayout->addRow("r%:", m_rPercentage);
    otherLayout->addRow("测量结束:", m_measurementResult);
    
    mainLayout->addLayout(otherLayout);
    
    // 按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (saveData()) {
            accept();
        }
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void DetailDataDialog::loadData()
{
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    // 加载基础信息
    query.prepare("SELECT secondary_voltage, temperature, humidity, "
                  "metering_point_code, test_date_code "
                  "FROM test_records WHERE id = ?");
    query.addBindValue(m_testRecordId);
    if (query.exec() && query.next()) {
        m_secondaryVoltage->setText(query.value(0).toString() + " U");
        m_temperature->setText(query.value(1).toString());
        m_humidity->setText(query.value(2).toString());
        m_meteringPoint->setText(query.value(3).toString());
        m_testDateCode->setText(query.value(4).toString());
        m_tanPhi->setText("-31.2336"); // 示例值
    }
    
    // 加载详细数据
    query.prepare("SELECT item_name, ao_f, ao_d, ao_du, ao_upt, ao_uyb, "
                  "bo_f, bo_d, bo_du, bo_upt, bo_uyb, "
                  "co_f, co_d, co_du, co_upt, co_uyb, "
                  "pt_check_note, r_percentage, measurement_result "
                  "FROM test_result_details WHERE test_record_id = ?");
    query.addBindValue(m_testRecordId);
    if (query.exec() && query.next()) {
        m_itemNameCombo->setCurrentText(query.value(0).toString());
        
        m_aoF->setValue(query.value(1).toDouble());
        m_aoD->setValue(query.value(2).toDouble());
        m_aoDU->setValue(query.value(3).toDouble());
        m_aoUpt->setValue(query.value(4).toDouble());
        m_aoUyb->setValue(query.value(5).toDouble());
        
        m_boF->setValue(query.value(6).toDouble());
        m_boD->setValue(query.value(7).toDouble());
        m_boDU->setValue(query.value(8).toDouble());
        m_boUpt->setValue(query.value(9).toDouble());
        m_boUyb->setValue(query.value(10).toDouble());
        
        m_coF->setValue(query.value(11).toDouble());
        m_coD->setValue(query.value(12).toDouble());
        m_coDU->setValue(query.value(13).toDouble());
        m_coUpt->setValue(query.value(14).toDouble());
        m_coUyb->setValue(query.value(15).toDouble());
        
        m_ptCheckNote->setText(query.value(16).toString());
        m_rPercentage->setValue(query.value(17).toDouble());
        m_measurementResult->setText(query.value(18).toString());
    }
}

bool DetailDataDialog::saveData()
{
    QSqlQuery query(DatabaseHelper::instance()->getDatabase());
    
    // 检查是否已存在记录
    query.prepare("SELECT COUNT(*) FROM test_result_details WHERE test_record_id = ?");
    query.addBindValue(m_testRecordId);
    query.exec();
    query.next();
    bool exists = query.value(0).toInt() > 0;
    
    if (exists) {
        // 更新
        query.prepare("UPDATE test_result_details SET "
                      "item_name = ?, "
                      "ao_f = ?, ao_d = ?, ao_du = ?, ao_upt = ?, ao_uyb = ?, "
                      "bo_f = ?, bo_d = ?, bo_du = ?, bo_upt = ?, bo_uyb = ?, "
                      "co_f = ?, co_d = ?, co_du = ?, co_upt = ?, co_uyb = ?, "
                      "pt_check_note = ?, r_percentage = ?, measurement_result = ? "
                      "WHERE test_record_id = ?");
    } else {
        // 插入
        query.prepare("INSERT INTO test_result_details "
                      "(test_record_id, item_name, "
                      "ao_f, ao_d, ao_du, ao_upt, ao_uyb, "
                      "bo_f, bo_d, bo_du, bo_upt, bo_uyb, "
                      "co_f, co_d, co_du, co_upt, co_uyb, "
                      "pt_check_note, r_percentage, measurement_result) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(m_testRecordId);
    }
    
    query.addBindValue(m_itemNameCombo->currentText());
    query.addBindValue(m_aoF->value());
    query.addBindValue(m_aoD->value());
    query.addBindValue(m_aoDU->value());
    query.addBindValue(m_aoUpt->value());
    query.addBindValue(m_aoUyb->value());
    
    query.addBindValue(m_boF->value());
    query.addBindValue(m_boD->value());
    query.addBindValue(m_boDU->value());
    query.addBindValue(m_boUpt->value());
    query.addBindValue(m_boUyb->value());
    
    query.addBindValue(m_coF->value());
    query.addBindValue(m_coD->value());
    query.addBindValue(m_coDU->value());
    query.addBindValue(m_coUpt->value());
    query.addBindValue(m_coUyb->value());
    
    query.addBindValue(m_ptCheckNote->text());
    query.addBindValue(m_rPercentage->value());
    query.addBindValue(m_measurementResult->text());
    
    if (exists) {
        query.addBindValue(m_testRecordId);
    }
    
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "保存数据失败: " + query.lastError().text());
        return false;
    }
    
    QMessageBox::information(this, "成功", "详细数据已保存");
    return true;
}
