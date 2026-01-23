#ifndef CREATERANDNUMS_H
#define CREATERANDNUMS_H

#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include <vector>
#include <QString>
#include <QDebug>
#include <map>
#include <queue>
#include <string>

// 原始设置
//#define SUMNUM 10000  //随机数的数量
//#define MAXNUM 1024   //随机数范围0---MAXNUM

// 降低规模的新设置
#define SUMNUM 1000    //随机数的数量减少到1000
#define MAXNUM 100     //随机数范围缩小到0-100

#pragma warning(disable:4996)

using namespace std;
using namespace chrono;

// 哈夫曼树节点
struct HuffmanNode {
    int value;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(int v, int f) : value(v), freq(f), left(nullptr), right(nullptr) {}
};

class CreateRandNums
{
public:
    int randseed;
    int* intMSG = NULL;
    QString strMSG = "";
    char binaryCode[SUMNUM][24];
    
    // 哈夫曼编码相关
    std::map<int, std::string> huffmanCodes;
    std::string huffmanDict;

    CreateRandNums() {
        randseed = -1;
        intMSG = new int[SUMNUM];
    }

    //析构函数
    ~CreateRandNums() {
        if (intMSG != NULL){
            delete intMSG;
        }
        cout << "Object is being deleted" << endl;
    }

    void AddRandNums() {
        randseed = randseed + 1;
        srand(randseed);
        for (int i = 0; i < SUMNUM; i++) {
            intMSG[i] = (rand()*RAND_MAX%MAXNUM+rand())%MAXNUM + 0;  //0~MAXNUM
        }
    }

    //将随机数数组转换为字符串，仅供参考，你可以自由修改成你喜欢的格式
    void Transform() {
        strMSG = "";
        for (int i = 0; i < SUMNUM; i++) {
            QString oneMSG = QString::number(intMSG[i]);
            strMSG = strMSG + oneMSG + " ";
        }
    }

    //将随机数数组编码为普通的未压缩二进制数组，用于在客户端编码
    void ToBinaryCode() {
        for (int i = 0; i < SUMNUM; i++) {
            int num = intMSG[i];
            // 11位二进制表示 (0-1024)
            for (int j = 10; j >= 0; j--) {
                binaryCode[i][10-j] = ((num >> j) & 1) ? '1' : '0';
            }
            binaryCode[i][11] = '\0';
        }
    }

    //将普通的未压缩二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
    void BinaryCodeToIntArray() {
        int index = 0;
        int strLen = strMSG.length();
        for (int i = 0; i < SUMNUM && index + 10 < strLen; i++) {
            int value = 0;
            for (int j = 0; j < 11 && index < strLen; j++) {
                value = (value << 1) | (strMSG[index++].toLatin1() - '0');
            }
            intMSG[i] = value;
        }
    }

    //将随机数数组编码为haffman算法编码后的二进制数组，务必同步传输字典，用于在客户端编码
    void ToHaffmanCode() {
        // 清理之前的数据
        huffmanCodes.clear();
        huffmanDict.clear();
        
        // 统计频率
        std::map<int, int> freqMap;
        for (int i = 0; i < SUMNUM; i++) {
            freqMap[intMSG[i]]++;
        }

        // 如果只有一个元素，特殊处理
        if (freqMap.size() == 1) {
            auto it = freqMap.begin();
            huffmanCodes[it->first] = "0";
        } else {
            // 构建哈夫曼树
            auto comp = [](HuffmanNode* a, HuffmanNode* b) { return a->freq > b->freq; };
            std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(comp)> pq(comp);
            
            for (auto& pair : freqMap) {
                pq.push(new HuffmanNode(pair.first, pair.second));
            }
            
            while (pq.size() > 1) {
                HuffmanNode* left = pq.top(); pq.pop();
                HuffmanNode* right = pq.top(); pq.pop();
                
                HuffmanNode* newNode = new HuffmanNode(-1, left->freq + right->freq);
                newNode->left = left;
                newNode->right = right;
                pq.push(newNode);
            }
            
            // 生成编码
            if (!pq.empty()) {
                HuffmanNode* root = pq.top();
                generateCodes(root, "", huffmanCodes);
                deleteTree(root); // 安全删除树
            }
        }
        
        // 存储字典
        huffmanDict = "HUFFMAN_DICT:";
        for (auto& code : huffmanCodes) {
            huffmanDict += std::to_string(code.first) + ":" + code.second + ";";
        }
        huffmanDict += "|";
        
        // 编码数据
        for (int i = 0; i < SUMNUM; i++) {
            std::string code = huffmanCodes[intMSG[i]];
            if (code.length() < 24) {
                strcpy(binaryCode[i], code.c_str());
            } else {
                // 如果编码太长，截断
                strncpy(binaryCode[i], code.c_str(), 23);
                binaryCode[i][23] = '\0';
            }
        }
    }

