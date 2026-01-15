#include <GL/glut.h>
#include <iostream>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 绘制点
    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 0.0f); // 红色
    glBegin(GL_POINTS);
    glVertex2f(-0.8f, 0.8f);
    glVertex2f(-0.6f, 0.8f);
    glVertex2f(-0.4f, 0.8f);
    glEnd();
    
    // 绘制线段
    glLineWidth(2.0f);
    glColor3f(0.0f, 1.0f, 0.0f); // 绿色
    glBegin(GL_LINES);
    glVertex2f(-0.8f, 0.5f);
    glVertex2f(-0.2f, 0.5f);
    glEnd();
    
    // 绘制连续线段
    glColor3f(0.0f, 0.0f, 1.0f); // 蓝色
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.8f, 0.2f);
    glVertex2f(-0.6f, 0.4f);
    glVertex2f(-0.4f, 0.2f);
    glVertex2f(-0.2f, 0.4f);
    glEnd();
    
    // 绘制三角形（轮廓）
    glColor3f(1.0f, 1.0f, 0.0f); // 黄色
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.2f, 0.7f);
    glVertex2f(0.8f, 0.7f);
    glVertex2f(0.5f, 0.3f);
    glEnd();
    
    // 绘制填充三角形
    glColor3f(1.0f, 0.0f, 1.0f); // 紫色
    glBegin(GL_TRIANGLES);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.8f, 0.0f);
    glVertex2f(0.5f, -0.4f);
    glEnd();
    
    // 绘制四边形
    glColor3f(0.0f, 1.0f, 1.0f); // 青色
    glBegin(GL_QUADS);
    glVertex2f(-0.8f, -0.2f);
    glVertex2f(-0.4f, -0.2f);
    glVertex2f(-0.4f, -0.6f);
    glVertex2f(-0.8f, -0.6f);
    glEnd();
    
    // 绘制多边形
    glColor3f(0.5f, 0.5f, 0.5f); // 灰色
    glBegin(GL_POLYGON);
    for(int i = 0; i < 6; i++) {
        float angle = i * 3.14159f / 3.0f;
        glVertex2f(0.5f + 0.2f * cos(angle), -0.8f + 0.2f * sin(angle));
    }
    glEnd();
    
    glFlush();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("实验1: OpenGL基本图元绘制");
    
    init();
    glutDisplayFunc(display);
    
    std::cout << "实验1: 显示不同颜色和属性的基本图元对象" << std::endl;
    
    glutMainLoop();
    return 0;
}
