#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QtMath>
#include <algorithm>
#include <QtCore5Compat/QTextCodec>
#include <QtConcurrent>
#include <QIcon>
#include <QFont>
#include <QStyle>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QStyleOption>
#include <QPainter>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), root(new TrieNode())
{
    setWindowTitle("智能文档搜索系统");
    resize(1200, 800);
    setMinimumSize(900, 600);
    createUI();
    
    // 连接信号和槽
    connect(importButton, &QPushButton::clicked, this, &MainWindow::importFiles);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::performSearch);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearSearch);
    connect(resultsList, &QListWidget::itemClicked, this, &MainWindow::displayFileContent);
}

MainWindow::~MainWindow()
{
    clearIndex();
}

void MainWindow::createUI()
{
    // 设置整体样式
    QString styleSheet = "QMainWindow { background-color: #f5f5f5; }"
                         "QLineEdit { padding: 8px; border-radius: 4px; border: 1px solid #ddd; }"
                         "QPushButton { background-color: #4a86e8; color: white; padding: 8px 16px; "
                         "border-radius: 4px; border: none; font-weight: bold; }"
                         "QPushButton:hover { background-color: #3a76d8; }"
                         "QListWidget { border-radius: 4px; border: 1px solid #ddd; background-color: white; }"
                         "QTextEdit { border-radius: 4px; border: 1px solid #ddd; background-color: white; }";
    this->setStyleSheet(styleSheet);
    
    // 创建中央窗口部件
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 添加标题标签
    QLabel* titleLabel = new QLabel("智能文档搜索系统");
    QFont titleFont("Microsoft YaHei", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #333; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // 创建顶部搜索区域 - 使用卡片式设计
    QWidget* searchCard = new QWidget();
    searchCard->setStyleSheet("background-color: white; border-radius: 8px; padding: 15px; border: 1px solid #eaeaea;");
    QVBoxLayout* cardLayout = new QVBoxLayout(searchCard);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    
    // 搜索区域顶部布局
    QHBoxLayout* topSearchLayout = new QHBoxLayout();
    importButton = new QPushButton("导入文件");
    importButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    importButton->setMinimumWidth(120);
    
    topSearchLayout->addWidget(importButton);
    topSearchLayout->addStretch();
    cardLayout->addLayout(topSearchLayout);
    
    // 搜索框和按钮布局
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("请输入要搜索的内容...");
    searchBox->setMinimumHeight(40);
    QFont searchFont("Microsoft YaHei", 10);
    searchBox->setFont(searchFont);
    
    searchButton = new QPushButton("搜索");
    searchButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    searchButton->setMinimumHeight(40);
    searchButton->setMinimumWidth(100);
    
    clearButton = new QPushButton("清空");
    clearButton->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    clearButton->setMinimumHeight(40);
    clearButton->setMinimumWidth(100);
    clearButton->setStyleSheet("background-color: #f44336; color: white;");
    
    searchLayout->addWidget(searchBox, 1);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(clearButton);
    
    cardLayout->addLayout(searchLayout);
    mainLayout->addWidget(searchCard);
    
    // 创建状态标签和进度条区域
    QWidget* statusCard = new QWidget();
    statusCard->setStyleSheet("background-color: white; border-radius: 8px; padding: 10px; border: 1px solid #eaeaea;");
    QHBoxLayout* statusLayout = new QHBoxLayout(statusCard);
    statusLayout->setContentsMargins(10, 10, 10, 10);
    
    statusLabel = new QLabel("欢迎使用智能文档搜索系统");
    statusLabel->setFont(searchFont);
    progressBar = new QProgressBar();
    progressBar->setTextVisible(true);
    progressBar->setVisible(false);
    progressBar->setMinimumWidth(200);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #ddd; border-radius: 3px; text-align: center; }"
                              "QProgressBar::chunk { background-color: #4a86e8; }");
    
    statusLayout->addWidget(statusLabel, 1);
    statusLayout->addWidget(progressBar);
    
    mainLayout->addWidget(statusCard);
    
    // 创建内容区域
    QWidget* contentCard = new QWidget();
    contentCard->setStyleSheet("background-color: white; border-radius: 8px; padding: 15px; border: 1px solid #eaeaea;");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentCard);
    
    // 结果标题
    QLabel* resultsTitle = new QLabel("搜索结果");
    resultsTitle->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    resultsTitle->setStyleSheet("color: #333; padding-bottom: 5px; border-bottom: 1px solid #eaeaea;");
    contentLayout->addWidget(resultsTitle);
    
    // 创建分割器
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->setHandleWidth(8);
    splitter->setStyleSheet("QSplitter::handle { background-color: #eaeaea; }");
    
    // 创建搜索结果列表
    resultsList = new QListWidget();
    resultsList->setAlternatingRowColors(true);
    resultsList->setStyleSheet("QListWidget::item { padding: 8px; border-bottom: 1px solid #eee; }"
                              "QListWidget::item:selected { background-color: #e3f2fd; color: #1565c0; }");
    resultsList->setFont(QFont("Microsoft YaHei", 9));
    resultsList->setIconSize(QSize(16, 16));
    
    // 创建文件内容查看器
    QWidget* previewContainer = new QWidget();
    QVBoxLayout* previewLayout = new QVBoxLayout(previewContainer);
    
    QLabel* previewTitle = new QLabel("文档预览");
    previewTitle->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    previewTitle->setStyleSheet("color: #333; padding-bottom: 5px; border-bottom: 1px solid #eaeaea;");
    previewLayout->addWidget(previewTitle);
    
    fileContentView = new QTextEdit();
    fileContentView->setReadOnly(true);
    fileContentView->setFont(QFont("Microsoft YaHei", 9));
    fileContentView->setStyleSheet("padding: 10px;");
    previewLayout->addWidget(fileContentView);
    
    splitter->addWidget(resultsList);
    splitter->addWidget(previewContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    
    contentLayout->addWidget(splitter);
    mainLayout->addWidget(contentCard, 1);
    
    // 连接回车键搜索
    connect(searchBox, &QLineEdit::returnPressed, this, &MainWindow::performSearch);
}