    //将haffman算法编码后的二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
    void HaffmanCodeToIntArray() {
        // 清理输出数组
        for (int i = 0; i < SUMNUM; i++) {
            intMSG[i] = 0;
        }
        
        // 查找字典结束标记
        int dictEnd = strMSG.indexOf("|");
        if (dictEnd == -1) {
            qDebug() << "未找到哈夫曼字典结束标记";
            return;
        }
        
        // 解析字典部分
        QString dictPart = strMSG.left(dictEnd);
        if (!dictPart.startsWith("HUFFMAN_DICT:")) {
            qDebug() << "哈夫曼字典格式错误";
            return;
        }
        
        // 移除"HUFFMAN_DICT:"前缀
        QString dictData = dictPart.mid(13);
        QStringList dictItems = dictData.split(";", Qt::SkipEmptyParts);
        
        // 构建解码映射
        std::map<std::string, int> decodeMap;
        for (const QString& item : dictItems) {
            if (item.contains(":")) {
                QStringList parts = item.split(":");
                if (parts.size() >= 2) {
                    int number = parts[0].toInt();
                    QString code = parts[1];
                    decodeMap[code.toStdString()] = number;
                }
            }
        }
        
        qDebug() << "解码字典大小:" << decodeMap.size();
        
        // 解码数据部分
        QString dataPart = strMSG.mid(dictEnd + 1);
        std::string currentCode;
        int decodedCount = 0;
        
        for (int i = 0; i < dataPart.length() && decodedCount < SUMNUM; i++) {
            currentCode += dataPart[i].toLatin1();
            auto it = decodeMap.find(currentCode);
            if (it != decodeMap.end()) {
                intMSG[decodedCount++] = it->second;
                currentCode.clear();
            }
        }
        
        qDebug() << "成功解码数字个数:" << decodedCount;
        
        // 如果解码不足，用0填充剩余位置
        for (int i = decodedCount; i < SUMNUM; i++) {
            intMSG[i] = 0;
        }
    }

    //将二进制数组binaryCode转为字符串的函数
    void TransformBinaryCode() {
        QString binaryString;
        // 只有在使用哈夫曼编码时才添加字典
        if (!huffmanDict.empty()) {
            binaryString = QString::fromStdString(huffmanDict);
        }
        
        // 添加二进制编码数据
        for (int i = 0; i < SUMNUM; i++) {
            binaryString += QString(binaryCode[i]);
        }
        strMSG = binaryString;
    }

private:
    // 生成哈夫曼编码的辅助函数
    void generateCodes(HuffmanNode* root, const std::string& code, std::map<int, std::string>& codes) {
        if (!root) return;
        
        if (!root->left && !root->right) {
            codes[root->value] = code.empty() ? "0" : code;
            return;
        }
        
        generateCodes(root->left, code + "0", codes);
        generateCodes(root->right, code + "1", codes);
    }
    
    // 安全删除哈夫曼树
    void deleteTree(HuffmanNode* root) {
        if (root) {
            deleteTree(root->left);
            deleteTree(root->right);
            delete root;
        }
    }
};

#endif // CREATERANDNUMS_H
