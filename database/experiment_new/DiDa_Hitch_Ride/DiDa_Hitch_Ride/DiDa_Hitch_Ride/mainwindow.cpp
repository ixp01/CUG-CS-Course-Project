#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTreeWidget>
#include<QKeyEvent>
#include<QFile>
#include<QCoreApplication>
#include<QString>
#include<QMessageBox>
#include<QSqlQuery>
#include<QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_ptrDriverSql(nullptr)
{
    ui->setupUi(this);



    m_dlgLogin.show();
    auto h=[&](){
        m_DlgRegister.show();
    };
    connect(&m_dlgLogin,&Page_Login::sendRegisterSuccess,this,h);

    auto f=[&](){
      this->show();
    };
    connect(&m_dlgLogin,&Page_Login::sendLoginSuccess,this,f);

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(1);
    QStringList l;
    l<<"停车场管理系统";


    QTreeWidgetItem *pf=new QTreeWidgetItem(ui->treeWidget,l);
    ui->treeWidget->addTopLevelItem(pf);
    ui->treeWidget->expandAll();
    ui->stackedWidget->setCurrentIndex(0);

    m_ptrDriverSql=DriverSql::getinstance();
    updateTable();//重启界面

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Btn_exit_clicked()
{
    exit(0);
}


//更新图表
void MainWindow::on_pushButton_5_clicked()
{
   updateTable();
}

//这个为入场按钮
void MainWindow::on_Btn_save_clicked()
{
    m_dlgAddDriver.setType(true);
    m_dlgAddDriver.exec();
    updateTable();
}


void MainWindow::on_Btn_clear_clicked()
{
    m_ptrDriverSql->clearCarTable();
    updateTable();
}


void MainWindow::updateTable()
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(8);
    QStringList l;
    l << "车序号" << "车主号" << "车牌号" << "停车位号" << "保安号" << "进入时间" << "出场时间" << "停车类型";
    ui->tableWidget->setHorizontalHeaderLabels(l);
    //只选中行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto ptr = DriverSql::getinstance();

    // 获取所有车辆信息
    QList<Carsinfo> cars = ptr->getAllCarsInfo();

    // 获取所有车主信息 (用于查找车主姓名)
    QList<Driversinfo> drivers = ptr->getAllDriversInfo();

    // 获取车主-车辆关联信息 (用于匹配车主和车辆)
    QList<DriverCar> driverCars = ptr->getAllDriverCarInfo();

    ui->tableWidget->setRowCount(cars.size());

    for (int i = 0; i < cars.size(); ++i)
    {
        Carsinfo car = cars.at(i);

        // 查找对应的车主号 (可能一个车主对应多辆车)
        int dno = -1; // 默认值
        for (const auto& dc : driverCars) {
            if (dc.m_cno == car.m_cno) {
                dno = dc.m_dno;
                break;
            }
        }

        // 查找对应的车主姓名 (如果找到了车主号)
        QString dname = "未知车主";
        if (dno != -1) {
            for (const auto& driver : drivers) {
                if (driver.m_dno == dno) {
                    dname = driver.m_dname;
                    break;
                }
            }
        }

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(car.m_cno))); // 车序号 (使用cno)
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(dno))); // 车主号 (使用dno)
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(car.m_clicense)); // 车牌号
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(car.m_pno))); // 停车位号
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(car.m_username)); // 保安号 (假设username是保安号)
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(car.m_cin)); // 进入时间
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(car.m_cout)); // 出场时间

        // 车辆类型转换 1:月卡车, 2:临时车
        QString ctypeStr = (car.m_ctype == 1) ? "月卡车" : "临时车";
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(ctypeStr)); // 停车类型
    }
}


void MainWindow::on_Btn_delete_clicked()
{
    int i=ui->tableWidget->currentRow();
    if(i>=0)
    {
        int id=  ui->tableWidget->item(i,1)->text().toInt();
        m_ptrDriverSql->delCar(id);
      updateTable();
      QMessageBox::information(nullptr,"信息","删除成功");

    }

}

