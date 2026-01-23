#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置全局字体和样式
    QFont font("Microsoft YaHei", 9);
    a.setFont(font);
    
    // Qt6中不需要显式设置编码，QTextCodec已经在core5compat模块中
    // 使用core5compat模块中的QTextCodec
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    MainWindow w;
    w.show();
    return a.exec();
}
