#include "mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextBrowser>
#include <algorithm>
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
#include <QMutex>
#include <QThread>
#include <QProgressDialog>
#include <QTimer>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), root(new TrieNode), fileLoadWatcher(nullptr)
{
    // 设置初始导入路径为应用程序当前目录
    lastImportPath = QDir::currentPath();
    createUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete root;
    if (fileLoadWatcher) {
        delete fileLoadWatcher;
    }
}

void MainWindow::createUI()
{
    // 创建中央部件和主布局
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 创建标题
    titleLabel = new QLabel("智能文档搜索系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // 创建搜索区域
    searchWidget = new QWidget(this);
    searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setSpacing(10);
    searchLayout->setContentsMargins(0, 0, 0, 0);

    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("请输入要搜索的内容...");
    searchInput->setMinimumWidth(300);

    searchButton = new QPushButton("搜索", this);
    searchButton->setFixedWidth(80);
    clearButton = new QPushButton("清空", this);
    clearButton->setFixedWidth(80);
    importButton = new QPushButton("导入文件", this);
    importButton->setFixedWidth(80);

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(clearButton);
    searchLayout->addWidget(importButton);
    mainLayout->addWidget(searchWidget);

    // 创建内容显示区域
    contentSplitter = new QSplitter(Qt::Vertical, this);
    contentSplitter->setChildrenCollapsible(false);

    // 创建结果显示区域
    resultDisplay = new QTextBrowser(this);
    resultDisplay->setOpenLinks(false);
    resultDisplay->setOpenExternalLinks(false);
    resultDisplay->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::LinksAccessibleByMouse);
    resultDisplay->setStyleSheet("QTextBrowser { background-color: #ffffff; border: 1px solid #cccccc; }");
    resultDisplay->setMinimumHeight(200);

    // 创建预览区域
    previewDisplay = new QTextEdit(this);
    previewDisplay->setReadOnly(true);
    previewDisplay->setPlaceholderText("文档预览将在这里显示");
    previewDisplay->setStyleSheet("QTextEdit { background-color: #ffffff; border: 1px solid #cccccc; }");
    previewDisplay->setMinimumHeight(200);

    contentSplitter->addWidget(resultDisplay);
    contentSplitter->addWidget(previewDisplay);
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 1);
    mainLayout->addWidget(contentSplitter);

    // 创建状态栏
    statusLabel = new QLabel(this);
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setTextVisible(true);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #cccccc; border-radius: 2px; text-align: center; }"
                             "QProgressBar::chunk { background-color: #4CAF50; }");

    QStatusBar* statusBar = new QStatusBar(this);
    statusBar->addWidget(statusLabel, 1);
    statusBar->addPermanentWidget(progressBar);
    setStatusBar(statusBar);

    // 设置初始状态
    searchButton->setEnabled(false);
    clearButton->setEnabled(false);
    statusLabel->setText("欢迎使用智能文档搜索系统");

    // 设置窗口属性
    resize(800, 800);
    setWindowTitle("智能文档搜索系统");
    setStyleSheet("QMainWindow { background-color: #f5f5f5; }"
                 "QPushButton { background-color: #4CAF50; color: white; border: none; padding: 5px; border-radius: 2px; }"
                 "QPushButton:hover { background-color: #45a049; }"
                 "QPushButton:disabled { background-color: #cccccc; }"
                 "QLineEdit { padding: 5px; border: 1px solid #cccccc; border-radius: 2px; }");
}

void MainWindow::setupConnections()
{
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::handleSearchButtonClicked);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::handleClearButtonClicked);
    connect(importButton, &QPushButton::clicked, this, &MainWindow::handleImportButtonClicked);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::handleSearchInputChanged);
}