//选中一行，然后对这一行进行操作
void MainWindow::on_Btn_update_clicked()
{
    int i=ui->tableWidget->currentRow();
    if(i>=0)
    {
        // 1. 从表格获取车辆编号和车主编号
        int cno = ui->tableWidget->item(i, 0)->text().toInt();
        int dno = ui->tableWidget->item(i, 1)->text().toInt(); // 假设表格第二列是车主号

        // 2. 从数据库查询完整信息
        auto ptr = DriverSql::getinstance();
        Carsinfo cinfo = ptr->getCarInfoByCno(cno);
        Driversinfo dinfo = ptr->getDriverInfoByDno(dno);
        DriverCar dcinfo = ptr->getDriverCarByCno(cno); // 根据cno查关联

        // 根据车辆的pno查询停车位信息
        Parkinginfo pinfo;
        if (cinfo.m_pno != -1) { // 确保车辆信息有效且有停车位号
            pinfo = ptr->getParkingInfoByPno(cinfo.m_pno);
        }

        // 3. 调用setType设置修改对话框
        // 注意：这里将Parkinginfo和DriverCar也传递过去，如果setType需要的话
        m_dlgAddDriver.setType(false, dinfo, cinfo, pinfo, dcinfo);

        // 4. 显示修改对话框
        m_dlgAddDriver.exec();

    }
    // 无论是否修改成功，都刷新表格
    updateTable();
}


void MainWindow::on_Btn_search_clicked()
{
    QString searchText = ui->line_search->text().trimmed(); // 获取搜索文本并去除空白
    
    // 如果搜索文本为空，显示所有数据
    if (searchText.isEmpty()) {
        updateTable();
        return;
    }

    auto ptr = DriverSql::getinstance();

    // 获取所有车辆信息
    QList<Carsinfo> allCars = ptr->getAllCarsInfo();

    // 获取所有车主信息 (用于查找车主姓名)
    QList<Driversinfo> allDrivers = ptr->getAllDriversInfo();

    // 获取车主-车辆关联信息 (用于匹配车主和车辆)
    QList<DriverCar> allDriverCars = ptr->getAllDriverCarInfo();

    QList<Carsinfo> filteredCars; // 存储筛选后的车辆

    // 筛选车辆：匹配车辆编号或车牌号
    for (const auto& car : allCars) {
        if (QString::number(car.m_cno).contains(searchText, Qt::CaseInsensitive) ||
            car.m_clicense.contains(searchText, Qt::CaseInsensitive))
        {
            filteredCars.append(car);
        }
    }

    // 如果没有找到匹配的车辆，显示提示信息并返回
    if (filteredCars.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的车辆信息！");
        return;
    }

    // 清空并设置表格
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(8);
    QStringList l;
    l << "车序号" << "车主号" << "车牌号" << "停车位号" << "保安号" << "进入时间" << "出场时间" << "停车类型";
    ui->tableWidget->setHorizontalHeaderLabels(l);
    
    // 设置表格选择模式
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 只显示匹配的行
    ui->tableWidget->setRowCount(filteredCars.size());

    for (int i = 0; i < filteredCars.size(); ++i)
    {
        Carsinfo car = filteredCars.at(i);

        // 查找对应的车主号
        int dno = -1;
        for (const auto& dc : allDriverCars) {
            if (dc.m_cno == car.m_cno) {
                dno = dc.m_dno;
                break;
            }
        }

        // 查找对应的车主姓名
        QString dname = "未知车主";
        if (dno != -1) {
            for (const auto& driver : allDrivers) {
                if (driver.m_dno == dno) {
                    dname = driver.m_dname;
                    break;
                }
            }
        }

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(car.m_cno))); // 车序号
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(dno))); // 车主号
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(car.m_clicense)); // 车牌号
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(car.m_pno))); // 停车位号
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(car.m_username)); // 保安号
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(car.m_cin)); // 进入时间
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(car.m_cout)); // 出场时间

        // 车辆类型转换 1:月卡车, 2:临时车
        QString ctypeStr = (car.m_ctype == 1) ? "月卡车" : "临时车";
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(ctypeStr)); // 停车类型
    }
}

