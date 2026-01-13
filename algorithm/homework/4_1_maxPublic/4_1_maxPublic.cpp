//#include <iostream>
//#include <vector>
//#include <algorithm>  
//
//using namespace std;
//
//int main() {
//    string s1, s2;
//    cin >> s1 >> s2; 
//
//    int m = s1.length();
//    int n = s2.length();
//
//    // 初始化动态规划表，大小为 (m+1) x (n+1)
//    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
//
//    // 填充dp表：动态规划表
//    for (int i = 1; i <= m; ++i) {
//        for (int j = 1; j <= n; ++j) {
//            if (s1[i - 1] == s2[j - 1]) {
//                dp[i][j] = dp[i - 1][j - 1] + 1;  // 字符匹配，长度+1
//            }
//            else {
//                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);  // 取较大值
//            }
//        }
//    }
//
//    cout << dp[m][n] << endl;  // 输出结果
//    return 0;
//}

#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int main()
{

}
