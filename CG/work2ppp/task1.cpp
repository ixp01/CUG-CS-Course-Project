#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

// 点结构
struct Point {
    double x, y, w; // 齐次坐标
    Point(double x = 0, double y = 0, double w = 1) : x(x), y(y), w(w) {}
};

// 3x3变换矩阵
class Matrix3x3 {
public:
    double m[3][3];
    
    Matrix3x3() {
        // 初始化为单位矩阵
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }
    
    // 矩阵乘法
    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                result.m[i][j] = 0;
                for(int k = 0; k < 3; k++)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
        return result;
    }
    
    // 点变换
    Point transform(const Point& p) const {
        Point result;
        result.x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.w;
        result.y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.w;
        result.w = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.w;
        return result;
    }
};

// 平移变换矩阵
Matrix3x3 translate(double tx, double ty) {
    Matrix3x3 mat;
    mat.m[0][2] = tx;
    mat.m[1][2] = ty;
    return mat;
}

// 旋转变换矩阵（绕原点，角度为弧度）
Matrix3x3 rotate(double angle) {
    Matrix3x3 mat;
    double c = cos(angle);
    double s = sin(angle);
    mat.m[0][0] = c;  mat.m[0][1] = -s;
    mat.m[1][0] = s;  mat.m[1][1] = c;
    return mat;
}

// 缩放变换矩阵
Matrix3x3 scale(double sx, double sy) {
    Matrix3x3 mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    return mat;
}

// 错切变换矩阵
Matrix3x3 shear(double shx, double shy) {
    Matrix3x3 mat;
    mat.m[0][1] = shx;
    mat.m[1][0] = shy;
    return mat;
}

// 绕任意点旋转
Matrix3x3 rotateAroundPoint(double cx, double cy, double angle) {
    return translate(cx, cy) * rotate(angle) * translate(-cx, -cy);
}

int main() {
    cout << "=== 二维几何变换演示 ===" << endl;
    
    // 测试点
    Point p(2.0, 3.0);
    cout << "原始点: (" << p.x << ", " << p.y << ")" << endl;
    
    // 平移
    Matrix3x3 trans = translate(5.0, 2.0);
    Point p1 = trans.transform(p);
    cout << "平移(5,2)后: (" << p1.x << ", " << p1.y << ")" << endl;
    
    // 旋转90度
    Matrix3x3 rot = rotate(M_PI / 2);
    Point p2 = rot.transform(p);
    cout << "旋转90度后: (" << p2.x << ", " << p2.y << ")" << endl;
    
    // 缩放
    Matrix3x3 scl = scale(2.0, 0.5);
    Point p3 = scl.transform(p);
    cout << "缩放(2,0.5)后: (" << p3.x << ", " << p3.y << ")" << endl;
    
    // 组合变换：先缩放再旋转再平移
    Matrix3x3 combined = translate(10, 10) * rotate(M_PI/4) * scale(2, 2);
    Point p4 = combined.transform(p);
    cout << "组合变换后: (" << p4.x << ", " << p4.y << ")" << endl;
    
    return 0;
}