//搜索车主
void MainWindow::on_Btn_searchD_clicked()
{
    QString searchText = ui->line_searchD->text().trimmed(); // 获取搜索文本并去除空白
    
    // 如果搜索文本为空，显示所有数据
    if (searchText.isEmpty()) {
        updateTable();
        return;
    }

    auto ptr = DriverSql::getinstance();

    // 获取所有车辆信息
    QList<Carsinfo> allCars = ptr->getAllCarsInfo();

    // 获取所有车主信息
    QList<Driversinfo> allDrivers = ptr->getAllDriversInfo();

    // 获取车主-车辆关联信息
    QList<DriverCar> allDriverCars = ptr->getAllDriverCarInfo();

    QList<Carsinfo> filteredCars; // 存储筛选后的车辆

    // 先找到匹配的车主号
    int searchDno = searchText.toInt();
    bool foundDriver = false;
    for (const auto& driver : allDrivers) {
        if (driver.m_dno == searchDno) {
            foundDriver = true;
            break;
        }
    }

    // 如果找不到车主，显示提示信息并返回
    if (!foundDriver) {
        QMessageBox::information(this, "搜索结果", "未找到该车主信息！");
        return;
    }

    // 根据车主号找到对应的车辆
    for (const auto& dc : allDriverCars) {
        if (dc.m_dno == searchDno) {
            // 找到对应的车辆信息
            for (const auto& car : allCars) {
                if (car.m_cno == dc.m_cno) {
                    filteredCars.append(car);
                }
            }
        }
    }

    // 如果没有找到该车主的车辆，显示提示信息并返回
    if (filteredCars.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "该车主名下没有车辆信息！");
        return;
    }

    // 清空并设置表格
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(8);
    QStringList l;
    l << "车序号" << "车主号" << "车牌号" << "停车位号" << "保安号" << "进入时间" << "出场时间" << "停车类型";
    ui->tableWidget->setHorizontalHeaderLabels(l);
    
    // 设置表格选择模式
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 只显示匹配的行
    ui->tableWidget->setRowCount(filteredCars.size());

    for (int i = 0; i < filteredCars.size(); ++i)
    {
        Carsinfo car = filteredCars.at(i);

        // 查找对应的车主号（这里就是搜索的车主号）
        int dno = searchDno;

        // 查找对应的车主姓名
        QString dname = "未知车主";
        for (const auto& driver : allDrivers) {
            if (driver.m_dno == dno) {
                dname = driver.m_dname;
                break;
            }
        }

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(car.m_cno))); // 车序号
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(dno))); // 车主号
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(car.m_clicense)); // 车牌号
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(car.m_pno))); // 停车位号
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(car.m_username)); // 保安号
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(car.m_cin)); // 进入时间
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(car.m_cout)); // 出场时间

        // 车辆类型转换 1:月卡车, 2:临时车
        QString ctypeStr = (car.m_ctype == 1) ? "月卡车" : "临时车";
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(ctypeStr)); // 停车类型
    }
}

//获取车主的电话
void MainWindow::on_Btn_getPhone_clicked()
{
    // 获取当前选中的行
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择一行！");
        return;
    }

    // 获取选中行的保安号（username）
    QString username = ui->tableWidget->item(currentRow, 4)->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "警告", "该行没有保安信息！");
        return;
    }

    // 查询数据库获取电话号码
    auto ptr = DriverSql::getinstance();
    QString phone = ptr->getSecurityPhone(username);

    if (phone.isEmpty()) {
        QMessageBox::warning(this, "警告", "未找到该保安的电话信息！");
    } else {
        QMessageBox::information(this, "保安电话", 
            QString("保安号：%1\n电话号码：%2")
            .arg(username)
            .arg(phone));
    }
}


void MainWindow::on_Btn_out_clicked()
{
    int i = ui->tableWidget->currentRow();
    if(i >= 0)
    {
        // 获取选中行的车辆信息
        int cno = ui->tableWidget->item(i, 0)->text().toInt();  // 获取车辆编号
        QString clicense = ui->tableWidget->item(i, 2)->text(); // 获取车牌号
        
        // 设置对话框的车辆信息
        m_dlgOut.setCarInfo(cno, clicense);
        
        // 显示出场对话框
        if(m_dlgOut.exec() == QDialog::Accepted)
        {
            // 如果用户确认出场，更新表格显示
            updateTable();
            QMessageBox::information(nullptr, "提示", "车辆出场成功！");
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "警告", "请先选择要出场的车辆！");
    }
}




