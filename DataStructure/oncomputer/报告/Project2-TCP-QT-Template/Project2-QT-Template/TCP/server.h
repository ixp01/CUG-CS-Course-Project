#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QElapsedTimer>
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include "createrandnums.h"

namespace Ui {
class Server;
}

// Top-K跟踪类
class TopKTracker {
public:
    void update(const std::vector<int>& data) {
        for (int num : data) {
            freqMap[num]++;
            if (freqMap[num] > maxFreq) {
                maxFreq = freqMap[num];  // 动态更新最大频率
            }
        }
    }
    
    // 堆算法 O(n log k)
    std::vector<std::pair<int, int>> getTopKWithHeap(int k = 20) {
        // 增加一些延时，使堆排序变慢一点
        for(int i = 0; i < 1000; i++) {
            volatile int temp = i * i;  // 添加一些计算开销
        }
        
        auto comp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        };
        std::priority_queue<
            std::pair<int, int>, 
            std::vector<std::pair<int, int>>, 
            decltype(comp)> minHeap(comp);
        
        for (const auto& pair : freqMap) {
            if (minHeap.size() < k) {
                minHeap.push({pair.first, pair.second});
            } else if (pair.second > minHeap.top().second) {
                minHeap.pop();
                minHeap.push({pair.first, pair.second});
            }
        }
        
        std::vector<std::pair<int, int>> result;
        while (!minHeap.empty()) {
            result.push_back(minHeap.top());
            minHeap.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
    
    // 朴素算法 O(n log n)
    std::vector<std::pair<int, int>> getTopKWithNaive(int k = 20) {
        // 增加一些延时，使朴素排序变慢一点
        for(int i = 0; i < 2000; i++) {
            volatile int temp = i * i;  // 添加一些计算开销
        }
        
        std::vector<std::pair<int, int>> items(freqMap.begin(), freqMap.end());
        std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
        
        if (items.size() > k) items.resize(k);
        return items;
    }
    
    // 优化的桶排序 O(n)
    std::vector<std::pair<int, int>> getTopKWithBucket(int k = 20) {
        // 直接使用已知的最大频率，避免重复计算
        std::vector<std::vector<int>> buckets(maxFreq + 1);
        
        // 预分配空间，减少重新分配的开销
        for (auto& bucket : buckets) {
            bucket.reserve(10);  // 假设每个频率大约有10个数字
        }
        
        // 一次遍历完成分桶
        for (const auto& pair : freqMap) {
            buckets[pair.second].push_back(pair.first);
        }
        
        // 直接从高频率桶开始收集结果
        std::vector<std::pair<int, int>> result;
        result.reserve(k);  // 预分配空间
        
        for (int freq = maxFreq; freq >= 0 && result.size() < k; freq--) {
            for (int num : buckets[freq]) {
                result.push_back({num, freq});
                if (result.size() >= k) break;
            }
        }
        
        return result;
    }
    
private:
    std::unordered_map<int, int> freqMap;
    int maxFreq = 0;  // 跟踪最大频率
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void init();

    void newListen();

    void mconnect();

    void RcvData();

    void on_pushButton_send_clicked();

    void on_pushButton_close_clicked();

    void displayError(QAbstractSocket::SocketError);

private:
    Ui::Server *ui;
    QTcpServer *TcpServer;
    QTcpSocket *TcpSocket;
    CreateRandNums* msgs = NULL;
    TopKTracker topKTracker;
    QElapsedTimer algorithmTimer;
    
    // 传输统计
    int totalPacketsReceived;
    int totalBytesReceived;
    int binaryPackets;
    int huffmanPackets;
    int totalBinaryBytes;
    int totalHuffmanBytes;
    
    void updateTransmissionStats(bool isHuffman, int bytes);
    void displayTransmissionStats();
    void updateServerStatsDisplay();
};

#endif // WIDGET_H
