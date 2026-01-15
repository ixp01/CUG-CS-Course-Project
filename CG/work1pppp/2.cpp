//#include <GL/glut.h>
//#include <iostream>
//#include <vector>
//#include <cmath>
//
//struct Point {
//    int x, y;
//    Point(int x = 0, int y = 0) : x(x), y(y) {}
//};
//
//std::vector<Point> pixels;
//
//void setPixel(int x, int y) {
//    pixels.push_back(Point(x, y));
//}
//
//// 中点Bresenham算法绘制直线
//void bresenhamLine(int x0, int y0, int x1, int y1) {
//    pixels.clear();
//
//    int dx = abs(x1 - x0);
//    int dy = abs(y1 - y0);
//    int sx = (x0 < x1) ? 1 : -1;
//    int sy = (y0 < y1) ? 1 : -1;
//    int err = dx - dy;
//
//    int x = x0, y = y0;
//
//    while (true) {
//        setPixel(x, y);
//
//        if (x == x1 && y == y1) break;
//
//        int e2 = 2 * err;
//        if (e2 > -dy) {
//            err -= dy;
//            x += sx;
//        }
//        if (e2 < dx) {
//            err += dx;
//            y += sy;
//        }
//    }
//}
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    // 绘制多条不同斜率的直线
//    glColor3f(1.0f, 0.0f, 0.0f);
//
//    // 水平线
//    bresenhamLine(50, 400, 350, 400);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    // 垂直线
//    glColor3f(0.0f, 1.0f, 0.0f);
//    bresenhamLine(200, 50, 200, 350);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    // 斜率为1的直线
//    glColor3f(0.0f, 0.0f, 1.0f);
//    bresenhamLine(50, 50, 200, 200);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    // 斜率为-1的直线
//    glColor3f(1.0f, 1.0f, 0.0f);
//    bresenhamLine(250, 50, 400, 200);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    // 斜率为2的直线
//    glColor3f(1.0f, 0.0f, 1.0f);
//    bresenhamLine(450, 50, 500, 150);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    // 斜率为0.5的直线
//    glColor3f(0.0f, 1.0f, 1.0f);
//    bresenhamLine(450, 200, 550, 250);
//    glBegin(GL_POINTS);
//    for (const auto& p : pixels) {
//        glVertex2i(p.x, p.y);
//    }
//    glEnd();
//
//    glFlush();
//}
//
//void init() {
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//    glColor3f(1.0f, 1.0f, 1.0f);
//    glPointSize(2.0f);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.0, 600.0, 0.0, 450.0);
//}
//
//int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//    glutInitWindowSize(600, 450);
//    glutCreateWindow("实验2: 中点Bresenham直线算法");
//
//    init();
//    glutDisplayFunc(display);
//
//    std::cout << "实验2: 中点Bresenham算法绘制不同斜率的直线" << std::endl;
//
//    glutMainLoop();
//    return 0;
//}