void MainWindow::importFiles()
{
    // 读取指定文件夹中的所有文本文件
    QString folderPath = "D:/myCollege/CS class/DataStructure/oncomputer/codes/Simple_search_engine/Simple_search_engine/C3-Art";
    QDir directory(folderPath);
    
    if (!directory.exists()) {
        QMessageBox::warning(this, "错误", "指定的文件夹不存在！");
        return;
    }
    
    // 获取文件夹中的所有文本文件
    QStringList fileNames = directory.entryList(QStringList() << "*.txt", QDir::Files);
    
    if (fileNames.isEmpty()) {
        QMessageBox::warning(this, "错误", "文件夹中没有找到文本文件！");
        return;
    }
    
    // 清除之前的数据
    clearIndex();
    
    // 显示进度条
    progressBar->setVisible(true);
    progressBar->setRange(0, fileNames.size() * 2); // 读取文件和建立索引两个阶段
    progressBar->setValue(0);
    
    // 读取所有文件
    int count = 0;
    foreach (const QString &fileName, fileNames) {
        QString filePath = directory.absoluteFilePath(fileName);
        documentPaths.append(filePath);
        
        // 读取文件内容
        QString content = readFileContent(filePath);
        documentContents.append(content);
        
        // 更新进度条
        progressBar->setValue(++count);
        QApplication::processEvents();
    }
    
    // 建立倒排索引
    statusLabel->setText("正在建立索引...");
    QApplication::processEvents();
    
    buildInvertedIndex();
    
    // 完成
    progressBar->setVisible(false);
    statusLabel->setText(QString("✓ 已成功导入 %1 个文件并建立索引").arg(documentPaths.size()));
    
    QMessageBox::information(this, "导入完成", 
                            QString("✓ 已成功导入 %1 个文件并建立索引\n系统已准备就绪，可以开始搜索").arg(documentPaths.size()));
}

QString MainWindow::readFileContent(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filePath;
        return QString();
    }
    
    // Qt6方式读取文件内容
    QByteArray data = file.readAll();
    file.close();
    
    // 在Qt6中使用core5compat模块中的QTextCodec处理中文编码
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    if (codec) {
        return codec->toUnicode(data);
    } else {
        // 如果GB18030编码不可用，使用UTF-8编码
        return QString::fromUtf8(data);
    }
}

QStringList MainWindow::tokenize(const QString& content)
{
    // 简易分词，使用空格、标点等作为分隔符
    // 对于中文文本，这里将每个中文字符视为一个词语
    QStringList tokens;
    QString token;
    
    for (const QChar& ch : content) {
        // 处理中文字符和其他字符
        if (ch.isLetterOrNumber()) {
            token.append(ch);
        } else {
            if (!token.isEmpty()) {
                tokens.append(token.toLower()); // 转换为小写以忽略大小写
                token.clear();
            }
            
            // Qt6中的脚本检测方式
            // 对于中文字符，每个字符作为一个词语
            if (ch.script() == QChar::Script_Han) {
                tokens.append(QString(ch));
            }
        }
    }
    
    // 处理最后一个token
    if (!token.isEmpty()) {
        tokens.append(token.toLower());
    }
    
    return tokens;
}