void MainWindow::importFiles()
{
    // 选择文件夹，使用上次的导入路径
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        "选择文件夹",
        lastImportPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (dirPath.isEmpty()) {
        return;
    }

    // 保存当前选择的路径
    lastImportPath = dirPath;

    // 清理之前的索引
    clearIndex();
    
    // 获取文件夹中的所有文本文件
    QDir dir(dirPath);
    QStringList filters;
    filters << "*.txt";

    // 使用递归方式获取所有txt文件
    QStringList files;
    
    // 显示进度对话框
    QProgressDialog progress("正在扫描文件...", "取消", 0, 0, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.show();

    // 首先计算文件总数
    int totalFiles = 0;
    {
        QDirIterator countIt(dirPath, filters, 
                          QDir::Files | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

        while (countIt.hasNext()) {
            countIt.next();
            totalFiles++;
            if (totalFiles % 100 == 0) {
                progress.setLabelText(QString("已发现 %1 个文件...").arg(totalFiles));
                QApplication::processEvents();
                if (progress.wasCanceled()) {
                    statusLabel->setText("文件扫描已取消");
                    return;
                }
            }
        }
    }

    // 创建新的迭代器来收集文件
    progress.setMaximum(totalFiles);
    progress.setLabelText("正在准备导入文件...");
    
    // 收集文件
    QStringList absolutePaths;
    int currentFile = 0;
    
    QDirIterator it(dirPath, filters, 
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        absolutePaths << filePath;
        
        currentFile++;
        progress.setValue(currentFile);
        progress.setLabelText(QString("正在处理: %1").arg(QFileInfo(filePath).fileName()));
        
        if (progress.wasCanceled()) {
            statusLabel->setText("文件导入已取消");
            return;
        }
    }
    
    if (absolutePaths.isEmpty()) {
        QMessageBox::warning(this, "警告", "所选文件夹中没有找到文本文件");
        return;
    }

    // 开始异步加载文件
    statusLabel->setText(QString("开始导入 %1 个文件...").arg(absolutePaths.size()));
    loadFilesAsync(absolutePaths, dir);
}
//异步加载文件
void MainWindow::loadFilesAsync(const QStringList& fileNames, const QDir& directory)
{
    // 保存当前目录
    currentDirectory = directory;
    selectedFiles = fileNames;

    // 更新状态
    statusLabel->setText("正在加载文件...");
    progressBar->setMaximum(fileNames.size());
    progressBar->setValue(0);
    progressBar->show();

    // 启动计时器
    fileLoadTimer.start();

    // 创建异步任务
    QFuture<QPair<QVector<QString>, QVector<QString>>> future = QtConcurrent::run([fileNames]() {
        QVector<QString> paths;
        QVector<QString> contents;
        paths.reserve(fileNames.size());
        contents.reserve(fileNames.size());

        QTextCodec* codec = QTextCodec::codecForName("GB18030");

        for (const QString& fileName : fileNames) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray data = file.readAll();
                QString content = codec->toUnicode(data);
                if (!content.isEmpty()) {
                    paths.append(fileName);
                    contents.append(content);
                }
                file.close();
            }
        }

        return qMakePair(paths, contents);
    });

    // 设置完成处理器
    if (!fileLoadWatcher) {
        fileLoadWatcher = new QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>(this);
        connect(fileLoadWatcher, &QFutureWatcher<QPair<QVector<QString>, QVector<QString>>>::finished,
                this, &MainWindow::handleFilesLoaded);
    }
    fileLoadWatcher->setFuture(future);
}

//处理加载完成的文件
void MainWindow::handleFilesLoaded()
{
    // 获取加载结果
    auto result = fileLoadWatcher->result();
    documentPaths = result.first;
    documentContents = result.second;

    // 更新状态
    int loadTime = fileLoadTimer.elapsed();
    QString status = QString("已加载 %1 个文件 (用时 %2 秒), 开始构建索引...")
                    .arg(documentContents.size())
                    .arg(loadTime / 1000.0, 0, 'f', 2);
    statusLabel->setText(status);
    progressBar->hide();

    // 如果没有成功加载任何文件，显示错误
    if (documentContents.isEmpty()) {
        QMessageBox::warning(this, "错误", "未能成功加载任何文件！");
        return;
    }

    // 开始构建索引
    optimizedBuildInvertedIndex();
}

//优化构建倒排索引
void MainWindow::optimizedBuildInvertedIndex()
{
    statusLabel->setText("正在构建索引...");
    progressBar->setMaximum(documentContents.size());
    progressBar->setValue(0);
    progressBar->show();

    // 创建临时哈希表来收集词项
    QHash<QString, QVector<QPair<int, QVector<int>>>> tempIndex;

    // 处理每个文档
    for (int docId = 0; docId < documentContents.size(); ++docId) {
        const QString& content = documentContents[docId];
        QStringList tokens = tokenize(content);

        // 记录每个词的位置
        QHash<QString, QVector<int>> wordPositions;
        for (int pos = 0; pos < tokens.size(); ++pos) {
            const QString& token = tokens[pos];
            if (!token.isEmpty()) {
                wordPositions[token].append(pos);
            }
        }

        // 将文档中的词添加到临时索引
        for (auto it = wordPositions.constBegin(); it != wordPositions.constEnd(); ++it) {
            tempIndex[it.key()].append(qMakePair(docId, it.value()));
        }

        // 更新进度
        progressBar->setValue(docId + 1);
        QApplication::processEvents();
    }

    // 将临时索引转换为最终的倒排索引
    invertedIndex.clear();
    invertedIndex.reserve(tempIndex.size());

    for (auto it = tempIndex.constBegin(); it != tempIndex.constEnd(); ++it) {
        InvertedIndexNode indexNode(it.key());
        const auto& docList = it.value();

        // 为每个文档创建DocumentNode
        for (const auto& docInfo : docList) {
            int docId = docInfo.first;
            const QVector<int>& positions = docInfo.second;

            // 为每个位置创建上下文
            for (int pos : positions) {
                QString context = extractContext(documentContents[docId], pos);
                indexNode.documents.append(DocumentNode(docId, pos, context));
            }
        }

        invertedIndex.append(indexNode);
    }

    //优化构建Trie树
    // 构建Trie树
    for (int i = 0; i < invertedIndex.size(); ++i) {
        insertToTrie(invertedIndex[i].keyword, i);
    }

    // 更新状态
    QString status = QString("索引构建完成，共处理 %1 个文档，构建了 %2 个索引项")
                    .arg(documentContents.size())
                    .arg(invertedIndex.size());
    statusLabel->setText(status);
    progressBar->hide();

    // 启用搜索功能
    searchInput->setEnabled(true);
    searchButton->setEnabled(!searchInput->text().isEmpty());
    clearButton->setEnabled(true);
}

