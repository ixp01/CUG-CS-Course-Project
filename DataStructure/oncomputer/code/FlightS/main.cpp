#include "mainwindow.h"
#include "data.h"
#include "logindialog.h"
#include "usermainwindow.h"
#include "adminmainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QScreen>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    a.setApplicationName("航班票务管理系统");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("数据结构课程项目");
    
    // 测试数据库连接
    DatabaseManager& dbManager = DatabaseManager::instance();
    
    // 使用指定的绝对路径
    QString dbPath = "D:/myCollege/CS class/DataStructure/oncomputer/code/FlightS/airticket.db";
    
    qDebug() << "使用数据库文件路径:" << dbPath;
    
    // 检查数据库文件是否存在
    QFileInfo dbFile(dbPath);
    if (!dbFile.exists()) {
        QMessageBox::critical(nullptr, "数据库错误", 
            QString("数据库文件不存在！\n路径: %1").arg(dbPath));
        return -1;
    }
    
    if (!dbManager.connectToDatabase(dbPath)) {
        QMessageBox::critical(nullptr, "数据库错误", 
            "无法连接到数据库！\n请确保 airticket.db 文件存在且可访问。");
        return -1;
    }
    
    qDebug() << "数据库连接成功！";
    
    // 显示登录界面
    LoginDialog loginDialog;
    
    // 设置登录对话框为应用程序中心
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    loginDialog.move(
        screenGeometry.center() - loginDialog.rect().center()
    );
    
    if (loginDialog.exec() == QDialog::Accepted) {
        // 登录成功，根据用户类型显示相应界面
        if (loginDialog.isAdmin()) {
            // 显示管理员界面
            AdminMainWindow* adminWindow = new AdminMainWindow(
                loginDialog.getUserId(), 
                loginDialog.getUsername()
            );
            
            // 设置窗口居中显示
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            adminWindow->move(
                screenGeometry.center() - adminWindow->rect().center()
            );
            
            adminWindow->show();
            
            // 确保管理员窗口关闭时程序退出
            QObject::connect(adminWindow, &QMainWindow::destroyed, &a, &QApplication::quit);
            
            return a.exec();
        } else {
            // 显示用户界面
            UserMainWindow* userWindow = new UserMainWindow(
                loginDialog.getUserId(), 
                loginDialog.getUsername()
            );
            
            // 设置窗口居中显示
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->geometry();
            userWindow->move(
                screenGeometry.center() - userWindow->rect().center()
            );
            
            userWindow->show();
            
            // 确保用户窗口关闭时程序退出
            QObject::connect(userWindow, &QMainWindow::destroyed, &a, &QApplication::quit);
            
            return a.exec();
        }
    }
    
    // 用户取消登录，直接退出
    qDebug() << "用户取消登录，程序退出";
    return 0;
}
