#include "twidget.h"

TWidget::TWidget(QWidget *parent)
    : QWidget{parent}
{

    initUI();
    connects();
    isDrawing=false;
    //tree =new BinaryTree<int>();
    currentPreIndex = 0;
    currentInIndex = 0;
    currentPostIndex = 0;

}

TWidget::~TWidget()
{
    delete btn_creat;
    delete btn_clear;
    delete btn_print;
    delete btn_findParent;
    delete btn_lowestCommonAncestor;
    delete btn_inputData;
}

void TWidget::initUI()
{
    resize(600, 500);

    // 创建树的按钮
    btn_creat = new QPushButton(this);
    btn_creat->setText("创建树");
    btn_creat->move(400, 60);
    btn_creat->resize(180, 40);

    // 清空树的按钮
    btn_clear = new QPushButton(this);
    btn_clear->setText("清空树");
    btn_clear->move(400, 110);
    btn_clear->resize(180, 40);

    // 查找节点父节点的按钮
    btn_findParent = new QPushButton(this);
    btn_findParent->setText("查找节点父节点");
    btn_findParent->move(400, 210);
    btn_findParent->resize(180, 40);

    // 查找最小公共祖先的按钮
    btn_lowestCommonAncestor = new QPushButton(this);
    btn_lowestCommonAncestor->setText("查找最小公共祖先");
    btn_lowestCommonAncestor->move(400, 260);
    btn_lowestCommonAncestor->resize(180, 40);

    // 输入序列数据的按钮
    btn_inputData = new QPushButton(this);
    btn_inputData->setText("请输入序列");
    btn_inputData->move(400, 10);
    btn_inputData->resize(180, 40);

    // 打印下一步的按钮
    btn_nextStep = new QPushButton(this);
    btn_nextStep->setText("下一步");
    btn_nextStep->move(400, 160);
    btn_nextStep->resize(180, 40);

    // 检查是否能打印二叉树
    btn_check = new QPushButton(this);
    btn_check->setText("检查是否能打印二叉树");
    btn_check->move(400, 310);
    btn_check->resize(180, 40);

    scene = new QGraphicsScene(this);
    treeView = new QGraphicsView(scene, this);
    treeView->setGeometry(10, 0, 370, 480); // 设置视图的位置和大小
}
//设定指针
void TWidget::SetPtr(BinaryTree<int>* x)
{
   tree=x;
}
//清空页面并删掉树
void TWidget::refresh()
{
    m_PreOrder.clear();
    m_InOrder.clear();
    m_PostOrder.clear();

    // 清空 BinaryTree<int>* tree 中的所有数据，并最终让 root 指针置空
    if (tree != nullptr) {
        // 定义一个辅助函数来递归删除所有节点
        deleteTree(tree->root);
        tree->root = nullptr; // 将 root 指针置空
    }
}

void TWidget::deleteTree(TreeNode<int> *node)
{
    if (node != nullptr) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}