void MainWindow::buildInvertedIndex()
{
    // 清除之前的索引
    invertedIndex.clear();
    
    QMap<QString, int> keywordMap; // 用于跟踪关键词的索引ID
    
    // 对每个文档进行处理
    for (int docId = 0; docId < documentContents.size(); ++docId) {
        const QString& content = documentContents[docId];
        QStringList tokens = tokenize(content);
        
        // 用于记录在当前文档中单词的位置
        QMap<QString, QVector<int>> wordPositions;
        
        // 记录每个单词在文档中的位置
        for (int pos = 0; pos < tokens.size(); ++pos) {
            const QString& token = tokens[pos];
            wordPositions[token].append(pos);
        }
        
        // 更新倒排索引
        for (auto it = wordPositions.begin(); it != wordPositions.end(); ++it) {
            const QString& word = it.key();
            const QVector<int>& positions = it.value();
            
            // 查找或创建索引条目
            int indexId;
            auto mapIt = keywordMap.find(word);
            if (mapIt == keywordMap.end()) {
                // 创建新的索引条目
                indexId = invertedIndex.size();
                keywordMap[word] = indexId;
                invertedIndex.append(InvertedIndexNode(word));
                
                // 将关键词添加到Trie树中
                insertToTrie(word, indexId);
            } else {
                indexId = mapIt.value();
            }
            
            // 对于每个位置，添加文档节点并提取上下文
            for (int position : positions) {
                QString context = extractContext(content, position);
                
                // 计算文档权重，简单地使用单词在文档中出现的频率
                double weight = 1.0 * positions.size() / tokens.size();
                
                // 添加文档节点
                invertedIndex[indexId].documents.append(DocumentNode(docId, position, context, weight));
            }
        }
        
        // 更新进度条
        progressBar->setValue(documentContents.size() + docId + 1);
        QApplication::processEvents();
    }
}

void MainWindow::insertToTrie(const QString& word, int indexId)
{
    TrieNode* current = root;
    
    for (const QChar& ch : word) {
        if (!current->children.contains(ch)) {
            current->children[ch] = new TrieNode();
        }
        current = current->children[ch];
    }
    
    current->isEndOfWord = true;
    current->indexId = indexId;
}

int MainWindow::searchInTrie(const QString& word)
{
    TrieNode* current = root;
    
    for (const QChar& ch : word) {
        if (!current->children.contains(ch)) {
            return -1; // 单词不存在
        }
        current = current->children[ch];
    }
    
    if (current->isEndOfWord) {
        return current->indexId;
    }
    
    return -1; // 单词不是完整的单词
}

QVector<DocumentNode> MainWindow::searchKeyword(const QString& keyword)
{
    QVector<DocumentNode> results;
    
    // 转换为小写
    QString lowerKeyword = keyword.toLower();
    
    // 在Trie中搜索关键词
    int indexId = searchInTrie(lowerKeyword);
    
    if (indexId != -1) {
        // 找到了关键词
        results = invertedIndex[indexId].documents;
    }
    
    return results;
}

void MainWindow::performSearch()
{
    // 获取搜索关键词
    QString keyword = searchBox->text().trimmed();
    
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入要搜索的内容！");
        return;
    }
    
    // 清空之前的搜索结果
    resultsList->clear();
    fileContentView->clear();
    
    // 搜索关键词
    QVector<DocumentNode> results = searchKeyword(keyword);
    
    if (results.isEmpty()) {
        statusLabel->setText("⚠️ 未找到匹配的结果");
        resultsList->clear();
        QListWidgetItem* noResultItem = new QListWidgetItem();
        noResultItem->setText("<div style='padding: 20px; text-align:center; color:#666;'>"
                              "<div style='font-size: 24px; margin-bottom: 10px;'>😕</div>"
                              "<div>未找到匹配的结果，请尝试其他关键词</div>"
                              "</div>");
        noResultItem->setFlags(Qt::NoItemFlags);
        resultsList->addItem(noResultItem);
        return;
    }
    
    // 对结果进行排序
    rankResults(results);
    
    // 显示搜索结果
    QSet<int> displayedDocs; // 用于跟踪已显示的文档，避免重复
    
    for (const DocumentNode& node : results) {
        // 已经显示过这个文档，跳过
        if (displayedDocs.contains(node.docId)) {
            continue;
        }
        
        QFileInfo fileInfo(documentPaths[node.docId]);
        QString fileName = fileInfo.fileName();
        
        // 创建列表项
        QListWidgetItem* item = new QListWidgetItem();
        
        // 创建更丰富的文本格式
        QString itemText = QString("<div style='margin: 5px 0;'>"
                                   "<span style='font-weight:bold; color:#1565c0;'>%1</span>"
                                   "<div style='margin-top:3px; color:#555;'>%2</div>"
                                   "</div>")
                         .arg(fileName)
                         .arg(QString(node.context).replace('\n', ' '));
        
        item->setText(itemText);
        item->setData(Qt::UserRole, node.docId);
        item->setData(Qt::UserRole + 1, node.position);
        
        // 设置图标
        item->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
        resultsList->addItem(item);
        
        // 记录已显示的文档
        displayedDocs.insert(node.docId);
        
        // 限制显示结果数量
        if (displayedDocs.size() >= 100) {
            break;
        }
    }
    
    statusLabel->setText(QString("🔍 找到 %1 个匹配文档").arg(displayedDocs.size()));
}