void MainWindow::performSearch()
{
    QString keyword = searchInput->text().trimmed();
    if (keyword.isEmpty()) {
        statusLabel->setText("请输入搜索关键词");
        return;
    }

    // 清空之前的搜索结果
    resultDisplay->clear();
    previewDisplay->clear();

    // 断开之前的连接
    disconnect(resultDisplay, &QTextBrowser::anchorClicked, nullptr, nullptr);

    // 在Trie树中查找关键词
    int indexId = searchInTrie(keyword);
    if (indexId == -1) {
        statusLabel->setText("未找到匹配的结果");
        return;
    }

    // 获取搜索结果
    const InvertedIndexNode& node = invertedIndex[indexId];
    QVector<DocumentNode> results = node.documents;

    if (results.isEmpty()) {
        statusLabel->setText("未找到匹配的结果");
        return;
    }

    // 对结果进行排序
    rankResults(results);

    // 显示结果
    QString resultText;
    QSet<int> displayedDocs;

    for (const DocumentNode& doc : results) {
        if (displayedDocs.contains(doc.docId)) {
            continue;
        }

        QFileInfo fileInfo(documentPaths[doc.docId]);
        QString fileName = fileInfo.fileName();

        // 创建可点击的结果项，使用自定义样式
        resultText += QString(
            "<div style='margin-bottom: 10px; padding: 10px; background-color: #f5f5f5; "
            "border: 1px solid #ddd; border-radius: 4px;'>"
            "<a href='doc:%1' style='text-decoration: none; color: #000; display: block;'>"
            "<div style='margin-bottom: 5px;'><b>文件:</b> <span style='color: #0066cc;'>%2</span></div>"
            "<div style='margin-bottom: 5px;'><b>位置:</b> 第 %3 个字符</div>"
            "<div><b>上下文:</b> %4</div>"
            "</a></div>"
        ).arg(doc.docId)
         .arg(fileName)
         .arg(doc.position)
         .arg(doc.context);

        displayedDocs.insert(doc.docId);
    }

    // 设置样式和内容
    QString htmlContent = QString(
        "<html><head><style>"
        "div { font-family: Arial, sans-serif; }"
        "div:hover { background-color: #e9e9e9 !important; cursor: pointer; }"
        "a { outline: none; }"
        "a:hover { color: #000 !important; }"
        "</style></head><body>%1</body></html>"
    ).arg(resultText);

    resultDisplay->setHtml(htmlContent);

    // 连接点击事件
    connect(resultDisplay, &QTextBrowser::anchorClicked, this, [this](const QUrl& url) {
        QString scheme = url.scheme();
        if (scheme == "doc") {
            bool ok;
            int docId = url.path().mid(1).toInt(&ok);  // 移除开头的 ":"
            if (ok) {
                displayFileContent(docId);
            }
        }
    });

    statusLabel->setText(QString("找到 %1 个匹配文档").arg(displayedDocs.size()));
}

void MainWindow::rankResults(QVector<DocumentNode>& results)
{
    // 计算每个文档的权重
    QHash<int, double> docWeights;
    
    for (DocumentNode& doc : results) {
        // 简单的权重计算：基于位置的权重（越靠前权重越高）
        double positionWeight = 1.0 / (doc.position + 1);
        
        // 更新文档权重
        if (!docWeights.contains(doc.docId)) {
            docWeights[doc.docId] = positionWeight;
        } else {
            docWeights[doc.docId] += positionWeight;
        }
        
        doc.weight = positionWeight;
    }
    
    // 对结果进行排序
    std::sort(results.begin(), results.end(), [](const DocumentNode& a, const DocumentNode& b) {
        if (a.docId == b.docId) {
            return a.position < b.position;  // 同一文档内按位置排序
        }
        return a.weight > b.weight;  // 不同文档按权重排序
    });
}