//用于生成输入数据的窗口
void TWidget::setDia(QDialog *dia, QLabel *lab1, QPushButton *check, QLineEdit *editPre, QLineEdit *editIn, QLineEdit *editPost, QString s_lab, QString title)
{

    //容器转string类型代码
    auto vectorToString = [](const std::vector<int>& vec) {
        std::string result;
        for (size_t i = 0; i < vec.size(); ++i) {
            result += std::to_string(vec[i]);
            if (i < vec.size() - 1) result += " ";
        }
        return result;
    };

    lab1->resize(400, 30);
    lab1->setText(s_lab);


    check->setText("确定"); check->move(85, 160); check->resize(80, 30);

    editPre->move(0, 50); editPre->resize(250, 20);
    editIn->move(0, 90); editIn->resize(250, 20);
    editPost->move(0, 130); editPost->resize(250, 20);
    dia->setWindowTitle(title); dia->resize(250, 200);

    //加入文字
    if (!m_PreOrder.empty()) {editPre->setText(QString::fromStdString(vectorToString(m_PreOrder)));}
    if (!m_InOrder.empty()) {editIn->setText(QString::fromStdString(vectorToString(m_InOrder)));}
    if (!m_PostOrder.empty()) {editPost->setText(QString::fromStdString(vectorToString(m_PostOrder)));}

    // 创建并初始化三个 QLabel 组件
    QLabel *labelPre = new QLabel(dia);
    QLabel *labelIn = new QLabel(dia);
    QLabel *labelPost = new QLabel(dia);

    // 设置标签的文本
    labelPre->setText("前序序列:");
    labelIn->setText("中序序列:");
    labelPost->setText("后序序列:");

    // 设置标签的位置和大小
    labelPre->move(0, 30);
    labelPre->resize(80, 20);
    labelIn->move(0, 70);
    labelIn->resize(80, 20);
    labelPost->move(0, 110);
    labelPost->resize(80, 20);
    dia->show();
}
//执行按键和槽的连接
void TWidget::connects()
{
    //输入序列数据的按钮
    connect(btn_inputData, &QPushButton::clicked, this, [ = ]() {
        QDialog *dia = new QDialog(this);
        QLabel *lab = new QLabel(dia);//代表父对象是dia
        QPushButton *check = new QPushButton(dia);
        QLineEdit *editPRE = new QLineEdit(dia);
        QLineEdit *editIN = new QLineEdit(dia);
        QLineEdit *editPOST = new QLineEdit(dia);
        setDia(dia, lab, check, editPRE,editIN,editPOST, "请输入需要创建的序列\n（使用空格相隔）", "建立树");

        connect(check, &QPushButton::clicked, dia, [ = ]() {
            std::string s1 = editPRE->text().toStdString();
            std::vector<int> tmp1 = getnum(s1);
            std::string s2 = editIN->text().toStdString();
            std::vector<int> tmp2 = getnum(s2);
            std::string s3 = editPOST->text().toStdString();
            std::vector<int> tmp3 = getnum(s3);
            m_PreOrder.clear();
            m_InOrder.clear();
            m_PostOrder.clear();

            for (std::vector<int>::size_type i = 0; i < tmp1.size(); i++) { m_PreOrder.push_back(tmp1[i]);}
            for (std::vector<int>::size_type i = 0; i < tmp2.size(); i++) { m_InOrder.push_back(tmp2[i]);}
            for (std::vector<int>::size_type i = 0; i < tmp3.size(); i++) {m_PostOrder.push_back(tmp3[i]);}
            //refresh();
            dia->close();
        });
    });
    // 创建树
    connect(btn_inputData, &QPushButton::clicked, this, &TWidget::drawBinaryTree);
    // 清空树
    connect(btn_inputData, &QPushButton::clicked, this, [ = ]() {
        scene->clear(); // 清空QGraphicsScene中的所有项
        refresh();
        isDrawing=false;
    });
    // 打印下一步
    connect(btn_nextStep, &QPushButton::clicked, this, &TWidget::drawNextStep);
    // 查找节点父节点
    connect(btn_inputData, &QPushButton::clicked, this, [ = ]() {

    });
    // 查找最小公共祖先
    connect(btn_inputData, &QPushButton::clicked, this, [ = ]() {

    });
    // 查找检测输入序列的有效性
    connect(btn_check, &QPushButton::clicked, this, [=]() {
        checkSequences(m_PreOrder, m_InOrder, m_PostOrder);
    });
}
//报错的函数
void showError(const QString& message) {
    QMessageBox::warning(nullptr, "错误", message);
}
//用于检测输入序列的有效性
bool TWidget::checkSequences(const std::vector<int> &m_PreOrder, const std::vector<int> &m_InOrder, const std::vector<int> &m_PostOrder)
{

    if (m_PreOrder.empty() && m_InOrder.empty() && m_PostOrder.empty()) {
        showError("没有输入任何序列！");
        return false;
    } else if ((m_PreOrder.empty() && m_InOrder.empty()) || (m_InOrder.empty() && m_PostOrder.empty())|| (m_PreOrder.empty() && m_PostOrder.empty())) {
        showError("需要输入两个序列来构建二叉树！");return false;
    } else if (m_InOrder.empty()) {
        showError("中序序列为空，无法构建二叉树！");return true;
    } else {//前中序列和中后序列
        if (!tree->canBuildTreeFromPreIn(m_PreOrder, m_InOrder) && !tree->canBuildTreeFromInPost(m_InOrder, m_PostOrder)) {
            showError("序列无法构建有效的二叉树！");return false;
        }
        else
        {
            QMessageBox::information(nullptr, "Message", "可以构建二叉树");return true;
        }
        // 如果序列有效，可以在这里添加进一步的处理逻辑
    }
}

void TWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 设置抗锯齿
    // 创建一个QPen对象，设置颜色为黑色，宽度为1，样式为实线
    QPen pen(Qt::black, 2, Qt::SolidLine);
    painter.setPen(pen); // 设置QPainter的笔触
    scene->clear(); // 清空场景中的所有项
    drawTree(scene, tree->root, nullptr); // 从根节点开始绘制
}
// 绘制下一步,即建立二叉树，没递归调用一次，进行一次update
void TWidget::drawNextStep()
{
    if (!isDrawing) return; // 如果没有开始绘制，直接返回

    if (!m_PreOrder.empty()) { // 前序中序构建
        if (currentPreIndex < m_PreOrder.size() && currentInIndex < m_InOrder.size()) {
            // 调用 BinaryTree 的辅助函数来构建下一步
            tree->buildTreeHelperPreIn(m_PreOrder, m_InOrder, currentPreIndex, currentInIndex, m_InOrder.size() - 1, this);
            currentPreIndex++; // 更新前序序列的索引
            currentInIndex++; // 更新中序序列的索引
            this->update(); // 更新视图
        } else {
            isDrawing = false; // 完成绘制
        }
    } else if (!m_InOrder.empty() && !m_PostOrder.empty()) { // 中序后序构建
        if (currentInIndex < m_InOrder.size() && currentPostIndex < m_PostOrder.size()) {
            // 调用 BinaryTree 的辅助函数来构建下一步
            tree->buildTreeHelperInPost(m_InOrder, m_PostOrder, currentInIndex, m_InOrder.size() - 1, currentPostIndex, this);
            currentInIndex++; // 更新中序序列的索引
            currentPostIndex++; // 更新后序序列的索引
            this->update(); // 更新视图
        } else {
            isDrawing = false; // 完成绘制
            currentPreIndex = 0;
            currentInIndex = 0;
            currentPostIndex = 0;
        }
    }
}
// 绘制二叉树
void TWidget::drawBinaryTree()
{
    // if (!checkSequences(m_PreOrder, m_InOrder, m_PostOrder)) {
    //     return;
    // }
    isDrawing=true;
}

void TWidget::drawTree(QGraphicsScene *scene, TreeNode<int> *node, QGraphicsItem *parentItem)
{
    if (!node) return;

    // 创建一个圆圈表示节点
    QGraphicsEllipseItem *circle = scene->addEllipse(0, 0, 20, 20);
    circle->setBrush(QBrush(Qt::white)); // 设置填充颜色
    circle->setPen(QPen(Qt::black, 2)); // 设置边框颜色和宽度

    // 创建一个文本项并设置文本
    QGraphicsSimpleTextItem *textItem = scene->addSimpleText(QString::fromStdString(std::to_string(node->value)));
    textItem->setBrush(QBrush(Qt::black)); // 设置文本颜色

    // 计算文本项的位置，使其位于圆圈中心
    QRectF textRect = textItem->boundingRect();
    textItem->setPos(-circle->rect().width() / 2 + textRect.width() / 2,
                     -circle->rect().height() / 2 + textRect.height() / 2);

    if (parentItem) {
        // 创建一条线连接父节点和子节点
        QGraphicsLineItem *line = scene->addLine(parentItem->pos().x(), parentItem->pos().y(), circle->pos().x(), circle->pos().y());
        line->setPen(QPen(Qt::black, 1)); // 设置线的颜色和宽度
    }

    if (node->left) {
        drawTree(scene, node->left, circle); // 递归绘制左子树
    }
    if (node->right) {
        drawTree(scene, node->right, circle); // 递归绘制右子树
    }
}


