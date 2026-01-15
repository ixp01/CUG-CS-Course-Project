#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& other) const {
        return abs(x - other.x) < 1e-9 && abs(y - other.y) < 1e-9;
    }
};

// 顶点节点（用于构建交点链表）
struct Vertex {
    Point point;
    bool isIntersection;
    bool isEntry;  // 进入裁剪窗口
    Vertex* next;
    Vertex* neighbor;  // 交点对应的另一多边形上的交点
    bool visited;
    
    Vertex(Point p, bool inter = false) 
        : point(p), isIntersection(inter), isEntry(false), 
          next(nullptr), neighbor(nullptr), visited(false) {}
};

class WeilerAtherton {
private:
    vector<Point> clipWindow;
    
    // 计算两线段交点
    bool getIntersection(const Point& p1, const Point& p2, 
                        const Point& p3, const Point& p4, Point& intersection) {
        double x1 = p1.x, y1 = p1.y;
        double x2 = p2.x, y2 = p2.y;
        double x3 = p3.x, y3 = p3.y;
        double x4 = p4.x, y4 = p4.y;
        
        double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (abs(denom) < 1e-9) return false;
        
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
        
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            intersection.x = x1 + t * (x2 - x1);
            intersection.y = y1 + t * (y2 - y1);
            return true;
        }
        return false;
    }
    
    // 判断点是否在多边形内
    bool pointInPolygon(const Point& p, const vector<Point>& polygon) {
        int n = polygon.size();
        int count = 0;
        
        for (int i = 0; i < n; i++) {
            Point p1 = polygon[i];
            Point p2 = polygon[(i + 1) % n];
            
            if ((p1.y <= p.y && p.y < p2.y) || (p2.y <= p.y && p.y < p1.y)) {
                double x = p1.x + (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                if (p.x < x) count++;
            }
        }
        
        return (count % 2) == 1;
    }
    
    // 构建顶点链表
    Vertex* buildVertexList(const vector<Point>& polygon) {
        if (polygon.empty()) return nullptr;
        
        Vertex* head = new Vertex(polygon[0]);
        Vertex* current = head;
        
        for (size_t i = 1; i < polygon.size(); i++) {
            current->next = new Vertex(polygon[i]);
            current = current->next;
        }
        current->next = head;  // 形成环
        
        return head;
    }
    
    // 插入交点
    void insertIntersections(Vertex* subjectHead, Vertex* clipHead) {
        Vertex* s = subjectHead;
        do {
            Vertex* c = clipHead;
            do {
                Point intersection;
                if (getIntersection(s->point, s->next->point, 
                                   c->point, c->next->point, intersection)) {
                    // 在subject多边形中插入交点
                    Vertex* sInter = new Vertex(intersection, true);
                    sInter->next = s->next;
                    s->next = sInter;
                    
                    // 在clip多边形中插入交点
                    Vertex* cInter = new Vertex(intersection, true);
                    cInter->next = c->next;
                    c->next = cInter;
                    
                    // 设置邻居关系
                    sInter->neighbor = cInter;
                    cInter->neighbor = sInter;
                }
                c = c->next;
            } while (c != clipHead);
            s = s->next;
        } while (s != subjectHead);
    }
    
    // 标记进入/离开点
    void markEntryExit(Vertex* subjectHead) {
        Vertex* current = subjectHead;
        bool status = !pointInPolygon(current->point, clipWindow);
        
        do {
            if (current->isIntersection) {
                current->isEntry = status;
                status = !status;
            }
            current = current->next;
        } while (current != subjectHead);
    }
    
    // 跟踪生成裁剪后的多边形
    vector<Point> tracePolygon(Vertex* start) {
        vector<Point> result;
        Vertex* current = start;
        
        do {
            result.push_back(current->point);
            current->visited = true;
            
            if (current->isIntersection && current->neighbor) {
                current = current->neighbor;
                current->visited = true;
            }
            
            current = current->next;
        } while (current != start && !current->visited);
        
        return result;
    }

public:
    WeilerAtherton(const vector<Point>& window) : clipWindow(window) {}
    
    // 主裁剪函数
    vector<vector<Point>> clip(const vector<Point>& subject) {
        vector<vector<Point>> results;
        
        // 构建链表
        Vertex* subjectHead = buildVertexList(subject);
        Vertex* clipHead = buildVertexList(clipWindow);
        
        if (!subjectHead || !clipHead) return results;
        
        // 插入交点
        insertIntersections(subjectHead, clipHead);
        
        // 标记进入/离开
        markEntryExit(subjectHead);
        
        // 跟踪所有多边形
        Vertex* current = subjectHead;
        do {
            if (current->isIntersection && current->isEntry && !current->visited) {
                vector<Point> polygon = tracePolygon(current);
                if (polygon.size() >= 3) {
                    results.push_back(polygon);
                }
            }
            current = current->next;
        } while (current != subjectHead);
        
        // 如果没有交点但subject完全在clip内
        if (results.empty() && pointInPolygon(subject[0], clipWindow)) {
            results.push_back(subject);
        }
        
        return results;
    }
};

