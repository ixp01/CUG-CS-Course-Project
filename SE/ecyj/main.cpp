#include "mainwindow.h"
#include "databasehelper.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 初始化数据库
    if (!DatabaseHelper::instance()->initDatabase()) {
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败!");
        return -1;
    }
    
    // 插入测试详细数据（如果表为空）
    DatabaseHelper::instance()->insertTestDetailsData();
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
