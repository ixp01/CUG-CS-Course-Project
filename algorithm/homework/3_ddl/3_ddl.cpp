#include <iostream>
#include<algorithm>
#include<vector>
using namespace std;

int greedyJob(vector <pair<int,int>> vec,int n)//创建作业排序函数
{
    sort(vec.begin(), vec.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.first > b.first;
        });

    vector<int> selected;  // 存储已选作业的截止期限
    int total = vec[0].first;  // 初始选第一个作业的得分
    selected.push_back(vec[0].second);

    for (size_t i = 1; i < vec.size(); ++i) {
        // 创建临时集合并排序
        vector<int> tmp = selected;
        tmp.push_back(vec[i].second);
        sort(tmp.begin(), tmp.end());

        // 检查所有位置是否满足条件
        bool valid = true;
        for (int k = 0; k < tmp.size(); ++k) {
            if (tmp[k] < k + 1) {  // 第k个作业必须在k+1天前完成
                valid = false;
                break;
            }
        }
        if (valid) {
            selected = tmp;
            total += vec[i].first;
        }
    }
    return total;
}


int main()
{
	int n=0;
	cin >> n;
	vector <pair<int, int>> vec;
	//分别输入题目分数和截至时间
	int first, second;
	for (int i = 0;i < n;i++)
	{
		cin >> first >> second;
		vec.emplace_back(first,second);
	}
	cout << greedyJob(vec, n);
	return 0;
}


