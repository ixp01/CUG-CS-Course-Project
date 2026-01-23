#include "addflightdialog.h"
#include "common.h"
#include <QRegularExpression>
#include <QValidator>

AddFlightDialog::AddFlightDialog(QWidget *parent)
    : QDialog(parent), isEditMode(false)
{
    setWindowTitle("添加新航班");
    setModal(true);
    setFixedSize(500, 600);
    
    setupUI();
    setupConnections();
    loadCities();
    
    // 设置默认值
    departureTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    arrivalTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1).addSecs(3600)); // 默认1小时后
    statusCombo->setCurrentText("Scheduled");
    totalSeatsSpinBox->setValue(150);
    availableSeatsSpinBox->setValue(150);
    priceSpinBox->setValue(500.0);
}

AddFlightDialog::AddFlightDialog(const Flight& flight, QWidget *parent)
    : QDialog(parent), isEditMode(true), originalFlightNumber(flight.flightNumber)
{
    setWindowTitle("编辑航班");
    setModal(true);
    setFixedSize(500, 600);
    
    setupUI();
    setupConnections();
    loadCities();
    loadFlightData(flight);
}

AddFlightDialog::~AddFlightDialog()
{
}

void AddFlightDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout();
    
    // 航班号
    flightNumberEdit = new QLineEdit();
    flightNumberEdit->setPlaceholderText("例如: CA1234");
    // 设置航班号格式验证
    QRegularExpression flightRegex("^[A-Z]{2}[0-9]{3,4}$");
    QRegularExpressionValidator *flightValidator = new QRegularExpressionValidator(flightRegex, this);
    flightNumberEdit->setValidator(flightValidator);
    formLayout->addRow("航班号*:", flightNumberEdit);
    
    // 航空公司
    airlineCombo = new QComboBox();
    airlineCombo->addItems({
        "中国国际航空", "中国南方航空", "中国东方航空", "海南航空",
        "四川航空", "山东航空", "厦门航空", "深圳航空", "春秋航空"
    });
    formLayout->addRow("航空公司*:", airlineCombo);
    
    // 出发城市
    departureCityCombo = new QComboBox();
    departureCityCombo->setEditable(true);
    formLayout->addRow("出发城市*:", departureCityCombo);
    
    // 到达城市
    arrivalCityCombo = new QComboBox();
    arrivalCityCombo->setEditable(true);
    formLayout->addRow("到达城市*:", arrivalCityCombo);
    
    // 出发时间
    departureTimeEdit = new QDateTimeEdit();
    departureTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    departureTimeEdit->setMinimumDateTime(QDateTime::currentDateTime());
    departureTimeEdit->setCalendarPopup(true);
    formLayout->addRow("出发时间*:", departureTimeEdit);
    
    // 到达时间
    arrivalTimeEdit = new QDateTimeEdit();
    arrivalTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    arrivalTimeEdit->setMinimumDateTime(QDateTime::currentDateTime());
    arrivalTimeEdit->setCalendarPopup(true);
    formLayout->addRow("到达时间*:", arrivalTimeEdit);
    
    // 总座位数
    totalSeatsSpinBox = new QSpinBox();
    totalSeatsSpinBox->setRange(50, 500);
    totalSeatsSpinBox->setSuffix(" 座");
    formLayout->addRow("总座位数*:", totalSeatsSpinBox);
    
    // 可用座位数
    availableSeatsSpinBox = new QSpinBox();
    availableSeatsSpinBox->setRange(0, 500);
    availableSeatsSpinBox->setSuffix(" 座");
    formLayout->addRow("可用座位*:", availableSeatsSpinBox);
    
    // 票价
    priceSpinBox = new QDoubleSpinBox();
    priceSpinBox->setRange(100.0, 5000.0);
    priceSpinBox->setDecimals(0);
    priceSpinBox->setSuffix(" 元");
    formLayout->addRow("票价*:", priceSpinBox);
    
    // 航班状态
    statusCombo = new QComboBox();
    statusCombo->addItems({"Scheduled", "Delayed", "Cancelled"});
    formLayout->addRow("状态:", statusCombo);
    
    // 经停城市
    stopoverEdit = new QLineEdit();
    stopoverEdit->setPlaceholderText("多个城市用逗号分隔，例如: 武汉,郑州");
    formLayout->addRow("经停城市:", stopoverEdit);
    
    mainLayout->addLayout(formLayout);
    
    // 添加一些说明文字
    QLabel *noteLabel = new QLabel("注意：带*号的字段为必填项");
    noteLabel->setStyleSheet("color: #666; font-size: 12px;");
    mainLayout->addWidget(noteLabel);
    
    mainLayout->addStretch();
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("取消");
    saveButton = new QPushButton("保存");
    saveButton->setDefault(true);
    saveButton->setStyleSheet("QPushButton { background-color: #007bff; color: white; font-weight: bold; padding: 8px 16px; }");
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    
    mainLayout->addLayout(buttonLayout);
}

