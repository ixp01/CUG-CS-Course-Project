#include "widget.h"
#include "ui_widget.h"
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <QDebug>
#include <QDialog>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lineEdit->installEventFilter(this);
    ui->tableWidget->installEventFilter(this);
    ui->treeWidget_2->installEventFilter(this);
    ui->widget->setHidden(true);
    ui->tableWidget->setHidden(true);
    ui->widget->setWindowOpacity(0.7);
    ui->treeWidget_2->setHeaderLabel("");
    QStringList list;
    list<<"顺序遍历搜索"<<"搜索二叉树搜索"<<"平衡二叉树搜索"<<"红黑树搜索"<<"字典树搜索";
    ui->comboBox->addItems(list);
    ui->tableWidget->hide();
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setFrameShape(QFrame::NoFrame); //设置无边框
    ui->tableWidget->setShowGrid(false);//设置不显示格子线
    ui->tableWidget->verticalHeader()->setVisible(false);//设置垂直头不可见
    ui->tableWidget->horizontalHeader()->setVisible(false);//设置水平头不可见
    ui->tableWidget->horizontalHeader()->resizeSection(1,300);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("zhangsan"));
    ui->tableWidget->setItem(0,1,new QTableWidgetItem("n.法外狂徒"));
    ui->treeWidget_2->setVisible(0);
    result_C=new QLabel(this);
    result_E=new QLabel(this);
    result_E->move(12,130);result_E->resize(500,40);result_E->setVisible(0);
    result_C->move(12,160);result_C->resize(500,40);result_C->setVisible(0);
    Init();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::BasicSearch(std::string s, std::vector<std::string> &vForEnglish, std::vector<std::string> &vForChinese)
{
    vforpath.clear();
    int l=0,r=v1->size()-1;
    int tmp;
    int x = 0;
    while (l<=r) {
        int mid=(l+r)>>1;
        vforpath.push_back(v1->at(mid));
        x++;
        if (v1->at(mid)>=s) {
            tmp=mid;
            r=mid-1;
        }
        else l=mid+1;
    }

    for (int i=0;i<10 && (i+tmp)<(v1->size());i++) {
        std::string &s1=v1->at(i+tmp);
        if (s==s1.substr(0,s.length())) {
            vForEnglish.push_back(s1);
            vForChinese.push_back(v2->at(i+tmp));
        }
        else break;
    }
    qDebug() << "顺序查找的关键字对比次数：" << x; // 在搜索结束后输出x的值
}

void Widget::BSTSearch(std::string s, std::vector<std::string> &vForEnglish, std::vector<std::string> &vForChinese)
{
    vforpath.clear();
    bst->Find(s,&vForEnglish,&vForChinese,&vforpath);
}

void Widget::AVLSearch(std::string s, std::vector<std::string> &vForEnglish, std::vector<std::string> &vForChinese)
{
    vforpath.clear();
    avl->Find(s,vForEnglish,vForChinese,vforpath);
}

void Widget::RBSearch(std::string s, std::vector<std::string> &vForEnglish, std::vector<std::string> &vForChinese)
{
    vforpath.clear();
    rbt->Find(s, &vForEnglish, &vForChinese, &vforpath);
}

void Widget::TireSearch(std::string s, std::vector<std::string> &vForEnglish, std::vector<std::string> &vForChinese)
{
    vforpath.clear();
   tire->Find(s,&vForEnglish,&vForChinese,&vforpath);
}

void Widget::Init()
{

    std::fstream fin("C:\\Users\\lenovo\\Desktop\\EnWords.csv",std::ios::in);
    std::string s,s1,s2;
    std::getline(fin,s);
    while (!fin.eof())  {
        //qDebug()<<"yes";

        std::getline(fin,s);
        int i=0;
        if (!s.empty()) {
            int t=++i;
            for  (;s[i]!='\"';i++);
            s1=s.substr(t,i-t);
            t=(i+=3);
            for  (;s[i]!='\"';i++);
            s2=s.substr(t,i-t);
            v1->push_back(s1);
            v2->push_back(s2);
           vectorForEnglish.push_back(s1);
           vectorForChinese.push_back(s2);
           //qDebug() << QString::fromStdString(s);
        }
    }
    fin.close();
   // qDebug() << QString::fromStdString(v1->at)
    bst=new BST(*v1,*v2);
    avl=new AVL(v1,v2);
    rbt=new RBT(v1,v2);
    tire=new Tire(v1,v2);
}


