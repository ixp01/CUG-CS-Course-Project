#include <gl/glut.h>
#include <cmath>
#include <cstdio>

// 四面体的四个顶点
float A[3] = { 2.0f, 0.0f, 0.0f };
float B[3] = { 2.0f, 1.0f, 0.0f };
float C[3] = { 0.0f, 1.0f, 0.0f };
float D[3] = { 1.0f, 1.0f, 1.0f };

void Initial()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 背景为白色
    glEnable(GL_DEPTH_TEST);                // 启用深度测试
    glLineWidth(2.0f);                      // 设置线宽
}

void ChangeSize(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    if (w >= h) {
        glOrtho(-4.0 * aspect, 4.0 * aspect, -4.0, 4.0, -10.0, 10.0);
    }
    else {
        glOrtho(-4.0, 4.0, -4.0 / aspect, 4.0 / aspect, -10.0, 10.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 从Y轴正方向向下看，观察XOZ平面
    gluLookAt(0.0, 5.0, 0.0,    // 相机位置
        0.0, 0.0, 0.0,    // 观察点
        0.0, 0.0, 1.0);   // 上方向为Z轴
}

// 绘制四面体所有边
void drawTetrahedron()
{
    glBegin(GL_LINES);

    // 底面三角形ABC
    glVertex3fv(A);
    glVertex3fv(B);

    glVertex3fv(B);
    glVertex3fv(C);

    glVertex3fv(C);
    glVertex3fv(A);

    // 从D到底面三个顶点的边
    glVertex3fv(D);
    glVertex3fv(A);

    glVertex3fv(D);
    glVertex3fv(B);

    glVertex3fv(D);
    glVertex3fv(C);

    glEnd();
}

// 绘制XOZ平面上的坐标轴
void drawAxes()
{
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);  // 黑色

    glBegin(GL_LINES);

    // X轴
    glVertex3f(-3.0f, 0.0f, 0.0f);
    glVertex3f(4.0f, 0.0f, 0.0f);

    // X轴箭头
    glVertex3f(4.0f, 0.0f, 0.0f);
    glVertex3f(3.8f, 0.0f, 0.1f);
    glVertex3f(4.0f, 0.0f, 0.0f);
    glVertex3f(3.8f, 0.0f, -0.1f);

    // Z轴
    glVertex3f(0.0f, 0.0f, -3.0f);
    glVertex3f(0.0f, 0.0f, 3.0f);

    // Z轴箭头
    glVertex3f(0.0f, 0.0f, 3.0f);
    glVertex3f(0.1f, 0.0f, 2.8f);
    glVertex3f(0.0f, 0.0f, 3.0f);
    glVertex3f(-0.1f, 0.0f, 2.8f);

    glEnd();

    // 绘制X轴刻度线
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = -3; i <= 4; i++) {
        if (i != 0) {
            glVertex3f((float)i, 0.0f, -0.1f);
            glVertex3f((float)i, 0.0f, 0.1f);
        }
    }

    // 绘制Z轴刻度线
    for (int i = -3; i <= 3; i++) {
        if (i != 0) {
            glVertex3f(-0.1f, 0.0f, (float)i);
            glVertex3f(0.1f, 0.0f, (float)i);
        }
    }
    glEnd();

    // 绘制坐标轴标签
    glColor3f(0.0f, 0.0f, 0.0f);

    // X轴标签
    glRasterPos3f(4.2f, 0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');

    // Z轴标签
    glRasterPos3f(0.0f, 0.0f, 3.2f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');

    // 原点标签
    glRasterPos3f(-0.2f, 0.0f, -0.2f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'O');

    glLineWidth(2.0f);
}

// 绘制文本标签
void drawText(const char* text, float x, float y, float z)
{
    glRasterPos3f(x, y, z);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绘制坐标轴
    drawAxes();

    // 正视图（主视图）- 红色
    // XOZ平面，原始位置，不需要变换
    glColor3f(1.0f, 0.0f, 0.0f);
    drawTetrahedron();
    // 添加主视图标签
    glColor3f(1.0f, 0.0f, 0.0f);
    drawText("Main View", 2.5f, 0.0f, 1.5f);

    // 俯视图 - 绿色
    // XOY平面投影到XOZ平面：先沿Z轴平移-1，然后绕X轴旋转-90度
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f);       // 沿Z轴平移-1（平移矢量）
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);   // 绕X轴旋转-90度
    glColor3f(0.0f, 1.0f, 0.0f);
    drawTetrahedron();
    glPopMatrix();
    // 添加俯视图标签
    glColor3f(0.0f, 1.0f, 0.0f);
    drawText("Top View", 3.3f, 0.0f, -2.0f);

    // 侧视图 - 蓝色
    // YOZ平面投影到XOZ平面：先沿X轴平移-1，然后绕Z轴旋转90度
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);       // 沿X轴平移-1（平移矢量）
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);    // 绕Z轴旋转90度
    glColor3f(0.0f, 0.0f, 1.0f);
    drawTetrahedron();
    glPopMatrix();
    // 添加侧视图标签
    glColor3f(0.0f, 0.0f, 1.0f);
    drawText("Side View", -2.8f, 0.0f, 1.5f);

    glutSwapBuffers();
}

// 键盘控制
void Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:  // ESC键退出
    case 'q':
    case 'Q':
        exit(0);
        break;
    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("空间四面体三视图 - T7.7");

    Initial();

    glutDisplayFunc(Display);
    glutReshapeFunc(ChangeSize);
    glutKeyboardFunc(Keyboard);

    // 打印说明信息
    printf("================================\n");
    printf("空间四面体三视图程序 - T7.7\n");
    printf("================================\n");
    printf("顶点坐标：\n");
    printf("  A(2, 0, 0)\n");
    printf("  B(2, 1, 0)\n");
    printf("  C(0, 1, 0)\n");
    printf("  D(1, 1, 1)\n");
    printf("================================\n");
    printf("XOZ平面上的三视图（平移矢量=1）：\n");
    printf("  1. 主视图（红色）- 原始位置\n");
    printf("  2. 俯视图（绿色）- 沿Z平移-1，绕X轴旋转-90度\n");
    printf("  3. 侧视图（蓝色）- 沿X平移-1，绕Z轴旋转90度\n");
    printf("================================\n");
    printf("操作说明：按 ESC 或 Q 键退出\n");
    printf("================================\n");

    glutMainLoop();
    return 0;
}

