#include <iostream>
#include<cmath>
using namespace std;

int sum = 0;//用于对哥德巴赫运算的计算

int isPrime(int x)//判断是否是质数
{
	if (x < 2)
	{
		return 1;//返回该数是质数
	}
	else
	{
		for (int i = 2;i <= sqrt(x);i++)
		{
			if (x % i == 0)
			{
				return 0;
			}
		}
		return 1;
	}
}

int Goldbach(int x)
{
	if (x < 4)return 0;
	int a = 2;
	for (;a<x/2;a++)
	{
		if (isPrime(a) && isPrime(x - a))
		{
			break;
		}
	}
	sum++;
	if ((x-2*a) < 4) return 0;
	return Goldbach(x-2*a);
}



int main()
{
	int x;
	cin >> x;
	Goldbach(x);
	cout << sum;
	return 0;

}



