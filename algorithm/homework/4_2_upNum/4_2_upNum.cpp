//#include <iostream>
//#include <vector>
//using namespace std;
//
//int main() {
//    int n, k;
//    cin >> n >> k;
//    vector<int> nums(n);
//    for (int i = 0; i < n; ++i) {
//        cin >> nums[i];
//    }
//
//    vector<vector<long long>> dp(n, vector<long long>(k + 1, 0));
//
//    // 初始化长度为1的子序列
//    for (int i = 0; i < n; ++i) {
//        dp[i][1] = 1;
//    }
//
//    // 动态规划填充dp数组
//    for (int i = 0; i < n; ++i) {
//        for (int j = 0; j < i; ++j) {
//            if (nums[j] < nums[i]) {
//                for (int m = 2; m <= k; ++m) {
//                    dp[i][m] += dp[j][m - 1];
//                }
//            }
//        }
//    }
//
//    // 统计所有长度为k的子序列数目
//    long long total = 0;
//    for (int i = 0; i < n; ++i) {
//        total += dp[i][k];
//    }
//
//    cout << total << endl;
//    return 0;
//}

#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

vector<int> nums;
vector<vector<long long>> memo; // memo[i][m]表示以i结尾长度为m的子序列数

long long count(int i, int m) {
    if (m == 1) return 1; // 基本情况：长度为1的子序列
    if (memo[i][m] != -1) return memo[i][m]; // 记忆化

    long long sum = 0;
    for (int j = 0; j < i; j++) {
        if (nums[j] < nums[i]) {
            sum += count(j, m - 1); // 递归累加
        }
    }
    memo[i][m] = sum; // 保存结果
    return sum;
}

int main() {
    int n, k;
    cin >> n >> k;
    nums.resize(n);
    memo.resize(n, vector<long long>(k + 1, -1)); // 初始化为-1表示未计算

    for (int i = 0; i < n; i++) {
        cin >> nums[i];
    }

    long long total = 0;
    for (int i = 0; i < n; i++) {
        total += count(i, k); // 累加所有以i结尾的长度为k的子序列
    }

    cout << total << endl;
    return 0;
}