void MainWindow::rankResults(QVector<DocumentNode>& results)
{
    // 对搜索结果进行排序
    // 这里使用简单的权重排序，可以扩展为更复杂的PageRank算法或相似度算法
    std::sort(results.begin(), results.end(), [](const DocumentNode& a, const DocumentNode& b) {
        return a.weight > b.weight; // 按权重降序排序
    });
}

QString MainWindow::extractContext(const QString& content, int position, int contextSize)
{
    // 从文档内容中提取关键词的上下文
    QStringList tokens = tokenize(content);
    
    // 确保位置有效
    if (position < 0 || position >= tokens.size()) {
        return QString();
    }
    
    // 确定上下文的起始和结束位置
    int start = qMax(0, position - contextSize / 2);
    int end = qMin(tokens.size() - 1, position + contextSize / 2);
    
    // 构建上下文字符串
    QString context;
    for (int i = start; i <= end; ++i) {
        if (i == position) {
            // 高亮显示关键词
            context += "<b style='color:red;'>" + tokens[i] + "</b> ";
        } else {
            context += tokens[i] + " ";
        }
    }
    
    return context.trimmed() + "...";
}

void MainWindow::displayFileContent()
{
    QListWidgetItem* item = resultsList->currentItem();
    if (!item) {
        return;
    }
    
    int docId = item->data(Qt::UserRole).toInt();
    int position = item->data(Qt::UserRole + 1).toInt();
    
    if (docId < 0 || docId >= documentContents.size()) {
        return;
    }
    
    // 获取文件内容和文件名
    QString content = documentContents[docId];
    QFileInfo fileInfo(documentPaths[docId]);
    QString fileName = fileInfo.fileName();
    
    // 显示文件内容，并高亮显示关键词
    QStringList tokens = tokenize(content);
    QString displayContent = QString("<h3>%1</h3><hr>").arg(fileName);
    
    // 在文本中高亮显示关键词
    for (int i = 0; i < tokens.size(); ++i) {
        if (i == position) {
            // 高亮显示关键词
            displayContent += "<span style='background-color:yellow;'>" + tokens[i] + "</span> ";
        } else {
            displayContent += tokens[i] + " ";
        }
    }
    
    fileContentView->setHtml(displayContent);
}

void MainWindow::clearSearch()
{
    searchBox->clear();
    resultsList->clear();
    fileContentView->clear();
    statusLabel->setText("🔄 搜索已清空");
    
    // 添加欢迎信息到文件内容区域
    fileContentView->setHtml("<div style='padding: 20px; text-align:center;'>"
                           "<h2 style='color:#4a86e8; margin-bottom: 20px;'>智能文档搜索系统</h2>"
                           "<p style='color:#555; font-size: 14px;'>请在上方输入框中输入搜索关键词，点击搜索按钮开始检索文档。</p>"
                           "<p style='color:#555; font-size: 14px; margin-top: 10px;'>如需导入文档，请点击'导入按钮'按钮。</p>"
                           "</div>");
}

void MainWindow::clearIndex()
{
    // 清除Trie树
    if (root) {
        // 递归删除所有节点
        std::function<void(TrieNode*)> deleteNode = [&](TrieNode* node) {
            if (!node) return;
            
            for (auto it = node->children.begin(); it != node->children.end(); ++it) {
                deleteNode(it.value());
            }
            
            delete node;
        };
        
        for (auto it = root->children.begin(); it != root->children.end(); ++it) {
            deleteNode(it.value());
        }
        
        root->children.clear();
    }
    
    // 清除其他数据
    documentPaths.clear();
    documentContents.clear();
    invertedIndex.clear();
}


