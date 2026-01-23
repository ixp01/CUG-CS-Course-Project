#include "skiplist.h"

SkipList::SkipList()
    : currentLevel(1), gen(rd()), dis(0, 1)
{
    head = new SkipNode();
}

SkipList::~SkipList()
{
    clear();
    delete head;
}

int SkipList::randomLevel()
{
    int level = 1;
    while (dis(gen) < 0.5 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

void SkipList::insert(const InvertedIndexNode& node)
{
    QVector<SkipNode*> update(MAX_LEVEL, nullptr);
    SkipNode* current = head;
    
    // 从最高层开始查找
    for (int i = currentLevel - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->data.keyword < node.keyword) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    // 如果关键词已存在，更新文档列表
    if (current->forward[0] && current->forward[0]->data.keyword == node.keyword) {
        current->forward[0]->data.documents = node.documents;
        return;
    }
    
    // 生成随机层数
    int newLevel = randomLevel();
    
    // 如果新层数大于当前层数，更新head的指针
    if (newLevel > currentLevel) {
        for (int i = currentLevel; i < newLevel; i++) {
            update[i] = head;
        }
        currentLevel = newLevel;
    }
    
    // 创建新节点
    SkipNode* newNode = new SkipNode(node, newLevel);
    
    // 更新指针
    for (int i = 0; i < newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

InvertedIndexNode* SkipList::find(const QString& keyword)
{
    SkipNode* current = head;
    
    // 从最高层开始查找
    for (int i = currentLevel - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->data.keyword < keyword) {
            current = current->forward[i];
        }
    }
    
    current = current->forward[0];
    
    // 检查是否找到关键词
    if (current && current->data.keyword == keyword) {
        return &(current->data);
    }
    
    return nullptr;
}

void SkipList::clear()
{
    SkipNode* current = head->forward[0];
    while (current) {
        SkipNode* next = current->forward[0];
        delete current;
        current = next;
    }
    
    for (int i = 0; i < MAX_LEVEL; i++) {
        head->forward[i] = nullptr;
    }
    currentLevel = 1;
}

int SkipList::size() const
{
    int count = 0;
    SkipNode* current = head->forward[0];
    while (current) {
        count++;
        current = current->forward[0];
    }
    return count;
} 