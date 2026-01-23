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
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QProgressBar>
#include <QStatusBar>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QtConcurrent>

// 文档节点结构体
struct DocumentNode {
    int docId;          // 文档ID
    int position;       // 词在文档中的位置
    QString context;    // 上下文内容
    double weight;      // 文档权重

    DocumentNode(int id = -1, int pos = -1, const QString& ctx = QString(), double w = 1.0)
        : docId(id), position(pos), context(ctx), weight(w) {}
};

// 倒排索引节点
struct InvertedIndexNode {
    QString keyword;                    // 关键词
    QVector<DocumentNode> documents;    // 包含该关键词的文档列表

    InvertedIndexNode(const QString& word = QString())
        : keyword(word) {}
};

// Trie树节点
struct TrieNode {
    QHash<QChar, TrieNode*> children;  // 子节点
    bool isEndOfWord;                  // 是否是单词结尾
    int indexId;                       // 对应的倒排索引ID

    TrieNode() : isEndOfWord(false), indexId(-1) {}
    ~TrieNode() {
        qDeleteAll(children);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // UI组件
    QWidget* centralWidget;        // 中央部件
    QVBoxLayout* mainLayout;       // 主布局
    QLabel* titleLabel;           // 标题标签
    
    // 搜索区域组件
    QWidget* searchWidget;        // 搜索区域容器
    QHBoxLayout* searchLayout;    // 搜索区域布局
    QLineEdit* searchInput;       // 搜索输入框
    QPushButton* searchButton;    // 搜索按钮
    QPushButton* clearButton;     // 清空按钮
    QPushButton* importButton;    // 导入按钮

    // 内容显示区域
    QSplitter* contentSplitter;   // 内容分割器
    QTextBrowser* resultDisplay;  // 结果显示区域
    QTextEdit* previewDisplay;    // 预览区域

    // 状态栏组件
    QLabel* statusLabel;          // 状态标签
    QProgressBar* progressBar;    // 进度条

    // 文件处理相关
    QString lastImportPath;       // 上次导入路径
    QStringList selectedFiles;    // 选中的文件列表
    QDir currentDirectory;        // 当前目录
    QVector<QString> documentPaths;    // 文档路径列表
    QVector<QString> documentContents; // 文档内容列表
    QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>* fileLoadWatcher;
    QElapsedTimer fileLoadTimer;

    // 索引结构
    QVector<InvertedIndexNode> invertedIndex;  // 倒排索引
    TrieNode* root;                            // Trie树根节点

    // 核心功能函数
    void createUI();                          // 创建UI
    void setupConnections();                  // 设置信号槽连接
    void importFiles();                       // 导入文件
    void loadFilesAsync(const QStringList& fileNames, const QDir& directory);  // 异步加载文件
    void handleFilesLoaded();                // 处理文件加载完成
    void buildInvertedIndex();               // 构建倒排索引
    void optimizedBuildInvertedIndex();      // 优化的索引构建
    void performSearch();                    // 执行搜索
    void clearSearch();                      // 清空搜索
    void clearIndex();                       // 清空索引
    void displayFileContent(int docId);      // 显示文件内容

    // 辅助函数
    QStringList tokenize(const QString& content);  // 分词
    void insertToTrie(const QString& word, int indexId);  // 插入Trie树
    QString extractContext(const QString& content, int position, int contextLength = 50);  // 提取上下文
    int searchInTrie(const QString& word);   // 在Trie树中搜索
    void rankResults(QVector<DocumentNode>& results);  // 结果排序
    void updateProgressUI(int current, int total);     // 更新进度UI

private slots:
    void handleSearchButtonClicked();
    void handleClearButtonClicked();
    void handleImportButtonClicked();
    void handleSearchInputChanged(const QString& text);
};

#endif // MAINWINDOW_H