// ==== OpenGL 可视化支持（新增） ====
static vector<Point> gWindow, gSubject;
static vector<vector<Point>> gResults;
static int gWinW = 800, gWinH = 600;

static void drawPolyLine(const vector<Point>& poly, float r, float g, float b, float w = 2.0f) {
    if (poly.empty()) return;
    glColor3f(r, g, b);
    glLineWidth(w);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : poly) glVertex2d(p.x, p.y);
    glEnd();
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (const auto& p : poly) glVertex2d(p.x, p.y);
    glEnd();
}

static void computeBounds(double& minX, double& minY, double& maxX, double& maxY) {
    auto collect = [&](const vector<Point>& poly) {
        for (auto& p : poly) {
            minX = min(minX, p.x); minY = min(minY, p.y);
            maxX = max(maxX, p.x); maxY = max(maxY, p.y);
        }
    };
    minX = minY = 1e100; maxX = maxY = -1e100;
    collect(gWindow);
    collect(gSubject);
    for (auto& poly : gResults) collect(poly);
    if (minX > maxX) { minX = 0; minY = 0; maxX = 100; maxY = 100; } // 兜底
}

static void setupOrtho() {
    double minX, minY, maxX, maxY; 
    computeBounds(minX, minY, maxX, maxY);
    double dx = maxX - minX, dy = maxY - minY;
    double pad = 0.1 * max(1.0, max(dx, dy));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(minX - pad, maxX + pad, minY - pad, maxY + pad);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void glInitWhiteBG() {
    glClearColor(1,1,1,1);
    glDisable(GL_DEPTH_TEST);
    setupOrtho();
}

static void onDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    // 裁剪窗口（蓝色）
    drawPolyLine(gWindow, 0.0f, 0.3f, 1.0f, 2.5f);
    // 原多边形（红色）
    drawPolyLine(gSubject, 1.0f, 0.0f, 0.0f, 2.0f);
    // 裁剪结果（绿色）
    for (auto& poly : gResults) drawPolyLine(poly, 0.0f, 0.6f, 0.0f, 2.5f);
    glutSwapBuffers();
}

static void onReshape(int w, int h) {
    gWinW = w; gWinH = h;
    glViewport(0, 0, w, h);
    setupOrtho();
}

static void onKey(unsigned char key, int, int) {
    if (key == 27) exit(0);
}
// ==== 可视化支持结束 ====

int main() {
    cout << "=== Weiler-Atherton多边形裁剪算法 ===" << endl;
    
    // 定义裁剪窗口
    vector<Point> window = {
        Point(0, 0),
        Point(100, 0),
        Point(100, 100),
        Point(0, 100)
    };
    
    WeilerAtherton clipper(window);
    
    // 测试多边形1：部分重叠
    vector<Point> polygon1 = {
        Point(-20, 50),
        Point(50, 120),
        Point(120, 80),
        Point(50, -20)
    };
    
    cout << "\n测试多边形1 (部分重叠):" << endl;
    cout << "原始顶点数: " << polygon1.size() << endl;
    
    vector<vector<Point>> result1 = clipper.clip(polygon1);
    cout << "裁剪后多边形数量: " << result1.size() << endl;
    
    for (size_t i = 0; i < result1.size(); i++) {
        cout << "多边形 " << i + 1 << " 顶点数: " << result1[i].size() << endl;
        cout << "顶点坐标:" << endl;
        for (const auto& p : result1[i]) {
            cout << "  (" << p.x << ", " << p.y << ")" << endl;
        }
    }
    
    // 测试多边形2：完全在内
    vector<Point> polygon2 = {
        Point(20, 20),
        Point(80, 20),
        Point(80, 80),
        Point(20, 80)
    };
    
    cout << "\n测试多边形2 (完全在窗口内):" << endl;
    vector<vector<Point>> result2 = clipper.clip(polygon2);
    cout << "裁剪后多边形数量: " << result2.size() << endl;
    
    // 测试多边形3：凹多边形
    vector<Point> polygon3 = {
        Point(50, 150),
        Point(150, 50),
        Point(50, 50),
        Point(100, 0),
        Point(-50, 50)
    };
    
    cout << "\n测试多边形3 (凹多边形):" << endl;
    cout << "原始顶点数: " << polygon3.size() << endl;
    vector<vector<Point>> result3 = clipper.clip(polygon3);
    cout << "裁剪后多边形数量: " << result3.size() << endl;
    
    // ==== 启动OpenGL可视化（对“测试多边形1”进行展示） ====
    gWindow = window;
    gSubject = polygon1;
    gResults = result1;

    int argc = 1; char* argv[1] = { (char*)"task4" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(gWinW, gWinH);
    glutCreateWindow("Weiler-Atherton - Visualization (Polygon 1)");
    glInitWhiteBG();
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKey);
    glutMainLoop();
    return 0;
}
