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
#include <QAtomicInt>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), root(new TrieNode())
{
    setWindowTitle("智能文档搜索系统");
    resize(1200, 800);
    setMinimumSize(900, 600);
    
    // 初始化异步处理组件
    fileLoadWatcher = new QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>();
    indexWatcher = new QFutureWatcher<IndexBatch>();
    connect(fileLoadWatcher, &QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>::finished, this, &MainWindow::handleFilesLoaded);
    connect(indexWatcher, &QFutureWatcher<IndexBatch>::finished, this, &MainWindow::handleIndexingFinished);
    connect(this, &MainWindow::progressUpdated, this, &MainWindow::updateProgressUI, Qt::QueuedConnection);
    
    createUI();
    
    // 连接信号和槽
    connect(importButton, &QPushButton::clicked, this, &MainWindow::importFiles);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::performSearch);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearSearch);
    connect(resultsList, &QListWidget::itemClicked, this, &MainWindow::displayFileContent);
}

MainWindow::~MainWindow()
{
    // 如果有正在运行的异步任务，先取消它们
    if (fileLoadWatcher && fileLoadWatcher->isRunning()) {
        fileLoadWatcher->cancel();
        fileLoadWatcher->waitForFinished();
    }
    if (indexWatcher && indexWatcher->isRunning()) {
        indexWatcher->cancel();
        indexWatcher->waitForFinished();
    }
    
    // 清理资源
    clearIndex();
    delete fileLoadWatcher;
    delete indexWatcher;
}

void MainWindow::updateProgressUI(int value, int maximum, const QString& message)
{
    progressBar->setMaximum(maximum);
    progressBar->setValue(value);
    statusLabel->setText(message);
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
    // 打开文件夹选择对话框让用户选择要导入的文件夹
    QString folderPath = QFileDialog::getExistingDirectory(this, "选择包含文本文件的文件夹");
    if (folderPath.isEmpty()) {
        return;  // 用户取消了选择
    }
    
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
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    statusLabel->setText("正在读取文件...");
    
    // 启动计时器
    processTimer.start();
    
    // 异步加载文件
    loadFilesAsync(fileNames, directory);
}

void MainWindow::loadFilesAsync(const QStringList& fileNames, const QDir& directory)
{
    // 将文件路径复制到局部变量以便在并行线程中访问
    QStringList localFileNames = fileNames;
    QString dirPath = directory.absolutePath();
    MainWindow* mainWindowPtr = this; // 在lambda中使用

    // 使用QtConcurrent::run启动异步任务
    QFuture<QPair<QVector<QString>, QVector<QString>>> future = QtConcurrent::run([localFileNames, dirPath, mainWindowPtr]() {
        // 在lambda内部创建原子计数器，确保它在整个异步操作期间有效
        QAtomicInt processedCount(0);
        QVector<QString> paths;
        QVector<QString> contents;
        
        // 预分配存储空间
        paths.reserve(localFileNames.size());
        contents.reserve(localFileNames.size());
        
        // 处理所有文件
        for (const QString& fileName : localFileNames) {
            QString filePath = dirPath + "/" + fileName;
            
            // 读取文件内容
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "无法打开文件:" << filePath;
                continue;
            }
            
            // 读取内容并处理编码
            QByteArray data = file.readAll();
            file.close();
            
            QTextCodec *codec = QTextCodec::codecForName("GB18030");
            QString content;
            if (codec) {
                content = codec->toUnicode(data);
            } else {
                content = QString::fromUtf8(data);
            }
            
            // 将结果添加到向量中
            paths.append(filePath);
            contents.append(content);
            
            // 更新进度
            int current = processedCount.fetchAndAddRelaxed(1) + 1;
            int total = localFileNames.size();
            
            // 使用信号安全地更新UI
            emit mainWindowPtr->progressUpdated(current, total, QString("正在读取文件... %1/%2").arg(current).arg(total));
        }
        
        return qMakePair(paths, contents);
    });
    
    // 设置监视器
    fileLoadWatcher->setFuture(future);
}

