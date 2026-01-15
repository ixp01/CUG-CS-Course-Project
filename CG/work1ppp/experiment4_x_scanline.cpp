#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <algorithm>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

std::vector<Point> polygon = {
    Point(100, 100), Point(200, 50), Point(300, 100),
    Point(350, 200), Point(250, 250), Point(150, 200)
};

std::vector<Point> fillPixels;

// X扫描线算法
void xScanlineFill() {
    fillPixels.clear();
    
    if (polygon.size() < 3) return;
    
    // 找到多边形的y范围
    int minY = polygon[0].y, maxY = polygon[0].y;
    for (const auto& p : polygon) {
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }
    
    // 对每条扫描线
    for (int y = minY; y <= maxY; y++) {
        std::vector<int> intersections;
        
        // 计算与多边形边的交点
        int n = polygon.size();
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            Point p1 = polygon[i];
            Point p2 = polygon[j];
            
            // 确保p1.y < p2.y
            if (p1.y > p2.y) {
                std::swap(p1, p2);
            }
            
            // 检查扫描线是否与边相交
            if (y >= p1.y && y <= p2.y && p1.y != p2.y) {
                // 计算交点的x坐标
                int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                
                // 处理水平边的特殊情况
                if (y == p2.y && j != 0) {
                    Point p3 = polygon[(j + 1) % n];
                    if ((p1.y < y && p3.y < y) || (p1.y > y && p3.y > y)) {
                        intersections.push_back(x);
                    }
                } else if (y != p2.y) {
                    intersections.push_back(x);
                }
            }
        }
        
        // 排序交点
        std::sort(intersections.begin(), intersections.end());
        
        // 填充交点对之间的像素
        for (int i = 0; i < intersections.size(); i += 2) {
            if (i + 1 < intersections.size()) {
                for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                    fillPixels.push_back(Point(x, y));
                }
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 执行扫描线填充
    xScanlineFill();
    
    // 绘制填充区域（蓝色）
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : fillPixels) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制多边形轮廓（红色）
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : polygon) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制顶点（绿色）
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& p : polygon) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    glFlush();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glPointSize(1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 500.0, 0.0, 400.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 400);
    glutCreateWindow("实验4: X扫描线法多边形填充");
    
    init();
    glutDisplayFunc(display);
    
    std::cout << "实验4: X扫描线法实现多边形填充" << std::endl;
    std::cout << "红色轮廓，蓝色填充，绿色顶点" << std::endl;
    
    glutMainLoop();
    return 0;
}
