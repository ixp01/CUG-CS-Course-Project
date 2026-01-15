#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Edge {
    int yMax;      // 边的最大y值
    float x;       // 当前扫描线与边的交点x坐标
    float dx;      // x增量 (1/斜率)
    
    Edge(int yMax, float x, float dx) : yMax(yMax), x(x), dx(dx) {}
    
    bool operator<(const Edge& other) const {
        return x < other.x;
    }
};

std::vector<Point> polygon = {
    Point(150, 100), Point(250, 80), Point(350, 150),
    Point(320, 250), Point(200, 280), Point(100, 200)
};

std::vector<Point> fillPixels;

// 有效边表法
void activeEdgeTableFill() {
    fillPixels.clear();
    
    if (polygon.size() < 3) return;
    
    // 创建边表ET (Edge Table)
    std::map<int, std::vector<Edge>> edgeTable;
    
    int n = polygon.size();
    int minY = polygon[0].y, maxY = polygon[0].y;
    
    // 构建边表
    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n];
        
        minY = std::min(minY, std::min(p1.y, p2.y));
        maxY = std::max(maxY, std::max(p1.y, p2.y));
        
        // 忽略水平边
        if (p1.y == p2.y) continue;
        
        // 确保p1是下端点，p2是上端点
        if (p1.y > p2.y) {
            std::swap(p1, p2);
        }
        
        // 计算斜率的倒数
        float dx = 0;
        if (p2.y != p1.y) {
            dx = (float)(p2.x - p1.x) / (p2.y - p1.y);
        }
        
        // 将边加入到其下端点对应的边表中
        edgeTable[p1.y].emplace_back(p2.y, p1.x, dx);
    }
    
    // 活性边表AET (Active Edge Table)
    std::vector<Edge> activeEdgeTable;
    
    // 扫描线算法
    for (int y = minY; y <= maxY; y++) {
        // 将新边加入AET
        if (edgeTable.find(y) != edgeTable.end()) {
            for (const auto& edge : edgeTable[y]) {
                activeEdgeTable.push_back(edge);
            }
        }
        
        // 按x坐标排序AET
        std::sort(activeEdgeTable.begin(), activeEdgeTable.end());
        
        // 填充扫描线上的像素
        for (size_t i = 0; i < activeEdgeTable.size(); i += 2) {
            if (i + 1 < activeEdgeTable.size()) {
                int x1 = (int)(activeEdgeTable[i].x + 0.5f);
                int x2 = (int)(activeEdgeTable[i + 1].x + 0.5f);
                
                for (int x = x1; x <= x2; x++) {
                    fillPixels.push_back(Point(x, y));
                }
            }
        }
        
        // 更新AET：移除达到yMax的边，更新其他边的x值
        for (auto it = activeEdgeTable.begin(); it != activeEdgeTable.end();) {
            if (it->yMax <= y) {
                it = activeEdgeTable.erase(it);
            } else {
                it->x += it->dx;
                ++it;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 执行有效边表填充
    activeEdgeTableFill();
    
    // 绘制填充区域（青色）
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : fillPixels) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制多边形轮廓（红色）
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : polygon) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制顶点（黄色）
    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(6.0f);
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 400);
    glutCreateWindow("实验5: 有效边表法多边形填充");
    
    init();
    glutDisplayFunc(display);
    
    std::cout << "实验5: 有效边表法(AET)实现多边形填充" << std::endl;
    std::cout << "红色轮廓，青色填充，黄色顶点" << std::endl;
    
    glutMainLoop();
    return 0;
}
