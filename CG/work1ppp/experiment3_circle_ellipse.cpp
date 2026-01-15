#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

std::vector<Point> boundaryPixels;
std::vector<Point> fillPixels;

void drawPixel(int x, int y, bool isBoundary = true) {
    if (isBoundary) {
        boundaryPixels.push_back(Point(x, y));
    } else {
        fillPixels.push_back(Point(x, y));
    }
}

// 中点圆算法
void midpointCircle(int xc, int yc, int radius) {
    boundaryPixels.clear();
    fillPixels.clear();
    
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    
    // 绘制8个对称点
    auto plot8Points = [&](int x, int y) {
        drawPixel(xc + x, yc + y);
        drawPixel(xc - x, yc + y);
        drawPixel(xc + x, yc - y);
        drawPixel(xc - x, yc - y);
        drawPixel(xc + y, yc + x);
        drawPixel(xc - y, yc + x);
        drawPixel(xc + y, yc - x);
        drawPixel(xc - y, yc - x);
    };
    
    while (x <= y) {
        plot8Points(x, y);
        
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
    
    // 填充圆的内部
    for (int py = yc - radius; py <= yc + radius; py++) {
        for (int px = xc - radius; px <= xc + radius; px++) {
            int dx = px - xc;
            int dy = py - yc;
            if (dx * dx + dy * dy < radius * radius) {
                drawPixel(px, py, false);
            }
        }
    }
}

// 椭圆算法
void midpointEllipse(int xc, int yc, int a, int b) {
    boundaryPixels.clear();
    fillPixels.clear();
    
    int x = 0, y = b;
    int a2 = a * a, b2 = b * b;
    int d1 = b2 - a2 * b + a2 / 4;
    
    auto plot4Points = [&](int x, int y) {
        drawPixel(xc + x, yc + y);
        drawPixel(xc - x, yc + y);
        drawPixel(xc + x, yc - y);
        drawPixel(xc - x, yc - y);
    };
    
    // 区域1
    while (2 * b2 * x < 2 * a2 * y) {
        plot4Points(x, y);
        
        if (d1 < 0) {
            d1 += 2 * b2 * x + 3 * b2;
        } else {
            d1 += 2 * b2 * x - 2 * a2 * y + 2 * a2 + 3 * b2;
            y--;
        }
        x++;
    }
    
    // 区域2
    int d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        plot4Points(x, y);
        
        if (d2 > 0) {
            d2 += -2 * a2 * y + 3 * a2;
        } else {
            d2 += 2 * b2 * x - 2 * a2 * y + 2 * b2 + 3 * a2;
            x++;
        }
        y--;
    }
    
    // 填充椭圆内部
    for (int py = yc - b; py <= yc + b; py++) {
        for (int px = xc - a; px <= xc + a; px++) {
            int dx = px - xc;
            int dy = py - yc;
            if (dx * dx * b2 + dy * dy * a2 <= a2 * b2) {
                drawPixel(px, py, false);
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 绘制圆
    midpointCircle(150, 150, 80);
    
    // 绘制填充（蓝色）
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : fillPixels) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制边界（红色）
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& p : boundaryPixels) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制椭圆
    midpointEllipse(400, 150, 100, 60);
    
    // 绘制填充（蓝色）
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : fillPixels) {
        glVertex2i(p.x, p.y);
    }
    glEnd();
    
    // 绘制边界（红色）
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& p : boundaryPixels) {
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
    gluOrtho2D(0.0, 600.0, 0.0, 300.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 300);
    glutCreateWindow("实验3: 圆和椭圆扫描转换");
    
    init();
    glutDisplayFunc(display);
    
    std::cout << "实验3: 圆和椭圆扫描转换 - 红色边界，蓝色填充" << std::endl;
    
    glutMainLoop();
    return 0;
}
