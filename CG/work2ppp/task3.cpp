#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

// 裁剪窗口边界
enum Edge { LEFT, RIGHT, BOTTOM, TOP };

class SutherlandHodgeman {
private:
    double xmin, xmax, ymin, ymax;
    
    // 判断点是否在边界内侧
    bool inside(const Point& p, Edge edge) {
        switch (edge) {
            case LEFT:   return p.x >= xmin;
            case RIGHT:  return p.x <= xmax;
            case BOTTOM: return p.y >= ymin;
            case TOP:    return p.y <= ymax;
        }
        return false;
    }
    
    // 计算与边界的交点
    Point intersect(const Point& p1, const Point& p2, Edge edge) {
        double x, y;
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        
        switch (edge) {
            case LEFT:
                x = xmin;
                y = p1.y + dy * (xmin - p1.x) / dx;
                break;
            case RIGHT:
                x = xmax;
                y = p1.y + dy * (xmax - p1.x) / dx;
                break;
            case BOTTOM:
                y = ymin;
                x = p1.x + dx * (ymin - p1.y) / dy;
                break;
            case TOP:
                y = ymax;
                x = p1.x + dx * (ymax - p1.y) / dy;
                break;
        }
        
        return Point(x, y);
    }
    
    // 对一条边进行裁剪
    vector<Point> clipByEdge(const vector<Point>& polygon, Edge edge) {
        vector<Point> output;
        
        if (polygon.empty()) return output;
        
        Point prevPoint = polygon.back();
        bool prevInside = inside(prevPoint, edge);
        
        for (const Point& currPoint : polygon) {
            bool currInside = inside(currPoint, edge);
            
            if (currInside) {
                if (!prevInside) {
                    // 从外到内，输出交点和当前点
                    output.push_back(intersect(prevPoint, currPoint, edge));
                }
                output.push_back(currPoint);
            } else if (prevInside) {
                // 从内到外，只输出交点
                output.push_back(intersect(prevPoint, currPoint, edge));
            }
            
            prevPoint = currPoint;
            prevInside = currInside;
        }
        
        return output;
    }

public:
    SutherlandHodgeman(double xmin, double xmax, double ymin, double ymax)
        : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) {}
    
    // 裁剪多边形
    vector<Point> clip(const vector<Point>& polygon) {
        vector<Point> output = polygon;
        
        // 依次对四条边进行裁剪
        output = clipByEdge(output, LEFT);
        output = clipByEdge(output, RIGHT);
        output = clipByEdge(output, BOTTOM);
        output = clipByEdge(output, TOP);
        
        return output;
    }
};

// 计算多边形面积（用于验证）
double polygonArea(const vector<Point>& polygon) {
    double area = 0;
    int n = polygon.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += polygon[i].x * polygon[j].y;
        area -= polygon[j].x * polygon[i].y;
    }
    return abs(area) / 2.0;
}

int main() {
    cout << "=== Sutherland-Hodgeman多边形裁剪算法 ===" << endl;
    
    // 定义裁剪窗口
    SutherlandHodgeman clipper(0, 100, 0, 100);
    
    // 测试多边形1：部分在窗口内
    vector<Point> polygon1 = {
        Point(-10, 50),
        Point(50, 120),
        Point(110, 80),
        Point(60, -10)
    };
    
    cout << "\n多边形1 (部分在窗口内):" << endl;
    cout << "原始顶点数: " << polygon1.size() << endl;
    cout << "原始面积: " << polygonArea(polygon1) << endl;
    
    vector<Point> clipped1 = clipper.clip(polygon1);
    cout << "裁剪后顶点数: " << clipped1.size() << endl;
    cout << "裁剪后面积: " << polygonArea(clipped1) << endl;
    cout << "裁剪后顶点坐标:" << endl;
    for (const auto& p : clipped1) {
        cout << "  (" << p.x << ", " << p.y << ")" << endl;
    }
    
    // 测试多边形2：完全在窗口内
    vector<Point> polygon2 = {
        Point(20, 20),
        Point(80, 20),
        Point(80, 80),
        Point(20, 80)
    };
    
    cout << "\n多边形2 (完全在窗口内):" << endl;
    cout << "原始顶点数: " << polygon2.size() << endl;
    vector<Point> clipped2 = clipper.clip(polygon2);
    cout << "裁剪后顶点数: " << clipped2.size() << endl;
    
    // 测试多边形3：凹多边形
    vector<Point> polygon3 = {
        Point(50, 150),
        Point(150, 50),
        Point(50, 50),
        Point(100, 0),
        Point(-50, 50)
    };
    
    cout << "\n多边形3 (凹多边形):" << endl;
    cout << "原始顶点数: " << polygon3.size() << endl;
    vector<Point> clipped3 = clipper.clip(polygon3);
    cout << "裁剪后顶点数: " << clipped3.size() << endl;
    cout << "裁剪后顶点坐标:" << endl;
    for (const auto& p : clipped3) {
        cout << "  (" << p.x << ", " << p.y << ")" << endl;
    }
    
    return 0;
}