void MainWindow::handleFilesLoaded()
{
    // 检查任务是否出错
    if (fileLoadWatcher->isCanceled()) {
        progressBar->setVisible(false);
        statusLabel->setText("文件加载已取消");
        return;
    }
    
    try {
        // 获取异步任务的结果
        QPair<QVector<QString>, QVector<QString>> result = fileLoadWatcher->result();
        
        // 更新成员变量
        documentPaths = result.first;
        documentContents = result.second;
        
        // 所有文件加载完成，开始构建索引
        statusLabel->setText("正在构建索引...");
        progressBar->setValue(0);
        
        // 使用并行方式构建索引
        buildInvertedIndexParallel();
    }
    catch (const std::exception& e) {
        // 处理可能的异常
        progressBar->setVisible(false);
        statusLabel->setText(QString("加载文件时出错: %1").arg(e.what()));
        QMessageBox::critical(this, "错误", QString("加载文件时出错: %1").arg(e.what()));
        return;
    }
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
    QStringList tokens;
    tokens.reserve(content.length() / 2);  // 预分配空间，假设平均每个token长度为2
    
    QString token;
    token.reserve(8);  // 预分配空间
    
    QString chineseToken;
    chineseToken.reserve(8);  // 预分配空间
    
    bool lastWasChinese = false;
    
    for (int i = 0; i < content.length(); ++i) {
        const QChar& ch = content.at(i);
        
        // 检查是否是中文字符
        bool isChinese = (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF);
        bool isPunctOrSpace = ch.isPunct() || ch.isSpace();
        
        if (isChinese) {
            // 如果有非中文token，先保存
            if (!token.isEmpty()) {
                tokens.append(token.toLower());
                token.clear();
            }
            
            // 如果前面有标点或空格，先保存当前的中文词组
            if (isPunctOrSpace && !chineseToken.isEmpty()) {
                tokens.append(chineseToken);
                // 同时添加单个字符，以支持单字搜索
                for (const QChar& c : chineseToken) {
                    tokens.append(QString(c));
                }
                chineseToken.clear();
            }
            
            chineseToken.append(ch);
            lastWasChinese = true;
        } else if (isPunctOrSpace) {
            // 如果是标点或空格，保存之前的token
            if (!token.isEmpty()) {
                tokens.append(token.toLower());
                token.clear();
            }
            if (!chineseToken.isEmpty()) {
                tokens.append(chineseToken);
                // 同时添加单个字符，以支持单字搜索
                for (const QChar& c : chineseToken) {
                    tokens.append(QString(c));
                }
                chineseToken.clear();
            }
            lastWasChinese = false;
        } else {
            // 如果是其他字符（字母或数字）
            if (lastWasChinese && !chineseToken.isEmpty()) {
                tokens.append(chineseToken);
                // 同时添加单个字符，以支持单字搜索
                for (const QChar& c : chineseToken) {
                    tokens.append(QString(c));
                }
                chineseToken.clear();
            }
            
            if (ch.isLetterOrNumber()) {
                token.append(ch);
            }
            lastWasChinese = false;
        }
    }
    
    // 处理最后的token
    if (!token.isEmpty()) {
        tokens.append(token.toLower());
    }
    if (!chineseToken.isEmpty()) {
        tokens.append(chineseToken);
        // 同时添加单个字符，以支持单字搜索
        for (const QChar& c : chineseToken) {
            tokens.append(QString(c));
        }
    }
    
    return tokens;
}

void MainWindow::buildInvertedIndexParallel()
{
    // 清除之前的索引
    invertedIndex.clear();
    
    // 设置进度条范围
    int totalDocuments = documentContents.size();
    progressBar->setRange(0, totalDocuments);
    progressBar->setValue(0);
    
    // 计算批次大小和数量
    const int batchSize = 20;  // 减小批次大小，使进度更新更频繁
    int numBatches = (totalDocuments + batchSize - 1) / batchSize;
    QList<BatchRange> batches;
    
    for (int i = 0; i < numBatches; ++i) {
        int start = i * batchSize;
        int end = qMin((i + 1) * batchSize, totalDocuments);
        batches.append(BatchRange(start, end));
    }
    
    // 创建Future Watcher
    if (indexWatcher) {
        delete indexWatcher;
    }
    indexWatcher = new QFutureWatcher<IndexBatch>();
    
    // 连接信号
    connect(indexWatcher, &QFutureWatcher<IndexBatch>::progressValueChanged,
            this, [this](int value) {
                int processedDocs = value * 20;  // 每个批次20个文档
                processedDocs = qMin(processedDocs, documentContents.size());
                emit progressUpdated(processedDocs, documentContents.size(),
                                  QString("正在构建索引... (%1/%2)")
                                  .arg(processedDocs)
                                  .arg(documentContents.size()));
            });
    
    connect(indexWatcher, &QFutureWatcher<IndexBatch>::finished,
            this, &MainWindow::handleIndexingFinished);
    
    // 启动并行处理
    QFuture<IndexBatch> future = QtConcurrent::mapped(batches,
                                                     [this](const BatchRange& range) {
                                                         return processBatch(range);
                                                     });
    
    indexWatcher->setFuture(future);
}