void AddFlightDialog::setupConnections()
{
    connect(saveButton, &QPushButton::clicked, this, &AddFlightDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &AddFlightDialog::onCancelClicked);
    connect(departureCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &AddFlightDialog::onDepartureCityChanged);
    connect(arrivalCityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &AddFlightDialog::onArrivalCityChanged);
    
    // 总座位数变化时，自动调整可用座位数的最大值
    connect(totalSeatsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) {
        availableSeatsSpinBox->setMaximum(value);
        if (availableSeatsSpinBox->value() > value) {
            availableSeatsSpinBox->setValue(value);
        }
    });
    
    // 出发时间变化时，调整到达时间的最小值
    connect(departureTimeEdit, &QDateTimeEdit::dateTimeChanged,
            [this](const QDateTime &dateTime) {
        arrivalTimeEdit->setMinimumDateTime(dateTime.addSecs(1800)); // 至少30分钟后
    });
}

void AddFlightDialog::loadCities()
{
    QVector<QString> cities = DatabaseManager::instance().getAllCities();
    
    departureCityCombo->addItems(cities);
    arrivalCityCombo->addItems(cities);
}

void AddFlightDialog::loadFlightData(const Flight& flight)
{
    // 加载航班数据到界面
    flightNumberEdit->setText(flight.flightNumber);
    flightNumberEdit->setReadOnly(true); // 编辑模式下航班号不可修改
    
    // 设置航空公司
    int airlineIndex = airlineCombo->findText(flight.airline);
    if (airlineIndex >= 0) {
        airlineCombo->setCurrentIndex(airlineIndex);
    } else {
        airlineCombo->setCurrentText(flight.airline);
    }
    
    // 设置城市
    departureCityCombo->setCurrentText(flight.departureCity);
    arrivalCityCombo->setCurrentText(flight.arrivalCity);
    
    // 设置时间
    departureTimeEdit->setDateTime(flight.departureTime);
    arrivalTimeEdit->setDateTime(flight.arrivalTime);
    
    // 设置座位和价格
    totalSeatsSpinBox->setValue(flight.totalSeats);
    availableSeatsSpinBox->setValue(flight.availableSeats);
    priceSpinBox->setValue(flight.price);
    
    // 设置状态
    statusCombo->setCurrentText(flight.status);
    
    // 设置经停城市
    if (!flight.stopovers.isEmpty()) {
        stopoverEdit->setText(flight.stopovers.join(","));
    }
}

void AddFlightDialog::onSaveClicked()
{
    if (!validateInput()) {
        return;
    }
    
    Flight flight = getFlightData();
    
    bool success = false;
    QString operation;
    
    if (isEditMode) {
        // 编辑模式：更新航班
        success = DatabaseManager::instance().updateFlight(flight);
        operation = "更新";
    } else {
        // 添加模式：新增航班
        success = DatabaseManager::instance().addFlight(flight);
        operation = "添加";
    }
    
    if (success) {
        QMessageBox::information(this, "成功", QString("航班%1成功！").arg(operation));
        accept();
    } else {
        QMessageBox::critical(this, "错误", 
            QString("航班%1失败，请检查航班号是否重复或其他错误。").arg(operation));
    }
}

