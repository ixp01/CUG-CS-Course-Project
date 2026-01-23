#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <iostream>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItemModel>
#include "bst.h"
#include "avl.h"
#include "rbt.h"
#include "tire.h"
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    std::vector<std::string> vectorForEnglish;
    std::vector<std::string> vectorForChinese;
    std::vector<std::string> *v1=new std::vector<std::string>(),*v2=new std::vector<std::string>();
    std::vector<std::string> vforpath;
    bool eventFilter(QObject *watched, QEvent *event);
    QStandardItemModel *standItemModel;
    QStandardItemModel *standItemModel2;
    QLabel *result_E,*result_C;
    ~Widget();
    int SearchModelChoice=0;


///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
    void Init();
    BST *bst;
    AVL *avl;
    RBT *rbt;
    Tire *tire;
    void BasicSearch(std::string s,std::vector<std::string>& vForEnglish,std::vector<std::string>& vForChinese);
    void BSTSearch(std::string s,std::vector<std::string>& vForEnglish,std::vector<std::string>& vForChinese);
    void AVLSearch(std::string s,std::vector<std::string>& vForEnglish,std::vector<std::string>& vForChinese);
    void RBSearch(std::string s,std::vector<std::string>& vForEnglish,std::vector<std::string>& vForChinese);
    void TireSearch(std::string s,std::vector<std::string>& vForEnglish,std::vector<std::string>& vForChinese);
    void Search(std::string s){
        switch(SearchModelChoice){
            case 0:
                BasicSearch(s,vectorForEnglish,vectorForChinese);
                break;
            case 1:
                BSTSearch(s,vectorForEnglish,vectorForChinese);
                break;
            case 2:
                AVLSearch(s,vectorForEnglish,vectorForChinese);
                break;
            case 3:
                RBSearch(s,vectorForEnglish,vectorForChinese);
                break;
            case 4:
                //TireSearch(s,vectorForEnglish,vectorForChinese);
                break;
            default:
                break;
        };
    }
///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
private slots:
    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
