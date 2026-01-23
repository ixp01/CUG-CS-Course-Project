#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadcitydata();
    loadisneighbor();
    question1();
    //question2();
    //question3();
    //question4();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadcitydata()
{

    QFile file("D:\\myCollege\\CS class\\DataStructure\\oncomputer\\code\\traffic_system\\省会城市邻接矩阵.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件";
        return;
    }
    else
    {
        qDebug()<<"文件打开了";
    }

    QTextStream in(&file);
    bool firstLine = true; // 用于跳过第一行（城市名称）

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(','); // 按逗号分割数据
        qDebug()<<fields;
        // 跳过第一行城市名
        if (firstLine) {
            firstLine = false;
            continue;
        }

        QVector<int> rowData;
        for (int i = 1; i < fields.size(); ++i) {
            QString s=fields[i];
            rowData.append(s.toDouble()); // 转换为整数并插入到 rowData 中
            qDebug()<<s;
          //  qDebug()<<fields[i].toInt();
        }

        cityDis.append(rowData); // 将解析的行数据插入到 QVector 中
    }

    file.close();
    // 打印数据验证
    int m=cityDis.size();
    int n=cityDis[0].size();
    qDebug()<<m<<n;
    int a=1;
    for(int i=1;i<m;i++){
        for(int j=0;j<a;j++){
            if(i<n)cityDis[j][i]=cityDis[i][j];
            //qDebug()<<cityDis[j][i];

        }
        a++;
    }

}

void MainWindow::loadisneighbor()
{

    QFile file("D:\\myCollege\\CS class\\DataStructure\\oncomputer\\code\\traffic_system\\省会城市邻接矩阵csv.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件";
        return;
    }

    QTextStream in(&file);
    bool firstLine = true; // 用于跳过第一行（城市名称）

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(','); // 按逗号分割数据

        // 跳过第一行城市名
        if (firstLine) {
            for (int i = 1; i < fields.size(); ++i) {
                name.append(fields[i]);
            }
            firstLine = false;
            continue;
        }

        QVector<int> rowData;
        for (int i = 1; i < fields.size(); ++i) {
            rowData.append(fields[i].toInt()); // 转换为整数并插入到 rowData 中
        }

        neighbor.append(rowData); // 将解析的行数据插入到 QVector 中
    }

    file.close();
    
}

//问题1------------------------------------------------------------------------------------------------
void MainWindow::question1()
{
    QSet<int>v;
    int loc=nametoloc("武汉");
    v.insert(loc);
    dfs(v,3,loc);
    int i=0;
    for (const int &elem : v) {
        // 排除港澳台和海口
        if (name[elem] != "台北" && name[elem] != "海口" && 
            name[elem] != "香港" && name[elem] != "澳门") {
            qDebug() << name[elem];
            i++;
        }
    }
    qDebug()<<i;
}
//end------------------------------------------------------------------------------------------------


void MainWindow::dfs(QSet<int> &v, int n, int loc)
{
    if(n==0)return;
    for(int i=0;i<neighbor[0].size();i++){
        if(neighbor[loc][i]==1){
            v.insert(i);
            dfs(v,n-1,i);
        }
    }
}



//问题2-------------------------------------------------------------------------------------------------------

