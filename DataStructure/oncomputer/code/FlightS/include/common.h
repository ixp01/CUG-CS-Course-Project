#ifndef COMMON_H
#define COMMON_H

#include <QString>

namespace Constants {
    // 数据库文件路径
    const QString DATABASE_PATH = "airticket.db";
    
    // 航班状态
    const QString FLIGHT_SCHEDULED = "Scheduled";
    const QString FLIGHT_DELAYED = "Delayed";
    const QString FLIGHT_CANCELLED = "Cancelled";
    const QString FLIGHT_COMPLETED = "Completed";
    
    // 票务状态
    const QString TICKET_AVAILABLE = "Available";
    const QString TICKET_BOOKED = "booked";
    const QString TICKET_RESERVED = "Reserved";
    const QString TICKET_CANCELLED = "cancelled";
    
    // 用户类型
    const QString USER_TYPE_NORMAL = "Normal";
    const QString USER_TYPE_VIP = "VIP";
    
    // 预约优先级
    const int PRIORITY_NORMAL = 0;
    const int PRIORITY_VIP = 1;
    const int PRIORITY_URGENT = 2;
    
    // 系统限制
    const int MAX_TRANSFER_STOPS = 2;
    const int MIN_TRANSFER_TIME = 60; // 分钟
}

// 购票结果枚举
enum class TicketResult {
    Success,
    Failed,
    NoSeats,
    AddedToQueue,
    FlightCancelled,
    InvalidFlight,
    UserNotFound
};

// 通知类型枚举
enum class NotificationType {
    FlightDelay,
    FlightCancellation,
    FlightGateChange,
    TicketConfirmation,
    ReservationAlert,
    AlternativeRecommendation
};

#endif // COMMON_H 