void MainWindow::displayFileContent(int docId)
{
    if (docId >= 0 && docId < documentContents.size()) {
        QString content = documentContents[docId];
        QString keyword = searchInput->text().trimmed();
        
        // 在预览中高亮所有关键词
        QString highlightedContent = content;
        QStringList lines = content.split('\n');
        QString htmlContent = "<html><body style='font-family: Arial, sans-serif;'>";
        
        for (const QString& line : lines) {
            QString processedLine = line;
            int pos = 0;
            while ((pos = processedLine.indexOf(keyword, pos, Qt::CaseInsensitive)) != -1) {
                processedLine.insert(pos + keyword.length(), "</span>");
                processedLine.insert(pos, "<span style='background-color: yellow;'>");
                pos += keyword.length() + QString("<span style='background-color: yellow;'></span>").length();
            }
            htmlContent += "<div style='margin-bottom: 5px;'>" + processedLine + "</div>";
        }
        
        htmlContent += "</body></html>";
        previewDisplay->setHtml(htmlContent);
        previewDisplay->moveCursor(QTextCursor::Start);
        
        // 更新状态栏
        QFileInfo fileInfo(documentPaths[docId]);
        statusLabel->setText(QString("正在预览: %1").arg(fileInfo.fileName()));
    }
}

void MainWindow::handleSearchButtonClicked()
{
    performSearch();
}

void MainWindow::handleClearButtonClicked()
{
    clearSearch();
}

void MainWindow::handleImportButtonClicked()
{
    importFiles();
}

void MainWindow::handleSearchInputChanged(const QString& text)
{
    searchButton->setEnabled(!text.trimmed().isEmpty());
}

void MainWindow::clearSearch()
{
    searchInput->clear();
    resultDisplay->clear();
    previewDisplay->clear();
    statusLabel->setText("搜索已清空");
}

void MainWindow::clearIndex()
{
    // 清理倒排索引
    invertedIndex.clear();

    // 清理Trie树
    delete root;
    root = new TrieNode();

    // 清理文档相关数据
    documentPaths.clear();
    documentContents.clear();
}

//分词
QStringList MainWindow::tokenize(const QString& content)
{
    QStringList tokens;
    tokens.reserve(content.length() / 2);
    
    QString token;
    token.reserve(8);
    
    for (int i = 0; i < content.length(); ++i) {
        const QChar& ch = content.at(i);
        
        // 处理中文字符
        if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) {
            if (!token.isEmpty()) {
                tokens.append(token.toLower());
                token.clear();
            }
            tokens.append(QString(ch));
        }
        // 处理英文和数字
        else if (ch.isLetterOrNumber()) {
            token.append(ch);
        }
        // 处理分隔符
        else if (!token.isEmpty()) {
            tokens.append(token.toLower());
            token.clear();
        }
    }
    
    if (!token.isEmpty()) {
        tokens.append(token.toLower());
    }
    
    return tokens;
}

//提取上下文
QString MainWindow::extractContext(const QString& content, int position, int contextLength)
{
    int start = qMax(0, position - contextLength);
    int end = qMin(content.length(), position + contextLength);
    QString context = content.mid(start, end - start);
    
    // 获取实际的关键词
    QString keyword = searchInput->text().trimmed();
    int keywordLength = keyword.length();
    
    // 计算关键词在上下文中的实际位置
    int highlightPos = position - start;
    
    // 确保高亮位置正确
    if (highlightPos >= 0 && highlightPos + keywordLength <= context.length()) {
        // 插入高亮标记
        context.insert(highlightPos + keywordLength, "</span>");
        context.insert(highlightPos, "<span style='background-color: yellow;'>");
    }
    
    return context;
}

//在Trie树中查找关键词
int MainWindow::searchInTrie(const QString& word)
{
    TrieNode* current = root;
    for (const QChar& ch : word) {
        if (!current->children.contains(ch)) {
            return -1;
        }
        current = current->children[ch];
    }
    return current->indexId;
}

//插入到Trie树
void MainWindow::insertToTrie(const QString& word, int indexId)
{
    TrieNode* current = root;
    for (const QChar& ch : word) {
        if (!current->children.contains(ch)) {
            current->children[ch] = new TrieNode();
        }
        current = current->children[ch];
    }
    current->indexId = indexId;
    current->isEndOfWord = true;
}

//更新进度条
void MainWindow::updateProgressUI(int current, int total)
{
    progressBar->setMaximum(total);
    progressBar->setValue(current);
    QApplication::processEvents();
}


