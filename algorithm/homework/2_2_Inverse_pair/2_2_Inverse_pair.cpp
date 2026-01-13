#include <iostream>
using namespace std;

int main()
{
    int l = 0;
    cin >> l;
    int* A = new int[l]();
    int sum = 0;
    for (int i = 0;i < l;i++)
    {
        cin >> A[i];
    }
    for (int i = 0;i < l;i++)
    {
        for (int j = i + 1;j < l;j++)
        {
            if (A[i] > A[j])
            {
                sum++;
            }
        }
    }
    cout << sum;
    return 0;
}


