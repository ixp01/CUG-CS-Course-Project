#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include "Manage.h"

class SimplePurchaseTest : public QWidget {
    Q_OBJECT
    
public:
    SimplePurchaseTest(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupData();
    }
    
private slots:
    void onPurchaseClicked() {
        QString passenger = passengerEdit->text();
        if (passenger.isEmpty()) {
            QMessageBox::warning(this, "警告", "请输入乘客姓名！");
            return;
        }
        
        QString flightID = flightCombo->currentData().toString();
        if (flightID.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择航班！");
            return;
        }
        
        qDebug() << "尝试购票：" << passenger << "航班：" << flightID;
        
        try {
            // 使用简单的购票方法
            Ticket* ticket = ticketManager->purchaseTicket(flightID, passenger);
            if (ticket) {
                QMessageBox::information(this, "购票成功", 
                    QString("乘客 %1 成功购买航班 %2 的机票\n票号：%3")
                    .arg(passenger)
                    .arg(flightID)
                    .arg(QString::fromStdString(ticket->ticketID)));
                
                // 更新航班下拉框
                updateFlightCombo();
                passengerEdit->clear();
            } else {
                QMessageBox::warning(this, "购票失败", "购票失败，可能没有余票！");
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "错误", QString("购票时发生错误：%1").arg(e.what()));
        } catch (...) {
            QMessageBox::critical(this, "错误", "购票时发生未知错误！");
        }
    }
    
private:
    void setupUI() {
        setWindowTitle("简单购票测试");
        setFixedSize(400, 200);
        
        QVBoxLayout* layout = new QVBoxLayout(this);
        
        // 航班选择
        QHBoxLayout* flightLayout = new QHBoxLayout();
        flightLayout->addWidget(new QLabel("选择航班："));
        flightCombo = new QComboBox();
        flightLayout->addWidget(flightCombo);
        layout->addLayout(flightLayout);
        
        // 乘客姓名
        QHBoxLayout* passengerLayout = new QHBoxLayout();
        passengerLayout->addWidget(new QLabel("乘客姓名："));
        passengerEdit = new QLineEdit();
        passengerLayout->addWidget(passengerEdit);
        layout->addLayout(passengerLayout);
        
        // 购票按钮
        QPushButton* purchaseBtn = new QPushButton("购票");
        connect(purchaseBtn, &QPushButton::clicked, this, &SimplePurchaseTest::onPurchaseClicked);
        layout->addWidget(purchaseBtn);
    }
    
    void setupData() {
        // 创建管理器
        flightManager = new FlightManager(this);
        ticketManager = new TicketManager(flightManager, this);
        
        // 添加测试航班
        Flight flight1("CA1234", "中国国航", 
                      QDateTime::currentDateTime().addDays(1),
                      QDateTime::currentDateTime().addDays(1).addSecs(7200),
                      "北京", "上海", 100, 800.0);
        
        Flight flight2("MU5678", "东方航空",
                      QDateTime::currentDateTime().addDays(2),
                      QDateTime::currentDateTime().addDays(2).addSecs(6600),
                      "上海", "广州", 150, 600.0);
        
        flightManager->addFlight(flight1);
        flightManager->addFlight(flight2);
        
        updateFlightCombo();
    }
    
    void updateFlightCombo() {
        flightCombo->clear();
        const QList<Flight*>& flights = flightManager->getAllFlights();
        for (Flight* flight : flights) {
            QString text = QString("%1 %2→%3 %4元 (余票:%5)")
                .arg(flight->getFlightID())
                .arg(flight->getFromCity())
                .arg(flight->getToCity())
                .arg(flight->price)
                .arg(flight->availableSeats);
            flightCombo->addItem(text, flight->getFlightID());
        }
    }
    
private:
    QComboBox* flightCombo;
    QLineEdit* passengerEdit;
    FlightManager* flightManager;
    TicketManager* ticketManager;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimplePurchaseTest test;
    test.show();
    
    return app.exec();
}

#include "simple_purchase_test.moc" 