QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dlg_adddriver.cpp \
    dlg_out.cpp \
    driversql.cpp \
    main.cpp \
    mainwindow.cpp \
    page_login.cpp \
    page_register.cpp

HEADERS += \
    dlg_adddriver.h \
    dlg_out.h \
    driversql.h \
    mainwindow.h \
    page_login.h \
    page_register.h

FORMS += \
    dlg_adddriver.ui \
    dlg_out.ui \
    mainwindow.ui \
    page_login.ui \
    page_register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
