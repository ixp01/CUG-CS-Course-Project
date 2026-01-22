#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTreeWidget>
#include<QKeyEvent>
#include<QFile>
#include<QCoreApplication>
#include<QString>
#include<QMessageBox>

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
    l<<"地大顺风车系统";
    QTreeWidgetItem *pf=new QTreeWidgetItem(ui->treeWidget,l);
    ui->treeWidget->addTopLevelItem(pf);

    l.clear();
    l<<"车主界面";
    QTreeWidgetItem *p1=new QTreeWidgetItem(pf,l);

    l.clear();
    l<<"乘客界面";
    QTreeWidgetItem *p2=new QTreeWidgetItem(pf,l);
    ui->treeWidget->addTopLevelItem(pf);
    pf->addChild(p1);
    pf->addChild(p2);

    ui->treeWidget->expandAll();
    ui->stackedWidget->setCurrentIndex(0);

    m_ptrDriverSql=DriverSql::getinstance();

    // connect(&m_dlgLogin, &Page_Login::sendLoginName, this, &MainWindow::onLoginNameReceived);


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

//这个应该为添加按钮
void MainWindow::on_Btn_save_clicked()
{
    m_dlgAddDriver.setType(true);
    m_dlgAddDriver.exec();
    updateTable();
}


void MainWindow::on_Btn_clear_clicked()
{
    m_ptrDriverSql->clearDriverTable();
    updateTable();
}


void MainWindow::updateTable()
{
ui->tableWidget->clear();
 ui->tableWidget->setColumnCount(13);
QStringList l;
 auto cnt =m_ptrDriverSql->getDriverCnt();
l<<"序号"<<"ID号"<<"姓名"<<"车颜色"<<"车牌号"<<"剩余车位"<<"车位位置"<<"车位价格"<<"出发时间小时"<<"出发时间分钟"<<"出发点"<<"目的地"<<"手机号码";
ui->tableWidget->setHorizontalHeaderLabels(l);
//只选中行
ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
ui->tableWidget->setRowCount(cnt);


QList<DriverInfo> drivers =m_ptrDriverSql->getPageStu(0,cnt);

for(int i =0;i<drivers.size();i++)
{
    ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString ::number(i)));
ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(drivers[i].m_id)));
ui->tableWidget->setItem(i,2,new QTableWidgetItem(drivers[i].m_name));
ui->tableWidget->setItem(i,3,new QTableWidgetItem(drivers[i].m_carColor));
ui->tableWidget->setItem(i,4,new QTableWidgetItem(drivers[i].m_licenseNum));
ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(drivers[i].m_seatNum)));
ui->tableWidget->setItem(i,6,new QTableWidgetItem(drivers[i].m_seatPos));
ui->tableWidget->setItem(i,7,new QTableWidgetItem(QString::number(drivers[i].m_seatPrice)));
ui->tableWidget->setItem(i,8,new QTableWidgetItem(QString::number(drivers[i].m_departTimeHour)));
ui->tableWidget->setItem(i,9,new QTableWidgetItem(QString::number(drivers[i].m_departTimeMin)));
ui->tableWidget->setItem(i,10,new QTableWidgetItem(drivers[i].m_departPlace));
ui->tableWidget->setItem(i,11,new QTableWidgetItem(drivers[i].m_arrivePlace));
ui->tableWidget->setItem(i,12,new QTableWidgetItem(drivers[i].m_phone));
}

}


void MainWindow::on_Btn_delete_clicked()
{
    int i=ui->tableWidget->currentRow();
    if(i>=0)
    {
        int id=  ui->tableWidget->item(i,1)->text().toInt();
        m_ptrDriverSql->delDriver(id);
      updateTable();
      QMessageBox::information(nullptr,"信息","删除成功");

    }

}


void MainWindow::on_Btn_update_clicked()
{
    DriverInfo info;
    int i=ui->tableWidget->currentRow();
    if(i>=0)
    {
        info.m_id=  ui->tableWidget->item(i,1)->text().toUInt();
        info.m_name=  ui->tableWidget->item(i,2)->text();
        info.m_carColor=  ui->tableWidget->item(i,3)->text();
        info.m_licenseNum=  ui->tableWidget->item(i,4)->text();
        info.m_seatNum=  ui->tableWidget->item(i,5)->text().toUInt();
        info.m_seatPos=  ui->tableWidget->item(i,6)->text();
        info.m_seatPrice=  ui->tableWidget->item(i,7)->text().toUInt();
        info.m_departTimeHour=  ui->tableWidget->item(i,8)->text().toUInt();
        info.m_departTimeMin=  ui->tableWidget->item(i,9)->text().toUInt();
        info.m_departPlace=  ui->tableWidget->item(i,10)->text();
        info.m_arrivePlace=  ui->tableWidget->item(i,11)->text();
        info.m_phone=  ui->tableWidget->item(i,12)->text();
        m_dlgAddDriver.setType(false,info);

        m_dlgAddDriver.exec();

    }
    updateTable();
}


void MainWindow::on_Btn_search_clicked()
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(13);
    QStringList l;
    auto cnt =m_ptrDriverSql->getDriverCnt();
    l<<"序号"<<"ID号"<<"姓名"<<"车颜色"<<"车牌号"<<"剩余车位"<<"车位位置"<<"车位价格"<<"出发时间小时"<<"出发时间分钟"<<"出发点"<<"目的地"<<"手机号码";
    ui->tableWidget->setHorizontalHeaderLabels(l);
    //只选中行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QList<DriverInfo> drivers =m_ptrDriverSql->getPageStu(0,cnt);
    qint8 nFilter=ui->line_search->text().toInt();
    if(nFilter == 0 && ui->line_search->text().isEmpty())
    {
        QMessageBox::information(nullptr,"警告","时间筛选为空");
        updateTable();
        return;
    }
    int index=0;
    for(int i =0;i<drivers.size();i++)
    {
        if(!(drivers[i].m_departTimeHour==nFilter))
        {
            continue;
        }

        ui->tableWidget->setItem(index,0,new QTableWidgetItem(QString ::number(index+1)));
        ui->tableWidget->setItem(index,1,new QTableWidgetItem(QString::number(drivers[i].m_id)));
        ui->tableWidget->setItem(index,2,new QTableWidgetItem(drivers[i].m_name));
        ui->tableWidget->setItem(index,3,new QTableWidgetItem(drivers[i].m_carColor));
        ui->tableWidget->setItem(index,4,new QTableWidgetItem(drivers[i].m_licenseNum));
        ui->tableWidget->setItem(index,5,new QTableWidgetItem(QString::number(drivers[i].m_seatNum)));
        ui->tableWidget->setItem(index,6,new QTableWidgetItem(drivers[i].m_seatPos));
        ui->tableWidget->setItem(index,7,new QTableWidgetItem(QString::number(drivers[i].m_seatPrice)));
        ui->tableWidget->setItem(index,8,new QTableWidgetItem(QString::number(drivers[i].m_departTimeHour)));
        ui->tableWidget->setItem(index,9,new QTableWidgetItem(QString::number(drivers[i].m_departTimeMin)));
        ui->tableWidget->setItem(index,10,new QTableWidgetItem(drivers[i].m_departPlace));
        ui->tableWidget->setItem(index,11,new QTableWidgetItem(drivers[i].m_arrivePlace));
        ui->tableWidget->setItem(index,12,new QTableWidgetItem(drivers[i].m_phone));
        index++;
    }
    ui->tableWidget->setRowCount(index);
}
//无用
void MainWindow::onLoginNameReceived(const QString &username)
{
    Q_UNUSED(username);
}