IndexBatch MainWindow::processBatch(const BatchRange& range)
{
    IndexBatch batch;
    QMap<QString, QVector<QPair<int, int>>> tempWordPositions;
    
    // 计算这个批次中所有文档的总字符数
    qint64 totalChars = 0;
    for (int docId = range.start; docId < range.end; ++docId) {
        totalChars += documentContents[docId].length();
    }
    qint64 processedChars = 0;
    
    // 处理这个批次中的所有文档
    for (int docId = range.start; docId < range.end; ++docId) {
        const QString& content = documentContents[docId];
        QStringList tokens = tokenize(content);
        
        // 记录每个单词在文档中的位置
        for (int pos = 0; pos < tokens.size(); ++pos) {
            const QString& token = tokens[pos];
            tempWordPositions[token].append(qMakePair(docId, pos));
        }
        
        // 更新进度（基于处理的字符数）
        processedChars += content.length();
        double progress = static_cast<double>(processedChars) / totalChars;
        int currentDoc = docId - range.start + 1;
        int totalDocs = range.end - range.start;
        emit progressUpdated(
            docId + 1,
            documentContents.size(),
            QString("正在构建索引... 批次进度: %1% (%2/%3)")
                .arg(qRound(progress * 100))
                .arg(currentDoc)
                .arg(totalDocs)
        );
    }
    
    // 为这个批次构建倒排索引
    int totalWords = tempWordPositions.size();
    int processedWords = 0;
    
    for (auto it = tempWordPositions.begin(); it != tempWordPositions.end(); ++it) {
        const QString& word = it.key();
        const QVector<QPair<int, int>>& positions = it.value();
        
        InvertedIndexNode node(word);
        
        // 为每个文档位置创建文档节点
        for (const auto& pos : positions) {
            int docId = pos.first;
            int position = pos.second;
            
            // 优化上下文提取，减少不必要的分词
            QString context = extractContext(documentContents[docId], position);
            
            // 使用缓存的tokens长度
            double weight = 1.0 * positions.size() / tokenize(documentContents[docId]).size();
            
            node.documents.append(DocumentNode(docId, position, context, weight));
        }
        
        batch.nodes.append(node);
        batch.keywordMap.insert(word, batch.nodes.size() - 1);
        
        // 更新处理进度
        processedWords++;
        double progress = static_cast<double>(processedWords) / totalWords;
        emit progressUpdated(
            range.end,
            documentContents.size(),
            QString("正在构建索引... 处理词条: %1% (%2/%3)")
                .arg(qRound(progress * 100))
                .arg(processedWords)
                .arg(totalWords)
        );
    }
    
    return batch;
}

void MainWindow::handleIndexingFinished()
{
    if (indexWatcher->isCanceled()) {
        progressBar->setVisible(false);
        statusLabel->setText("索引构建已取消");
        return;
    }
    
    try {
        // 获取所有批次的结果
        QList<IndexBatch> results = indexWatcher->future().results();
        
        // 合并结果
        mergeIndexResults(results);
        
        // 计算总用时
        int elapsedMs = processTimer.elapsed();
        
        // 完成
        progressBar->setVisible(false);
        statusLabel->setText(QString("✓ 已成功导入 %1 个文件并建立索引 (用时: %2 秒)")
                           .arg(documentPaths.size())
                           .arg(elapsedMs / 1000.0, 0, 'f', 2));
        
        QMessageBox::information(this, "导入完成",
                               QString("✓ 已成功导入 %1 个文件并建立索引\n系统已准备就绪，可以开始搜索\n\n总耗时: %2 秒")
                               .arg(documentPaths.size())
                               .arg(elapsedMs / 1000.0, 0, 'f', 2));
    }
    catch (const std::exception& e) {
        progressBar->setVisible(false);
        statusLabel->setText(QString("索引构建时出错: %1").arg(e.what()));
        QMessageBox::critical(this, "错误", QString("索引构建时出错: %1").arg(e.what()));
    }
}

void MainWindow::mergeIndexResults(const QList<IndexBatch>& results)
{
    // 第一遍遍历：收集所有唯一的关键词并插入跳表
    for (const IndexBatch& batch : results) {
        for (const InvertedIndexNode& node : batch.nodes) {
            invertedIndex.insert(node);
            // 将关键词添加到Trie树中
            insertToTrie(node.keyword, 0);  // 跳表不需要索引ID
        }
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
    QSet<int> addedDocs;  // 用于去重
    
    // 对搜索关键词进行分词
    QStringList searchTokens = tokenize(keyword);
    
    // 搜索每个分词结果
    for (const QString& token : searchTokens) {
        // 在跳表中查找关键词
        InvertedIndexNode* node = invertedIndex.find(token);
        if (node) {
            // 添加未重复的文档
            for (const DocumentNode& doc : node->documents) {
                if (!addedDocs.contains(doc.docId)) {
                    results.append(doc);
                    addedDocs.insert(doc.docId);
                }
            }
        }
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


