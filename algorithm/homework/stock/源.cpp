#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
using namespace std;

int n, m, d, totalPiles;
vector<int> powers;  // 存储 7 的幂次，powers[i] = 7^i

// 使用一个全局记忆化缓存，key 由 (d, state) 合成
unordered_map<long long, double> dp;

// 将 d 和 state 合成一个 key：将 d 左移 32 位再加上 state
long long encodeKey(int ops, int state) {
    return ((long long)ops << 32) | state;
}

// 递归函数，返回剩余 d 次操作下，从 state 状态最终使得小亮的堆全部为空的概率
double rec(int ops, int state) {
    long long key = encodeKey(ops, state);
    if (dp.find(key) != dp.end())
        return dp[key];

    // 当操作次数为0时，判断小亮的堆是否全为空
    if (ops == 0) {
        bool ok = true;
        for (int i = n; i < totalPiles; i++) {
            int cnt = (state / powers[i]) % 7;
            if (cnt > 0) { ok = false; break; }
        }
        return dp[key] = ok ? 1.0 : 0.0;
    }

    // 找到所有非空堆
    vector<int> available;
    for (int i = 0; i < totalPiles; i++) {
        int cnt = (state / powers[i]) % 7;
        if (cnt > 0)
            available.push_back(i);
    }

    // 若没有可选堆，则状态保持不变（吸收态）
    if (available.empty()) {
        bool ok = true;
        for (int i = n; i < totalPiles; i++) {
            int cnt = (state / powers[i]) % 7;
            if (cnt > 0) { ok = false; break; }
        }
        return dp[key] = ok ? 1.0 : 0.0;
    }

    double res = 0.0;
    int k = available.size();
    // 每个非空堆都有相同概率 1/k 被选中
    for (int idx : available) {
        int newState = state - powers[idx]; // 对应堆的石子数减1
        res += (1.0 / k) * rec(ops - 1, newState);
    }

    return dp[key] = res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> d;
    totalPiles = n + m;
    vector<int> piles(totalPiles);

    // 读入小明的石子堆（前 n 个）
    for (int i = 0; i < n; i++) {
        cin >> piles[i];
    }
    // 读入小亮的石子堆（后 m 个）
    for (int i = n; i < totalPiles; i++) {
        cin >> piles[i];
    }

    // 预先计算 powers[i] = 7^i
    powers.resize(totalPiles);
    powers[0] = 1;
    for (int i = 1; i < totalPiles; i++) {
        powers[i] = powers[i - 1] * 7;
    }

    // 将 piles 状态编码成整数
    int initState = 0;
    for (int i = 0; i < totalPiles; i++) {
        initState += piles[i] * powers[i];
    }

    double ans = rec(d, initState);
    cout << fixed << setprecision(4) << ans << "\n";
    return 0;
}