void MainWindow::question2()
{
    // dijkstra(0,21,cityDis);
    // floydWarshall(0,21);
    dfs(0,21);
}
//1.dijkstra算法
QVector<int> MainWindow::dijkstra(int start, int end, QVector<QVector<int> > &matrix)
{
    int n = matrix[0].size();
    QVector<int> dist(n, INT_MAX);
    QVector<int> prev(n, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (u == end) break;
        if (d > dist[u]) continue;

        for (int v = 0; v < n; ++v) {
            if (dist[v] > dist[u] + matrix[u][v]&&neighbor[u][v]==1) {
                dist[v] = dist[u] + matrix[u][v];
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    //qDebug()<<"最短路径:"<<dist[end];
    //ui->shortest->setText(QString::number(dist[end]));
    return reconstructPath(prev, start,end); // 回溯路径
}
//重构完整的路径    
QVector<int> MainWindow::reconstructPath(const QVector<int> &prev,int start, int end)
{
    QVector<int> path;

    // 终点不可达（前驱为-1）
    if (prev[end] == -1 && end != start) {
        return path; // 返回空路径
    }

    // 从终点反向回溯到起点
    int current = end;
    while (current != -1) {
        path.prepend(current); // 插入头部，保证顺序正确
        current = prev[current];
    }

    // 若最终起点不在路径中，说明路径无效
    if (path.isEmpty() || path[0] != start) {
        return QVector<int>();
    }
    QString path1="";
    for(int i=0;i<path.size();i++){
        //qDebug()<<name[path[i]];
        if(i==path.size()-1){
            path1+=name[path[i]];
        }else{
            path1+=name[path[i]]+"->";
        }
    }
    ui->path->setText(path1);
    return path;
}


//2.floyd算法
// Floyd-Warshall算法核心实现
QVector<int> MainWindow::floydWarshall(int start, int end)
{

    int n=cityDis.size();
    int m=cityDis[0].size();
    QVector<QVector<int>>distMatrix(n,QVector<int>(m));
    QVector<QVector<int>> next(n,QVector<int>(m,-1));
    // 初始化距离矩阵：有连接时使用实际距离，否则设为INF
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (i == j) {
                distMatrix[i][j] = 0;    // 自身距离为0
            } else if (neighbor[i][j] == 1) {
                // 使用提供的距离值，保持原数据
                distMatrix[i][j]=cityDis[i][j];
            } else {
                distMatrix[i][j] = INT_MAX;  // 无连接时设为INT_MAX
            }
        }
    }

    // 初始化路径矩阵
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (distMatrix[i][j] != INT_MAX && i != j) {
                next[i][j] = j;  // 直接连接时下一跳为j
            }
        }
    }

    for (int k = 0; k < m; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (distMatrix[i][k] != INT_MAX && distMatrix[k][j] != INT_MAX &&
                    distMatrix[i][j] > distMatrix[i][k] + distMatrix[k][j])
                {
                    distMatrix[i][j] = distMatrix[i][k] + distMatrix[k][j];
                    next[i][j] = next[i][k];  // 更新路径
                }
            }
        }
    }

    return getPath(start,end,next);
    //ui->shortest->setText(QString::number(distMatrix[start][end]));

    //qDebug()<<"最短路径："<<distMatrix[start][end];
}
//重构完整的路径    
QVector<int> MainWindow::getPath(int start, int end, QVector<QVector<int>> next)
{
    QVector<int> path;
    if (next[start][end] == -1) return path; // 不可达

    path.push_back(start);
    while (start != end) {
        start = next[start][end];
        path.push_back(start);
    }
    // for(int i=0;i<path.size();i++){
    //     qDebug()<<name[path[i]];
    // }
    QString path1="";
    for(int i=0;i<path.size();i++){
        //qDebug()<<name[path[i]];
        if(i==path.size()-1){
            path1+=name[path[i]];
        }else{
            path1+=name[path[i]]+"->";
        }
    }
   // ui->path->setText(path1);
    return path;
}

//3.回溯法
// 快速排序的 partition 函数
int partition(QVector<QPair<QVector<int>, int>>& paths, int low, int high) {
    int pivot = paths[high].second;  // 使用最后一个元素的 second 作为 pivot
    int i = low - 1;  // i 是小于 pivot 的元素的最后一个索引

    for (int j = low; j < high; ++j) {
        if (paths[j].second < pivot) {
            ++i;
            // 交换 paths[i] 和 paths[j]
            qSwap(paths[i], paths[j]);
        }
    }

    // 交换 pivot 到正确的位置
    qSwap(paths[i + 1], paths[high]);
    return i + 1;
}

// 快速排序的递归函数
void quickSort(QVector<QPair<QVector<int>, int>>& paths, int low, int high) {
    if (low < high) {
        int pi = partition(paths, low, high);  // 获取 pivot 的位置
        quickSort(paths, low, pi - 1);  // 递归排序 pivot 左侧
        quickSort(paths, pi + 1, high);  // 递归排序 pivot 右侧
    }
}
void MainWindow::dfs(int start, int end)
{
    QVector<QPair<QVector<int>, int>> paths;
    QVector<int>path;
    path.push_back(0);
    int n=ui->cityn->text().toInt();//递归n次
    dfs(paths,n,start,end,path,0);
    quickSort(paths, 0, paths.size() - 1);


    int a=1;
    for (const auto& path : paths) {
        QString path1 = "第"+QString::number(a)+"条路径："+"\n";
        a++;
        int lastIndex = path.first.size() - 1;  // 获取最后一个元素的索引

        for (int i = 0; i < path.first.size(); ++i) {
            int num = path.first[i];

            // 判断是否遍历到最后一个元素
            if (i == lastIndex) {
                path1+=name[num]+"\n";
            }else{
                path1+=name[num]+"->";
            }


        }
        path1+="距离："+QString::number(path.second)+"\n";
        ui->textEdit->insertPlainText(path1);
      //  qDebug() << "Value:" << path.second;
    }
    // 打开文件
    QFile file("D:\\myCollege\\CS class\\DataStructure\\oncomputer\\code\\traffic_system\\traffic_system\\output.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件!";
        return;
    }

    QTextStream out(&file);

    // 输出数据到文件
    for (const auto& path : paths) {
        // 输出 QVector<int> 部分
        out << "路径：";
        for (int i = 0; i < path.first.size(); ++i) {
            out <<name[path.first[i]];
            if (i < path.first.size() - 1) {
                out << ", ";
            }
        }
        out << "      路径距离：";

        // 输出第二个 int 部分
        out << path.second << "\n";
    }

    // 关闭文件
    file.close();

}
void MainWindow::dfs(QVector<QPair<QVector<int>, int> > &paths, int n, int start, int end, QVector<int> path, int distance)
{
    if(start==end){
        QPair<QVector<int>, int>pathinfo;
        pathinfo.first=path;
        pathinfo.second=distance;
        if (!paths.contains(pathinfo)) {
            paths.append(pathinfo);
        }
        return;
    }
    if(n==0)return;
    for(int i=0;i<cityDis[0].size();i++){
        if(neighbor[i][start]==1&&i!=start){
            path.push_back(i);
            distance+=cityDis[start][i];
            dfs(paths,n-1,i,end,path,distance);
            path.pop_back();
            distance-=cityDis[start][i];
        }
    }
}

