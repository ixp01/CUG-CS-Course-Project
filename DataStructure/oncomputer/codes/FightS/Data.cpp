#include "Data.h"
#include <QDir>
#include <QThread>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QUuid>
#include <QDebug>

// FileLockManager实现
FileLockManager* FileLockManager::instance = nullptr;

FileLockManager* FileLockManager::getInstance()
{
    static QMutex staticMutex;
    QMutexLocker locker(&staticMutex);
    if (!instance) {
        instance = new FileLockManager();
    }
    return instance;
}

FileLockManager::FileLockManager()
{
    // 生成唯一的客户端ID
    clientId = generateClientId();
    
    // 设置同步日志文件路径
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    syncLogFile = tempDir + "/flights_sync.log";
    
    qDebug() << "FileLockManager initialized with clientId:" << clientId;
}

FileLockManager::~FileLockManager()
{
    // 释放所有锁
    for (auto it = lockFiles.begin(); it != lockFiles.end(); ++it) {
        if (it.value()) {
            it.value()->unlock();
            delete it.value();
        }
    }
    lockFiles.clear();
}

QString FileLockManager::generateClientId()
{
    return QUuid::createUuid().toString().mid(1, 8); // 取UUID的前8位
}

QString FileLockManager::getLockFileName(const QString& filename)
{
    QFileInfo fileInfo(filename);
    QString lockFileName = fileInfo.absolutePath() + "/" + fileInfo.baseName() + ".lock";
    return lockFileName;
}

bool FileLockManager::acquireLock(const QString& filename, int timeoutMs)
{
    QMutexLocker locker(&mutex);
    
    QString lockFileName = getLockFileName(filename);
    
    // 如果已经持有锁，直接返回true
    if (lockFiles.contains(filename)) {
        return true;
    }
    
    QLockFile* lockFile = new QLockFile(lockFileName);
    
    // 设置锁文件的过期时间（30秒）
    lockFile->setStaleLockTime(30000);
    
    // 尝试获取锁
    if (lockFile->tryLock(timeoutMs)) {
        lockFiles[filename] = lockFile;
        qDebug() << "Successfully acquired lock for:" << filename << "by client:" << clientId;
        return true;
    } else {
        // 获取锁失败，输出详细信息
        qint64 pid;
        QString hostname, appname;
        if (lockFile->getLockInfo(&pid, &hostname, &appname)) {
            qDebug() << "Failed to acquire lock for:" << filename 
                     << "- locked by PID:" << pid << "hostname:" << hostname << "app:" << appname;
        }
        delete lockFile;
        return false;
    }
}

void FileLockManager::releaseLock(const QString& filename)
{
    QMutexLocker locker(&mutex);
    
    if (lockFiles.contains(filename)) {
        QLockFile* lockFile = lockFiles[filename];
        if (lockFile) {
            lockFile->unlock();
            delete lockFile;
            qDebug() << "Released lock for:" << filename << "by client:" << clientId;
        }
        lockFiles.remove(filename);
    }
}

bool FileLockManager::isLocked(const QString& filename)
{
    QString lockFileName = getLockFileName(filename);
    QLockFile lockFile(lockFileName);
    
    // 检查锁文件是否存在且有效
    qint64 pid;
    QString hostname, appname;
    if (lockFile.getLockInfo(&pid, &hostname, &appname)) {
        return true;
    }
    return false;
}

void FileLockManager::recordOperation(const Operation& op)
{
    QMutexLocker locker(&mutex);
    
    QFile file(syncLogFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream out(&file);
        out << static_cast<int>(op.type) << "|"
            << op.data << "|"
            << op.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << "|"
            << op.clientId << "\n";
        file.close();
        
        qDebug() << "Recorded operation:" << static_cast<int>(op.type) 
                 << "by client:" << op.clientId;
    }
}

QList<Operation> FileLockManager::getRecentOperations(const QDateTime& since)
{
    QMutexLocker locker(&mutex);
    QList<Operation> operations;
    
    QFile file(syncLogFile);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|");
            
            if (parts.size() >= 4) {
                Operation op;
                op.type = static_cast<OperationType>(parts[0].toInt());
                op.data = parts[1];
                op.timestamp = QDateTime::fromString(parts[2], "yyyy-MM-dd hh:mm:ss.zzz");
                op.clientId = parts[3];
                
                // 只返回指定时间之后且不是当前客户端的操作
                if (op.timestamp > since && op.clientId != clientId) {
                    operations.append(op);
                }
            }
        }
        file.close();
    }
    
    return operations;
}

void FileLockManager::cleanupStaleLocks()
{
    QMutexLocker locker(&mutex);
    
    // 清理过期的锁文件
    QDir dir(".");
    QStringList lockFiles = dir.entryList(QStringList() << "*.lock", QDir::Files);
    
    for (const QString& lockFileName : lockFiles) {
        QLockFile lockFile(lockFileName);
        
        // 检查锁是否过期
        qint64 pid;
        QString hostname, appname;
        if (!lockFile.getLockInfo(&pid, &hostname, &appname)) {
            // 锁文件无效，删除它
            QFile::remove(lockFileName);
            qDebug() << "Cleaned up stale lock file:" << lockFileName;
        }
    }
}

// Flight类的实现
Flight::Flight()
    : totalSeats(0)
    , availableSeats(0)
    , price(0.0)
    , status(FlightStatus::Normal)
{
}

Flight::Flight(const string& id, const string& air, const QDateTime& dep,
               const QDateTime& arr, const string& from, const string& to,
               int seats, double p)
    : flightID(id)
    , airline(air)
    , departureTime(dep)
    , arrivalTime(arr)
    , fromCity(from)
    , toCity(to)
    , totalSeats(seats)
    , availableSeats(seats)
    , price(p)
    , status(FlightStatus::Normal)
{
}

Flight::Flight(const Flight& other)
    : flightID(other.flightID)
    , airline(other.airline)
    , departureTime(other.departureTime)
    , arrivalTime(other.arrivalTime)
    , viaCities(other.viaCities)
    , fromCity(other.fromCity)
    , toCity(other.toCity)
    , totalSeats(other.totalSeats)
    , availableSeats(other.availableSeats)
    , price(other.price)
    , status(other.status)
{
}

// CityNode类的实现
CityNode::CityNode() : edges() {
}

CityNode::~CityNode() {
    // 清理所有边
    for (auto edge : edges) {
        delete edge;
    }
    edges.clear();
}

// RouteEdge类的实现
RouteEdge::RouteEdge()
    : fromCity(nullptr)
    , toCity(nullptr)
    , flight(nullptr)
    , price(0)
    , duration(0)
{
}

RouteEdge::~RouteEdge() {
    // 注意：这里不删除fromCity、toCity和flight指针
    // 因为它们由其他类管理
}

// Ticket类的实现
Ticket::Ticket()
    : flight(nullptr)
    , isReserved(false)
{
}

Ticket::Ticket(const string& pid, const string& pname, Flight* f)
    : ticketID(pid)
    , passengerName(pname)
    , flight(f)
    , isReserved(false)
{
    purchaseTime = QDateTime::currentDateTime();
}

Ticket::~Ticket() {
    // 注意：这里不删除flight，因为它由FlightManager管理
}
