#ifndef INVERTEDINDEXNODE_H
#define INVERTEDINDEXNODE_H

#include <QString>
#include <QVector>
#include <QMap>

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

#endif // INVERTEDINDEXNODE_H 