#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include<QVBoxLayout>
#include <QAbstractItemView>
#include "data.h"
#include "flightgraph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserMainWindow; }
QT_END_NAMESPACE

class UserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    UserMainWindow(int userId, const QString& username, QWidget *parent = nullptr);
    ~UserMainWindow();

private slots:
    // 航班查询
    void onSearchFlights();
    void onTransferSearch();
    void onFlightDoubleClicked(int row, int column);
    
    // 票务管理
    void refreshMyTickets();
    void onRefundTicket(int ticketId);
    
    // 预约管理
    void refreshReservations();
    void onCancelReservation(int reservationId);
    
    // 菜单操作
    void onShowProfile();
    void onLogout();
    void onAbout();

private:
    Ui::UserMainWindow *ui;
    
    // 用户信息
    int currentUserId;
    QString currentUsername;
    
    // 数据
    QVector<Flight> currentFlights;
    QVector<Ticket> currentTickets;
    QVector<Reservation> currentReservations;
    
    // 图算法
    FlightGraph flightGraph;
    
    // 私有方法
    void setupUI();
    void setupTableHeaders();
    void loadCities();
    void displayFlights(const QVector<Flight>& flights);
    void displayTickets();
    void displayReservations();
    void showFlightDetails(const Flight& flight);
    bool confirmBooking(const Flight& flight);
    
    // 表格列索引常量
    enum FlightColumn {
        COL_FLIGHT_NUMBER = 0,
        COL_AIRLINE,
        COL_DEPARTURE_CITY,
        COL_ARRIVAL_CITY,
        COL_DEPARTURE_TIME,
        COL_ARRIVAL_TIME,
        COL_AVAILABLE_SEATS,
        COL_PRICE,
        COL_STATUS,
        COL_ACTION
    };
    
    enum TicketColumn {
        COL_TICKET_ID = 0,
        COL_TICKET_FLIGHT,
        COL_TICKET_DEPARTURE,
        COL_TICKET_ARRIVAL,
        COL_TICKET_TIME,
        COL_TICKET_SEAT,
        COL_TICKET_PRICE,
        COL_TICKET_STATUS
    };
    
    enum ReservationColumn {
        COL_RES_ID = 0,
        COL_RES_FLIGHT,
        COL_RES_DEPARTURE,
        COL_RES_ARRIVAL,
        COL_RES_REQUEST_TIME,
        COL_RES_PRIORITY
    };
};

#endif // USERMAINWINDOW_H 
