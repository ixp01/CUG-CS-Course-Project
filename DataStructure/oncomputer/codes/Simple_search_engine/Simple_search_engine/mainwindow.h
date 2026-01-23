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
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QPair>
#include "invertedindexnode.h"
#include "skiplist.h"

// 在InvertedIndexNode结构体后添加新的结构体
struct IndexBatch {
    QVector<InvertedIndexNode> nodes;
    QMap<QString, int> keywordMap;
};

struct BatchRange {
    int start;
    int end;
    BatchRange(int s, int e) : start(s), end(e) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void progressUpdated(int value, int maximum, const QString& message);

private slots:
    void importFiles();               // 导入文件
    void performSearch();             // 执行搜索
    void displayFileContent();        // 显示文件内容
    void clearSearch();               // 清空搜索
    void updateProgressUI(int value, int maximum, const QString& message); // 更新进度UI
    void handleFilesLoaded();         // 处理文件加载完成事件
    void handleIndexingFinished();    // 处理索引完成的槽

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
    SkipList invertedIndex;           // 使用跳表存储倒排索引
    TrieNode* root;                   // 词典树根节点
    
    // 异步处理成员
    QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>* fileLoadWatcher;  // 文件加载完成监视器
    QFutureWatcher<IndexBatch>* indexWatcher;  // 索引构建监视器
    QElapsedTimer processTimer;       // 处理时间计时器
    
    // 功能方法
    void createUI();                  // 创建用户界面
    QString readFileContent(const QString& filePath); // 读取文件内容
    QStringList tokenize(const QString& content);   // 分词方法
    void buildInvertedIndex();        // 建立倒排索引
    void loadFilesAsync(const QStringList& fileNames, const QDir& directory); // 异步加载文件
    void insertToTrie(const QString& word, int indexId); // 向词典树中插入单词
    int searchInTrie(const QString& word);          // 在词典树中搜索单词
    QVector<DocumentNode> searchKeyword(const QString& keyword); // 搜索关键词
    void rankResults(QVector<DocumentNode>& results); // 对结果进行排序
    QString extractContext(const QString& content, int position, int contextSize = 50); // 提取上下文
    void clearIndex();                // 清空索引
    void buildInvertedIndexParallel();  // 并行构建索引
    IndexBatch processBatch(const BatchRange& range);  // 处理单个批次
    void mergeIndexResults(const QList<IndexBatch>& results);  // 合并索引结果
    

};

#endif // MAINWINDOW_H
