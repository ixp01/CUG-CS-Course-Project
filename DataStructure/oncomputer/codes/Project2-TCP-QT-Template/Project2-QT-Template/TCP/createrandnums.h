#ifndef CREATERANDNUMS_H
#define CREATERANDNUMS_H

#include <stdlib.h>
#include <time.h>
#include <chrono>
#include<iostream>
#include<cstring>
#include<vector>
#include<QString>
#include <QDebug>
#define SUMNUM 10000  //随机数的数量
#define MAXNUM 999  //随机数范围0---MAXNUM

#pragma warning(disable:4996)

using namespace std;
using namespace chrono;
class CreateRandNums
{

public:
    int randseed;
    int* intMSG = NULL;
    QString strMSG = "";
    //存储二进制编码的数组，定长情况下假设每个数字用8位二进制表示，所以对于三位数的整数，最多24位，可自行修改
    char binaryCode[SUMNUM][24];

    CreateRandNums() {
        randseed = -1;
        intMSG = new int[SUMNUM];
    }

    //析构函数
    ~CreateRandNums() {
        if (intMSG != NULL){
            delete intMSG;
        }
        cout << "Object is being deleted" << endl;
    }

    void AddRandNums() {
        randseed = randseed + 1;
        srand(randseed);
//        delete intMSG;
        for (int i = 0; i < SUMNUM; i++) {
            intMSG[i] = (rand()*RAND_MAX%MAXNUM+rand())%MAXNUM + 0;  //0~MAXNUM
        }
    }

    //将随机数数组转换为字符串，仅供参考，你可以自由修改成你喜欢的格式
    void Transform() {
        strMSG = "";
        for (int i = 0; i < SUMNUM; i++) {
            QString oneMSG = QString::number(intMSG[i]);
            strMSG = strMSG + oneMSG + " ";
        }
    }

    //Your Code Here：将随机数数组编码为普通的未压缩二进制数组，用于在客户端编码
    void ToBinaryCode() {
    }

    //Your Code Here：将普通的未压缩二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
    void BinaryCodeToIntArray() {
    }

    //Your Code Here：将随机数数组编码为haffman算法编码后的二进制数组，务必同步传输字典，用于在客户端编码
    void ToHaffmanCode() {
    }

    //Your Code Here：将haffman算法编码后的二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
    void HaffmanCodeToIntArray() {
    }

    //Your Code Here：新增将二进制数组binaryCode转为字符串的函数
    void TransformBinaryCode() {
    }

};


#endif // CREATERANDNUMS_H
