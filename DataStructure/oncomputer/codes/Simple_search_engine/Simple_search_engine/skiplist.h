#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <QVector>
#include <random>
#include "invertedindexnode.h"

class SkipList {
private:
    static const int MAX_LEVEL = 16;  // 最大层数
    
    struct SkipNode {
        InvertedIndexNode data;
        QVector<SkipNode*> forward;
        
        SkipNode(const InvertedIndexNode& node, int level)
            : data(node), forward(level, nullptr) {}
            
        SkipNode(int level = MAX_LEVEL)
            : forward(level, nullptr) {}
    };
    
    SkipNode* head;
    int currentLevel;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    int randomLevel();

public:
    SkipList();
    ~SkipList();
    
    void insert(const InvertedIndexNode& node);
    InvertedIndexNode* find(const QString& keyword);
    void clear();
    
    // 迭代器支持
    class Iterator {
    private:
        SkipNode* current;
    public:
        Iterator(SkipNode* node) : current(node) {}
        bool operator!=(const Iterator& other) const { return current != other.current; }
        Iterator& operator++() { current = current->forward[0]; return *this; }
        InvertedIndexNode& operator*() { return current->data; }
    };
    
    Iterator begin() { return Iterator(head->forward[0]); }
    Iterator end() { return Iterator(nullptr); }
    
    // 获取大小
    int size() const;
};

#endif // SKIPLIST_H 