//end-------------------------------------------------------------------------------------------------------

//问题3-------------------------------------------------------------------------------------------------------
void MainWindow::question3()
{
    //只需要将所有与该城市相邻的城市的邻接关系改为0即可,绕过武汉
    pass(0,21,22);
}

void MainWindow::pass(int start, int end, int passcity)
{
    QVector<int>ischange;
    for(int i=0;i<cityDis[0].size();i++){
        if(neighbor[passcity][i]==1)
        {
            neighbor[passcity][i]=0;
            ischange.push_back(i);
        }
    }

    QVector<int>path;
    if(ui->comboBox->currentText()=="Dijkstras算法"){
        path=dijkstra(start,end,cityDis);

    }else{
        path=floydWarshall(start,end);
    }
    int sum=0;
    QString path1="";
    for(int i=0;i<path.size();i++){
        //qDebug()<<name[path[i]];
        if(i==path.size()-1){
            path1+=name[path[i]];
        }else{
            path1+=name[path[i]]+"->";
        }
    }
    for(int i=0;i<path.size()-1;i++){
        sum+=cityDis[path[i]][path[i+1]];
    }
    ui->shortest->setText(QString::number(sum));
    ui->path->setText(path1);
    for(int i=0;i<ischange.size();i++){
        neighbor[passcity][ischange[i]]=1;
    }
}




//end-------------------------------------------------------------------------------------------------------

//问题4-------------------------------------------------------------------------------------------------------
// Yen's K-Shortest Paths 算法

void MainWindow::question4()
{
    int k=3;
    QVector<QVector<int>>paths=yenKShortestPaths(0,21,k);
    for (int i = 0; i < paths.size(); ++i) {
        qDebug() <<i+1;
        int sum=0;
        for (int j = 0; j < paths[i].size(); ++j) {
            qDebug() << name[paths[i][j]] << " ";  // 输出路径中的每个元素
        }
        for(int m=0;m<paths[i].size()-1;m++){
            sum+=cityDis[paths[i][m]][paths[i][m+1]];
        }
        qDebug() <<"Valus: "<<sum;

    }

}
QVector<QVector<int>> MainWindow::yenKShortestPaths(int start, int end, int k)
{
    QVector<QVector<int>> results;
    QVector<QPair<int, QVector<int>>> candidates; // 存储（权重，路径）

    // 辅助lambda：计算路径总权重
    auto calculateWeight = [&](const QVector<int>& path) {
        int total = 0;
        for (int i = 0; i < path.size() - 1; ++i) {
            if (cityDis[path[i]][path[i+1]] == INT_MAX) return INT_MAX;
            total += cityDis[path[i]][path[i+1]];
        }
        return total;
    };

    // 1. 获取第一条最短路径
    QVector<int> firstPath = dijkstra(start, end, cityDis);
    if (firstPath.isEmpty() || firstPath.first() != start)
        return results;

    results.push_back(firstPath);

    // 2. 迭代寻找后续路径
    for (int target_k = 1; target_k < k; ++target_k) {
        const QVector<int>& prevPath = results[target_k-1];

        // 对前k-1条路径的每个偏离点生成候选
        for (int spurIdx = 0; spurIdx < prevPath.size()-1; ++spurIdx) {
            int spurNode = prevPath[spurIdx];

            // 创建临时图：屏蔽之前路径在偏离点的原始边
            QVector<QVector<int>> tempGraph = cityDis;
            for (const auto& path : results) {
                if (spurIdx < path.size()-1 && path[spurIdx] == spurNode) {
                    tempGraph[path[spurIdx]][path[spurIdx+1]] = INT_MAX;
                }
            }

            // 计算根路径（起点到偏离点）
            QVector<int> rootPath = prevPath.mid(0, spurIdx+1);

            // 计算偏离路径（偏离点到终点）
            QVector<int> spurPath = dijkstra(spurNode, end, tempGraph);
            if (spurPath.isEmpty() || spurPath.first() != spurNode)
                continue;

            // 拼接完整路径并去重
            QVector<int> totalPath = rootPath;
            if (spurPath.size() > 1) {
                totalPath += spurPath.mid(1);
            }

            // 验证路径有效性
            bool exists = false;
            for (const auto& p : results) {
                if (p == totalPath) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                int weight = calculateWeight(totalPath);
                if (weight != INT_MAX) {
                    candidates.append(qMakePair(weight, totalPath));
                }
            }
        }

        // 3. 选择权重最小的候选路径
        if (candidates.isEmpty()) break;

        // 排序候选路径
        std::sort(candidates.begin(), candidates.end(),
                  [](const QPair<int, QVector<int>>& a,
                     const QPair<int, QVector<int>>& b) {
                      return a.first < b.first;
                  });

        // 添加最短候选路径并去重
        auto uniqueEnd = std::unique(candidates.begin(), candidates.end(),
                                     [](const QPair<int, QVector<int>>& a,
                                        const QPair<int, QVector<int>>& b) {
                                         return a.second == b.second;
                                     });
        candidates.erase(uniqueEnd, candidates.end());

        results.append(candidates.first().second);
        candidates.removeFirst();
    }

    // 4. 返回前k条或所有找到的路径
    return results.size() > k ? QVector<QVector<int>>(results.begin(), results.begin()+k) : results;
}




