#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSet>
#include <QFileInfo>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QProgressBar>
#include <QStringList>
#include <QDebug>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>
#include <QtCore5Compat/QTextCodec>

// 文档节点结构体，用于存储文档ID、在文档中的位置和上下文
struct DocumentNode {
    int docId;                // 文档ID
    int position;             // 单词在文档中的位置
    QString context;          // 单词的上下文（前后文）
    double weight;            // 文档权重

    DocumentNode(int id = 0, int pos = 0, const QString& ctx = "", double w = 1.0)
        : docId(id), position(pos), context(ctx), weight(w) {}
};

// 倒排索引节点，用于存储关键词与包含该关键词的文档列表
struct InvertedIndexNode {
    QString keyword;                   // 关键词
    QVector<DocumentNode> documents;   // 包含此关键词的文档列表
    
    InvertedIndexNode(const QString& key = "")
        : keyword(key) {}
};

// 词典树节点，用于快速检索关键词
struct TrieNode {
    QMap<QChar, TrieNode*> children;   // 子节点
    bool isEndOfWord;                  // 是否是单词结尾
    int indexId;                       // 对应的倒排索引ID，-1表示不是单词结尾

    TrieNode() : isEndOfWord(false), indexId(-1) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void importFiles();               // 导入文件
    void performSearch();             // 执行搜索
    void displayFileContent();        // 显示文件内容
    void clearSearch();               // 清空搜索

private:
    // UI组件
    QLineEdit* searchBox;             // 搜索框
    QPushButton* searchButton;        // 搜索按钮
    QPushButton* importButton;        // 导入按钮
    QPushButton* clearButton;         // 清空按钮
    QListWidget* resultsList;         // 搜索结果列表
    QTextEdit* fileContentView;       // 文件内容查看器
    QProgressBar* progressBar;        // 进度条
    QLabel* statusLabel;              // 状态标签

    // 数据成员
    QVector<QString> documentPaths;   // 文档路径列表
    QVector<QString> documentContents; // 文档内容列表
    QVector<InvertedIndexNode> invertedIndex; // 倒排索引
    TrieNode* root;                   // 词典树根节点
    
    // 功能方法
    void createUI();                  // 创建用户界面
    QString readFileContent(const QString& filePath); // 读取文件内容
    QStringList tokenize(const QString& content);   // 分词
    void buildInvertedIndex();        // 建立倒排索引
    void insertToTrie(const QString& word, int indexId); // 向词典树中插入单词
    int searchInTrie(const QString& word);          // 在词典树中搜索单词
    QVector<DocumentNode> searchKeyword(const QString& keyword); // 搜索关键词
    void rankResults(QVector<DocumentNode>& results); // 对结果进行排序
    QString extractContext(const QString& content, int position, int contextSize = 50); // 提取上下文
    void clearIndex();                // 清空索引
};

#endif // MAINWINDOW_H
