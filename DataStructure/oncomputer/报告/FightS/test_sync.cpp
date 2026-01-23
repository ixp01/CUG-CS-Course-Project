#include "Data.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "=== 测试改进的文件锁机制 ===";
    
    FileLockManager* lockManager = FileLockManager::getInstance();
    
    // 测试1: 基本锁功能
    qDebug() << "\n--- 测试1: 基本锁功能 ---";
    QString testFile = "test_flights.txt";
    
    if (lockManager->acquireLock(testFile, 1000)) {
        qDebug() << "✓ 成功获取锁";
        
        // 模拟文件操作
        QThread::msleep(500);
        
        lockManager->releaseLock(testFile);
        qDebug() << "✓ 成功释放锁";
    } else {
        qDebug() << "✗ 获取锁失败";
    }
    
    // 测试2: 操作记录功能
    qDebug() << "\n--- 测试2: 操作记录功能 ---";
    Operation op1(OperationType::FlightAdd, "CA1001", "");
    Operation op2(OperationType::FlightUpdate, "CA1002", "");
    
    lockManager->recordOperation(op1);
    lockManager->recordOperation(op2);
    qDebug() << "✓ 记录了两个操作";
    
    // 获取最近操作
    QDateTime since = QDateTime::currentDateTime().addSecs(-10);
    QList<Operation> recentOps = lockManager->getRecentOperations(since);
    qDebug() << "最近操作数量:" << recentOps.size();
    
    // 测试3: 清理过期锁
    qDebug() << "\n--- 测试3: 清理过期锁 ---";
    lockManager->cleanupStaleLocks();
    qDebug() << "✓ 执行了锁清理";
    
    qDebug() << "\n=== 测试完成 ===";
    
    return 0;
}

#include "test_sync.moc" 