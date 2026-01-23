QT += widgets sql

TARGET = FlightS
TEMPLATE = app

# 包含路径
INCLUDEPATH += include src

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/data.cpp \
    src/flightgraph.cpp \
    src/logindialog.cpp \
    src/usermainwindow.cpp \
    src/adminmainwindow.cpp \
    src/addflightdialog.cpp

HEADERS += \
    mainwindow.h \
    include/data.h \
    include/flightgraph.h \
    include/common.h \
    include/logindialog.h \
    include/usermainwindow.h \
    include/adminmainwindow.h \
    include/addflightdialog.h

FORMS += \
    mainwindow.ui \
    ui/login.ui \
    ui/user_main.ui \
    ui/admin_main.ui

# C++17 标准 (Qt 6 推荐)
CONFIG += c++17

# 确保使用正确的编译器标准
QMAKE_CXXFLAGS += -std=c++17

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
