QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 设置构建目录
DESTDIR = $$PWD
OBJECTS_DIR = $$PWD
MOC_DIR = $$PWD
RCC_DIR = $$PWD
UI_DIR = $$PWD

# 确保使用 MinGW 编译器
QMAKE_CXX = g++
QMAKE_CC = gcc

# 添加包含路径
INCLUDEPATH += $$PWD

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Data.cpp \
    Manage.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Data.h \
    Manage.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
