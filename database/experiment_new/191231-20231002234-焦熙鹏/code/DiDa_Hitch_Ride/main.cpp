#include "mainwindow.h"
#include"page_login.h"
#include <QApplication>
#include"driversql.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    DriverSql sql;
    return a.exec();
}
