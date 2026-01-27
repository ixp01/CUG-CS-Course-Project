#ifndef DETAILDATADIALOG_H
#define DETAILDATADIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>

class DetailDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetailDataDialog(int testRecordId, QWidget *parent = nullptr);
    
    // 保存数据
    bool saveData();

private:
    void setupUI();
    void loadData();
    
    int m_testRecordId;
    
    // 基础信息（只读）
    QLineEdit *m_secondaryVoltage;
    QLineEdit *m_temperature;
    QLineEdit *m_humidity;
    QLineEdit *m_tanPhi;
    QLineEdit *m_meteringPoint;
    QLineEdit *m_testDateCode;
    
    // 项目名称
    QComboBox *m_itemNameCombo;  // PT侧/CT侧
    
    // ao数据
    QDoubleSpinBox *m_aoF;
    QDoubleSpinBox *m_aoD;
    QDoubleSpinBox *m_aoDU;
    QDoubleSpinBox *m_aoUpt;
    QDoubleSpinBox *m_aoUyb;
    
    // bo数据
    QDoubleSpinBox *m_boF;
    QDoubleSpinBox *m_boD;
    QDoubleSpinBox *m_boDU;
    QDoubleSpinBox *m_boUpt;
    QDoubleSpinBox *m_boUyb;
    
    // co数据
    QDoubleSpinBox *m_coF;
    QDoubleSpinBox *m_coD;
    QDoubleSpinBox *m_coDU;
    QDoubleSpinBox *m_coUpt;
    QDoubleSpinBox *m_coUyb;
    
    // 其他信息
    QLineEdit *m_ptCheckNote;
    QDoubleSpinBox *m_rPercentage;
    QLineEdit *m_measurementResult;
};

#endif // DETAILDATADIALOG_H