void AddFlightDialog::onCancelClicked()
{
    reject();
}

void AddFlightDialog::onDepartureCityChanged()
{
    validateCitySelection();
}

void AddFlightDialog::onArrivalCityChanged()
{
    validateCitySelection();
}

bool AddFlightDialog::validateInput()
{
    // 检查必填字段
    if (flightNumberEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入航班号！");
        flightNumberEdit->setFocus();
        return false;
    }
    
    if (departureCityCombo->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择出发城市！");
        departureCityCombo->setFocus();
        return false;
    }
    
    if (arrivalCityCombo->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择到达城市！");
        arrivalCityCombo->setFocus();
        return false;
    }
    
    // 检查城市不能相同
    if (departureCityCombo->currentText() == arrivalCityCombo->currentText()) {
        QMessageBox::warning(this, "输入错误", "出发城市和到达城市不能相同！");
        arrivalCityCombo->setFocus();
        return false;
    }
    
    // 检查时间逻辑
    if (departureTimeEdit->dateTime() >= arrivalTimeEdit->dateTime()) {
        QMessageBox::warning(this, "输入错误", "到达时间必须晚于出发时间！");
        arrivalTimeEdit->setFocus();
        return false;
    }
    
    // 检查座位数
    if (availableSeatsSpinBox->value() > totalSeatsSpinBox->value()) {
        QMessageBox::warning(this, "输入错误", "可用座位数不能超过总座位数！");
        availableSeatsSpinBox->setFocus();
        return false;
    }
    
    return true;
}

Flight AddFlightDialog::getFlightData()
{
    Flight flight;
    
    flight.flightNumber = flightNumberEdit->text().trimmed().toUpper();
    flight.airline = airlineCombo->currentText();
    flight.departureCity = departureCityCombo->currentText().trimmed();
    flight.arrivalCity = arrivalCityCombo->currentText().trimmed();
    flight.departureTime = departureTimeEdit->dateTime();
    flight.arrivalTime = arrivalTimeEdit->dateTime();
    flight.totalSeats = totalSeatsSpinBox->value();
    flight.availableSeats = availableSeatsSpinBox->value();
    flight.price = priceSpinBox->value();
    flight.status = statusCombo->currentText();
    
    // 处理经停城市
    QString stopoverText = stopoverEdit->text().trimmed();
    if (!stopoverText.isEmpty()) {
        flight.stopovers = stopoverText.split(",", Qt::SkipEmptyParts);
        // 去除每个城市名的前后空格
        for (QString &city : flight.stopovers) {
            city = city.trimmed();
        }
    }
    
    return flight;
}

void AddFlightDialog::validateCitySelection()
{
    QString departureCity = departureCityCombo->currentText();
    QString arrivalCity = arrivalCityCombo->currentText();
    
    // 只在两个城市都不为空且相同时才给出视觉提示
    bool citiesAreSame = (departureCity == arrivalCity && !departureCity.isEmpty() && !arrivalCity.isEmpty());
    
    // 设置样式来提示用户（而不是弹出对话框）
    QString normalStyle = "";
    QString errorStyle = "QComboBox { border: 2px solid red; }";
    
    if (citiesAreSame) {
        departureCityCombo->setStyleSheet(errorStyle);
        arrivalCityCombo->setStyleSheet(errorStyle);
        departureCityCombo->setToolTip("出发城市和到达城市不能相同");
        arrivalCityCombo->setToolTip("出发城市和到达城市不能相同");
    } else {
        departureCityCombo->setStyleSheet(normalStyle);
        arrivalCityCombo->setStyleSheet(normalStyle);
        departureCityCombo->setToolTip("");
        arrivalCityCombo->setToolTip("");
    }
} 