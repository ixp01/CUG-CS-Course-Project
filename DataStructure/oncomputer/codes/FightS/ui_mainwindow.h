/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionImport;
    QAction *actionSave;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QToolBar *toolBar;
    QTabWidget *tabWidget;
    QWidget *flightManageTab;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *flightTable;
    QHBoxLayout *horizontalLayout;
    QPushButton *addFlightButton;
    QPushButton *deleteFlightButton;
    QPushButton *importButton;
    QPushButton *refreshFlightButton;
    QPushButton *changeFlightStatusButton;
    QWidget *ticketTab;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *flightComboBox;
    QLabel *label_2;
    QLineEdit *passengerNameEdit;
    QLabel *label_3;
    QLineEdit *ticketIDEdit;
    QLabel *label_4;
    QComboBox *searchTypeCombo;
    QLabel *label_5;
    QLineEdit *searchEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *purchaseButton;
    QPushButton *refundButton;
    QPushButton *reserveButton;
    QPushButton *searchButton;
    QPushButton *processReserveButton;
    QPushButton *refreshTicketButton;
    QTableWidget *ticketTable;
    QWidget *searchTab;
    QVBoxLayout *verticalLayout_4;
    QFormLayout *formLayout_2;
    QLabel *label_31;
    QComboBox *fromCityCombo;
    QLabel *label_41;
    QComboBox *toCityCombo;
    QLabel *label_51;
    QDateTimeEdit *startDateEdit;
    QLabel *label_6;
    QDateTimeEdit *endDateEdit;
    QPushButton *searchButton1;
    QTableWidget *resultTable;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *sortByPriceButton;
    QPushButton *sortByTimeButton;
    QPushButton *refreshSearchButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1200, 800);
        actionImport = new QAction(MainWindow);
        actionImport->setObjectName("actionImport");
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        toolBar = new QToolBar(centralwidget);
        toolBar->setObjectName("toolBar");
        toolBar->setMovable(false);

        verticalLayout->addWidget(toolBar);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        flightManageTab = new QWidget();
        flightManageTab->setObjectName("flightManageTab");
        verticalLayout_2 = new QVBoxLayout(flightManageTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        flightTable = new QTableWidget(flightManageTab);
        if (flightTable->columnCount() < 10)
            flightTable->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        flightTable->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        flightTable->setObjectName("flightTable");

        verticalLayout_2->addWidget(flightTable);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        addFlightButton = new QPushButton(flightManageTab);
        addFlightButton->setObjectName("addFlightButton");

        horizontalLayout->addWidget(addFlightButton);

        deleteFlightButton = new QPushButton(flightManageTab);
        deleteFlightButton->setObjectName("deleteFlightButton");

        horizontalLayout->addWidget(deleteFlightButton);

        importButton = new QPushButton(flightManageTab);
        importButton->setObjectName("importButton");

        horizontalLayout->addWidget(importButton);

        refreshFlightButton = new QPushButton(flightManageTab);
        refreshFlightButton->setObjectName("refreshFlightButton");

        horizontalLayout->addWidget(refreshFlightButton);

        changeFlightStatusButton = new QPushButton(flightManageTab);
        changeFlightStatusButton->setObjectName("changeFlightStatusButton");

        horizontalLayout->addWidget(changeFlightStatusButton);


        verticalLayout_2->addLayout(horizontalLayout);

        tabWidget->addTab(flightManageTab, QString());
        ticketTab = new QWidget();
        ticketTab->setObjectName("ticketTab");
        verticalLayout_3 = new QVBoxLayout(ticketTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(ticketTab);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        flightComboBox = new QComboBox(ticketTab);
        flightComboBox->setObjectName("flightComboBox");

        formLayout->setWidget(0, QFormLayout::FieldRole, flightComboBox);

        label_2 = new QLabel(ticketTab);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        passengerNameEdit = new QLineEdit(ticketTab);
        passengerNameEdit->setObjectName("passengerNameEdit");

        formLayout->setWidget(1, QFormLayout::FieldRole, passengerNameEdit);

        label_3 = new QLabel(ticketTab);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        ticketIDEdit = new QLineEdit(ticketTab);
        ticketIDEdit->setObjectName("ticketIDEdit");

        formLayout->setWidget(2, QFormLayout::FieldRole, ticketIDEdit);

        label_4 = new QLabel(ticketTab);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        searchTypeCombo = new QComboBox(ticketTab);
        searchTypeCombo->setObjectName("searchTypeCombo");

        formLayout->setWidget(3, QFormLayout::FieldRole, searchTypeCombo);

        label_5 = new QLabel(ticketTab);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        searchEdit = new QLineEdit(ticketTab);
        searchEdit->setObjectName("searchEdit");

        formLayout->setWidget(4, QFormLayout::FieldRole, searchEdit);


        verticalLayout_3->addLayout(formLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        purchaseButton = new QPushButton(ticketTab);
        purchaseButton->setObjectName("purchaseButton");

        horizontalLayout_2->addWidget(purchaseButton);

        refundButton = new QPushButton(ticketTab);
        refundButton->setObjectName("refundButton");

        horizontalLayout_2->addWidget(refundButton);

        reserveButton = new QPushButton(ticketTab);
        reserveButton->setObjectName("reserveButton");

        horizontalLayout_2->addWidget(reserveButton);

        searchButton = new QPushButton(ticketTab);
        searchButton->setObjectName("searchButton");

        horizontalLayout_2->addWidget(searchButton);

        processReserveButton = new QPushButton(ticketTab);
        processReserveButton->setObjectName("processReserveButton");

        horizontalLayout_2->addWidget(processReserveButton);

        refreshTicketButton = new QPushButton(ticketTab);
        refreshTicketButton->setObjectName("refreshTicketButton");

        horizontalLayout_2->addWidget(refreshTicketButton);


        verticalLayout_3->addLayout(horizontalLayout_2);

        ticketTable = new QTableWidget(ticketTab);
        if (ticketTable->columnCount() < 6)
            ticketTable->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(2, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(3, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(4, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        ticketTable->setHorizontalHeaderItem(5, __qtablewidgetitem15);
        ticketTable->setObjectName("ticketTable");
        ticketTable->setColumnCount(6);

        verticalLayout_3->addWidget(ticketTable);

        tabWidget->addTab(ticketTab, QString());
        searchTab = new QWidget();
        searchTab->setObjectName("searchTab");
        verticalLayout_4 = new QVBoxLayout(searchTab);
        verticalLayout_4->setObjectName("verticalLayout_4");
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        label_31 = new QLabel(searchTab);
        label_31->setObjectName("label_31");

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_31);

        fromCityCombo = new QComboBox(searchTab);
        fromCityCombo->setObjectName("fromCityCombo");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, fromCityCombo);

        label_41 = new QLabel(searchTab);
        label_41->setObjectName("label_41");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_41);

        toCityCombo = new QComboBox(searchTab);
        toCityCombo->setObjectName("toCityCombo");

        formLayout_2->setWidget(1, QFormLayout::FieldRole, toCityCombo);

        label_51 = new QLabel(searchTab);
        label_51->setObjectName("label_51");

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_51);

        startDateEdit = new QDateTimeEdit(searchTab);
        startDateEdit->setObjectName("startDateEdit");
        startDateEdit->setDate(QDate(2025, 1, 1));
        startDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, startDateEdit);

        label_6 = new QLabel(searchTab);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_6);

        endDateEdit = new QDateTimeEdit(searchTab);
        endDateEdit->setObjectName("endDateEdit");
        endDateEdit->setDate(QDate(2025, 1, 1));
        endDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, endDateEdit);


        verticalLayout_4->addLayout(formLayout_2);

        searchButton1 = new QPushButton(searchTab);
        searchButton1->setObjectName("searchButton1");

        verticalLayout_4->addWidget(searchButton1);

        resultTable = new QTableWidget(searchTab);
        if (resultTable->columnCount() < 10)
            resultTable->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(1, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(2, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(3, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(4, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(5, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(6, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(7, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(8, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(9, __qtablewidgetitem25);
        resultTable->setObjectName("resultTable");

        verticalLayout_4->addWidget(resultTable);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        sortByPriceButton = new QPushButton(searchTab);
        sortByPriceButton->setObjectName("sortByPriceButton");

        horizontalLayout_3->addWidget(sortByPriceButton);

        sortByTimeButton = new QPushButton(searchTab);
        sortByTimeButton->setObjectName("sortByTimeButton");

        horizontalLayout_3->addWidget(sortByTimeButton);

        refreshSearchButton = new QPushButton(searchTab);
        refreshSearchButton->setObjectName("refreshSearchButton");

        horizontalLayout_3->addWidget(refreshSearchButton);


        verticalLayout_4->addLayout(horizontalLayout_3);

        tabWidget->addTab(searchTab, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1200, 18));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        toolBar->addAction(actionImport);
        toolBar->addAction(actionSave);
        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionImport);
        menuFile->addAction(actionSave);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\210\252\347\217\255\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        actionImport->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\205\245\346\225\260\346\215\256", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        QTableWidgetItem *___qtablewidgetitem = flightTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\350\210\252\347\217\255\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = flightTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\350\210\252\347\251\272\345\205\254\345\217\270", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = flightTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = flightTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = flightTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = flightTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = flightTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "\344\273\267\346\240\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = flightTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "\346\200\273\345\272\247\344\275\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = flightTable->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "\344\275\231\347\245\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = flightTable->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        addFlightButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\350\210\252\347\217\255", nullptr));
        deleteFlightButton->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\350\210\252\347\217\255", nullptr));
        importButton->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\205\245\350\210\252\347\217\255", nullptr));
        refreshFlightButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        changeFlightStatusButton->setText(QCoreApplication::translate("MainWindow", "\344\277\256\346\224\271\347\212\266\346\200\201", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(flightManageTab), QCoreApplication::translate("MainWindow", "\350\210\252\347\217\255\347\256\241\347\220\206", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\350\210\252\347\217\255\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\344\271\230\345\256\242\345\247\223\345\220\215\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\245\250\345\217\267\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242\347\261\273\345\236\213\357\274\232", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242\345\206\205\345\256\271\357\274\232", nullptr));
        purchaseButton->setText(QCoreApplication::translate("MainWindow", "\350\264\255\347\245\250", nullptr));
        refundButton->setText(QCoreApplication::translate("MainWindow", "\351\200\200\347\245\250", nullptr));
        reserveButton->setText(QCoreApplication::translate("MainWindow", "\351\242\204\347\272\246", nullptr));
        searchButton->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242", nullptr));
        processReserveButton->setText(QCoreApplication::translate("MainWindow", "\345\244\204\347\220\206\351\242\204\347\272\246", nullptr));
        refreshTicketButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = ticketTable->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "\347\245\250\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = ticketTable->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "\350\210\252\347\217\255\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = ticketTable->horizontalHeaderItem(2);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "\344\271\230\345\256\242\345\247\223\345\220\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = ticketTable->horizontalHeaderItem(3);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = ticketTable->horizontalHeaderItem(4);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = ticketTable->horizontalHeaderItem(5);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "\350\264\255\347\245\250\346\227\266\351\227\264", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(ticketTab), QCoreApplication::translate("MainWindow", "\347\245\250\345\212\241\346\223\215\344\275\234", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\345\237\216\345\270\202\357\274\232", nullptr));
        label_41->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\345\237\216\345\270\202\357\274\232", nullptr));
        label_51->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\227\266\351\227\264\357\274\232", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\347\273\223\346\235\237\346\227\266\351\227\264\357\274\232", nullptr));
        searchButton1->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = resultTable->horizontalHeaderItem(0);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "\350\210\252\347\217\255\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = resultTable->horizontalHeaderItem(1);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "\350\210\252\347\251\272\345\205\254\345\217\270", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = resultTable->horizontalHeaderItem(2);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = resultTable->horizontalHeaderItem(3);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\345\237\216\345\270\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = resultTable->horizontalHeaderItem(4);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "\345\207\272\345\217\221\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = resultTable->horizontalHeaderItem(5);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "\345\210\260\350\276\276\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = resultTable->horizontalHeaderItem(6);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "\344\273\267\346\240\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = resultTable->horizontalHeaderItem(7);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "\346\200\273\345\272\247\344\275\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = resultTable->horizontalHeaderItem(8);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "\344\275\231\347\245\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = resultTable->horizontalHeaderItem(9);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        sortByPriceButton->setText(QCoreApplication::translate("MainWindow", "\346\214\211\344\273\267\346\240\274\346\216\222\345\272\217", nullptr));
        sortByTimeButton->setText(QCoreApplication::translate("MainWindow", "\346\214\211\346\227\266\351\227\264\346\216\222\345\272\217", nullptr));
        refreshSearchButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(searchTab), QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