int MainWindow::nametoloc(QString n)
{
    for(int i=0;i<name.size();i++){
        if(name[i]==n){
            return i;
        }
    }
    return -1;
}



int MainWindow::searchdistance(QString city1, QString city2)
{

    if (cityDistances.contains(city1) && cityDistances[city1].contains(city2)) {
        qDebug() << "The distance between" << city1 << "and" << city2 << "is:" << cityDistances[city1][city2] << "km";
        return cityDistances[city1][city2];
    } else {
        qDebug() << "Distance data not available for these cities.";
        return -1;
    }
}

void MainWindow::on_search_clicked()
{
    int start=nametoloc(ui->startcity->text());
    int end=nametoloc(ui->endcity->text());
    if(start==-1||end==-1){
        QMessageBox::information(nullptr, "Title", "输入城市有误！");
        return;
    }
    if(ui->passcity->text()=="无")
    {
        QVector<int>path;
        if(ui->comboBox->currentText()=="Dijkstras算法"){
            path=dijkstra(start,end,cityDis);

        }else{
            path=floydWarshall(start,end);
        }
        int sum=0;
        QString path1="";
        for(int i=0;i<path.size();i++){
            //qDebug()<<name[path[i]];
            if(i==path.size()-1){
                path1+=name[path[i]];
            }else{
                path1+=name[path[i]]+"->";
            }
        }
        for(int i=0;i<path.size()-1;i++){
            sum+=cityDis[path[i]][path[i+1]];
        }
        ui->shortest->setText(QString::number(sum));
        ui->path->setText(path1);
    }else{
        int passcity=nametoloc(ui->passcity->text());
        pass(start,end,passcity);
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    int k=ui->kvalue->text().toInt();
    if(k==0){
        QMessageBox::information(nullptr, "Title", "未输入k值！");
        return;
    }
    int start=nametoloc(ui->startcity->text());
    int end=nametoloc(ui->endcity->text());
    QVector<QVector<int>>paths=yenKShortestPaths(start,end,k);

    int i=paths.size()-1;
    int sum=0;
    QString path1="";
    for (int j = 0; j < paths[i].size(); ++j) {
       // qDebug() << name[paths[i][j]] << " ";  // 输出路径中的每个元素
        if(j==paths[i].size()-1){
           path1+=name[paths[i][j]];
        }else{
        path1+=name[paths[i][j]]+"->";
        }
    }
    ui->kpath->setText(path1);
    for(int m=0;m<paths[i].size()-1;m++){
        sum+=cityDis[paths[i][m]][paths[i][m+1]];
    }
    ui->kshortest->setText(QString::number(sum));


}


void MainWindow::on_searchn_clicked()
{
    int start=nametoloc(ui->startcity->text());
    int end=nametoloc(ui->endcity->text());
    dfs(start,end);
}

