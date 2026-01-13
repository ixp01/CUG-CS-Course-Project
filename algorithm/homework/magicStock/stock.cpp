//#include <iostream>
//#include <iomanip>
//#include <vector>
//#include <unordered_map>//哈希表容器
//using namespace std;
//
//int n, m, d, totalPiles;//提前定义总堆数
//vector<int> power;  // 存储7的幂次
//unordered_map<long long, double> dp;//创建存储键值对的容器，第一个位置存储，第二个位置存储概率
//
//long long encodeKey(int ops, int state)//将操作次数ops和各堆石子状态state合并为一个唯一的long long键
//{
//    return ((long long)ops << 32) | state;
//}
//
//double rec(int ops, int state) {
//    long long key = encodeKey(ops, state);//建立键
//    if (dp.count(key)) return dp[key];
//
//    // 检查小亮堆是否全空，并计算总石子数
//    bool liangEmpty = true;//设定标记
//    int sum_liang = 0;
//    for (int i = n; i < totalPiles; ++i) {
//        int cnt = (state / power[i]) % 7;
//        if (cnt > 0) {
//            liangEmpty = false;
//            sum_liang += cnt;
//        }
//    }
//    if (liangEmpty) return dp[key] = 1.0;
//    if (sum_liang > ops) return dp[key] = 0.0;
//    if (ops == 0) return dp[key] = 0.0;
//
//    vector<int> available;
//    for (int i = 0; i < totalPiles; ++i) {
//        if ((state / power[i]) % 7 > 0)
//            available.push_back(i);
//    }
//
//    double res = 0.0;
//    int k = available.size();
//    for (int idx : available) {
//        int newState = state - power[idx];
//        res += rec(ops - 1, newState) / k;
//    }
//
//    return dp[key] = res;
//}
//
//int main() {
//    ios::sync_with_stdio(false);
//    cin.tie(nullptr);
//
//    cin >> n >> m >> d;
//    totalPiles = n + m;
//    vector<int> piles(totalPiles);//创建容器存储各堆的堆数
//
//    for (int i = 0; i < n; ++i) cin >> piles[i];//输入明容器数量
//    for (int i = n; i < totalPiles; ++i) cin >> piles[i];
//
//    power.resize(totalPiles);
//    power[0] = 1;
//    for (int i = 1; i < totalPiles; ++i)
//        power[i] = power[i - 1] * 7;
//
//    int initState = 0;
//    for (int i = 0; i < totalPiles; ++i)
//        initState += piles[i] * power[i];
//
//    cout << fixed << setprecision(4) << rec(d, initState) << "\n";
//    return 0;
//}

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
using namespace std;

// 全局变量：自己的堆数、对手的堆数、操作步数以及总堆数
int ownPilesCount, oppPilesCount, steps, totalStacks;
// 存储 7 的幂次，用于状态编码
vector<int> basePowers;
// 记忆化搜索容器：键为 (剩余步数, 状态) 的唯一编码，值为对应的概率
unordered_map<long long, double> memo;

// 将剩余操作数 ops 和当前状态 state 合并为一个唯一的 64 位整数键
long long generateKey(int ops, int state) {
    return ((long long)ops << 32) | state;
}

// 递归函数：计算在剩余 ops 步内，从状态 state 出发成功清空对手堆的概率
double computeProbability(int ops, int state) {
    long long key = generateKey(ops, state);
    if (memo.count(key)) return memo[key];

    // 判断对手的堆是否已全部清空，并计算对手堆中石子的总数
    bool opponentCleared = true;
    int oppTotalStones = 0;
    // 对手的堆位于 ownPilesCount 到 totalStacks-1
    for (int i = ownPilesCount; i < totalStacks; ++i) {
        int stoneCount = (state / basePowers[i]) % 7;
        if (stoneCount > 0) {
            opponentCleared = false;
            oppTotalStones += stoneCount;
        }
    }
    // 若对手所有堆已清空，成功概率为 1
    if (opponentCleared) return memo[key] = 1.0;
    // 若剩余操作步数不足以减完对手的所有石子，必然失败
    if (oppTotalStones > ops) return memo[key] = 0.0;
    // 没有步数剩余但对手未清空，返回 0
    if (ops == 0) return memo[key] = 0.0;

    // 枚举所有非空堆的索引
    vector<int> nonEmptyIndices;
    for (int i = 0; i < totalStacks; ++i) {
        if ((state / basePowers[i]) % 7 > 0)
            nonEmptyIndices.push_back(i);
    }

    double probSum = 0.0;
    int nonEmptyCount = nonEmptyIndices.size();
    // 遍历所有可能选择的堆，等概率选择一个非空堆
    for (int index : nonEmptyIndices) {
        // 更新状态：相当于该堆石子数减1
        int newState = state - basePowers[index];
        probSum += computeProbability(ops - 1, newState) / nonEmptyCount;
    }

    return memo[key] = probSum;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 输入自己的堆数、对手堆数和总操作步数
    cin >> ownPilesCount >> oppPilesCount >> steps;
    totalStacks = ownPilesCount + oppPilesCount;
    vector<int> stacks(totalStacks);

    // 输入自己的堆石子数
    for (int i = 0; i < ownPilesCount; ++i)
        cin >> stacks[i];
    // 输入对手的堆石子数
    for (int i = ownPilesCount; i < totalStacks; ++i)
        cin >> stacks[i];

    // 初始化 basePowers 数组，计算 7^i
    basePowers.resize(totalStacks);
    basePowers[0] = 1;
    for (int i = 1; i < totalStacks; ++i)
        basePowers[i] = basePowers[i - 1] * 7;

    // 将各堆石子数编码成一个整数状态
    int initialState = 0;
    for (int i = 0; i < totalStacks; ++i)
        initialState += stacks[i] * basePowers[i];

    // 输出最终概率，保留4位小数
    cout << fixed << setprecision(4) << computeProbability(steps, initialState) << "\n";
    return 0;
}