void Widget::on_pushButton_2_clicked()
{
    if(ui->widget->isHidden()){
        ui->widget->setHidden(false);
        result_E->setVisible(0);
        result_C->setVisible(0);
    }else{
        ui->widget->setHidden(true);
    }
}
bool Widget::eventFilter(QObject *watched, QEvent *event){
    if(watched==ui->lineEdit){
        if (event->type()==QEvent::FocusIn)
         {
             ui->tableWidget->setHidden(false);
             result_E->setVisible(0);
             result_C->setVisible(0);
         }
         if (event->type()==QEvent::FocusOut)
         {
             ui->tableWidget->setHidden(true);
             result_E->setVisible(1);
             result_C->setVisible(1);
         }
    }
    if(watched==ui->tableWidget){
            if (event->type()==QEvent::FocusIn)
            {
                ui->tableWidget->setHidden(false);
            }
            if (event->type()==QEvent::FocusOut)
            {
                ui->tableWidget->setHidden(true);
            }
        };
    return QWidget::eventFilter(watched,event);
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    SearchModelChoice=index;//改变搜索类型
    return;
}

void Widget::on_lineEdit_textChanged(const QString &arg1)//改变文本框内容
{
    vectorForEnglish.clear();
    vectorForChinese.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setHidden(false);//显示tablewidget
    std::string s;
    if(!(ui->lineEdit->text().isEmpty())){
        s=ui->lineEdit->text().toStdString();//获取lineedit上的字符
///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
        Search(s);//调用搜索函数
///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
        if(vectorForEnglish.size()<=0){
            ui->tableWidget->setHidden(true);
        }
        else{
            int a=vectorForEnglish.size();
            int temp = (a < 10) ? a : 10;//若是不小于等于10则取前十个
            for(int i=0;i<temp;i++)
            {
                QString str = QString(QString::fromStdString(vectorForChinese[i]));
                if (vectorForEnglish[i].length()<=0)
                {
                    if(i==0)
                    {
                        QMessageBox::warning(nullptr, "warning", "Can't find it !", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                    }break;
                }
                else
                {
                    QString str1 = QString(QString::fromStdString(vectorForEnglish[i]));
                    QString str2 = QString(QString::fromStdString(vectorForChinese[i]));
                    ui->tableWidget->setItem(i,0,new QTableWidgetItem(str1));
                    ui->tableWidget->setItem(i,1,new QTableWidgetItem(str2));
                }
            }
        }
    }else{
        if(vectorForEnglish.size()<=0){
            ui->tableWidget->setHidden(true);
        }
    }
    return;
}

void Widget::on_pushButton_clicked()
{
    vectorForEnglish.clear();
    vectorForChinese.clear();
    ui->treeWidget_2->clear();
    ui->tableWidget->hide();
    if(!(ui->lineEdit->text().isEmpty())){
        std::string s;
        QString str1;
        QString str2;
        if(!(ui->lineEdit->text().isEmpty())){
           s=ui->lineEdit->text().toStdString();
///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
           Search(s);
///////////////////////////////////////////////////////Search函数///////////////////////////////////////////////
           //qDebug()<<vectorForEnglish.size();
           if(vectorForEnglish.size()<=0){
               QMessageBox::warning(nullptr, "warning", "Can't find it !", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
           }
           else{

               if(vectorForEnglish[0].empty()){
                   QMessageBox::warning(nullptr, "warning", "Can't find it !", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
               }else{
                  QDialog *dia=new QDialog(this);
                  QLabel  *lab=new QLabel(dia); lab->move(0,0);
                  dia->resize(100,100);
                  QString s;
                  for (auto i:vforpath) {
                      s+=QString::fromStdString(i)+"\n";
                  }
                  lab->setText(s);
                  dia->show();
                  str2 = QString(QString::fromStdString(vectorForChinese[0]));
                  QTreeWidgetItem * item=new QTreeWidgetItem(QStringList()<<ui->lineEdit->text()<<str2);
                  ui->treeWidget_2->addTopLevelItem(item);
                   result_E->setVisible(1);
                   result_C->setVisible(1);
                   result_E->setText(QString::fromStdString(vectorForEnglish[0]));
                   result_C->setText(QString::fromStdString(vectorForChinese[0]));


               };

           }
        }
    }else{
       QMessageBox::warning(nullptr, "warning", "Plz tap a word first!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    return;
};

void Widget::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if(ui->tableWidget->item(row,column)->text().length()>=0){
        ui->lineEdit->setText(ui->tableWidget->item(row,column)->text());
        ui->tableWidget->setHidden(true);
    };
    return;
};
