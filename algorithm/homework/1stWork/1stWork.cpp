#include <iostream>
#include<cmath>
using namespace std;

int main()
{
	int N, K;
	cin >> N >> K;//输入台阶总数N和一次可以走的台阶数K
	if (K < 1)return 0;//防止K的现实意义出问题
	long* Fi = new long[N+1]();//设置出从1到N的空间
	//Fi[1] = 1;//设定数组从一号位置开始
	//Fi[2] = 2;
	Fi[1] = 1; // 2^(1-1) = 1
	for (int m = 2; m <= K; m++) {
		Fi[m] = Fi[m - 1] * 2;
	}
	for (int i = K+1;i <= N;i++)//我们需要求出从K+1号位置开始所有对应的斐波那契数列的值
	{
		for (int j = 1;j <= K;j++)//该循环由f(n)=f(1)+f(2)+...+f(n-K)得出
		{
			Fi[i] += Fi[i - j];
		}
	}
	cout << (Fi[N] % 100003);//获得对应的值
	return 0;
}

