

#include <gl/glut.h>
#include <math.h>
#include <iostream>
using namespace std;

// 正确的中点Bresenham算法绘制抛物线 100x = y²
void MidBresenhamParabola() {
    int x = 0, y = 0;
    float d; // 判别式

    glBegin(GL_POINTS);
    glVertex2i(x, y);  // 绘制起始点(0,0)

    // ========== Region 1: y < 50, 斜率|dy/dx| > 1 ==========
    // 中点M(x+0.5, y+1), 判别式d = F(M) = (y+1)² - 100(x+0.5)
    d = (y + 1) * (y + 1) - 100 * (x + 0.5);  // 初始值: d = 1² - 100×0.5 = -49

    cout << "Region 1开始 (y < 50):" << endl;

    while (y < 50) {  // 基于斜率变化的正确切换条件
        // 绘制当前点
        glVertex2i(x, y);
        if (y > 0) glVertex2i(x, -y);

        cout << "点: (" << x << ", " << y << "), d = " << d << endl;

        // 决策
        if (d >= 0) {
            // 中点在抛物线内，选择E点(x+1, y+1)
            x++;
            d += 2 * y + 3 - 100;  // 增量: 2y + 3 - 100
            cout << "  选择E点, 增量=" << (2 * y + 3 - 100) << endl;
        }
        else {
            // 中点在抛物线外，选择N点(x, y+1)  
            d += 2 * y + 3;        // 增量: 2y + 3
            cout << "  选择N点, 增量=" << (2 * y + 3) << endl;
        }
        y++;
    }

    cout << "Region 1结束, 切换到Region 2" << endl;
    cout << "切换点: (" << x << ", " << y << ")" << endl;

    // ========== Region 2: y >= 50, 斜率|dy/dx| ≤ 1 ==========
    // 重新计算判别式确保连续性: 中点M(x+1, y+0.5), d = (y+0.5)² - 100(x+1)
    d = (y + 0.5) * (y + 0.5) - 100 * (x + 1);

    cout << "Region 2开始 (y >= 50):" << endl;
    cout << "初始d = " << d << endl;

    while (x < 300 && y < 250) {  // 限制绘制范围
        // 绘制当前点
        glVertex2i(x, y);
        if (y > 0) glVertex2i(x, -y);

        cout << "点: (" << x << ", " << y << "), d = " << d << endl;

        // 决策
        if (d < 0) {
            // 中点在抛物线内，选择NE点(x+1, y+1)
            d += 2 * y + 2 - 100;  // 增量: 2y + 2 - 100
            y++;
            cout << "  选择NE点, 增量=" << (2 * y + 2 - 100) << endl;
        }
        else {
            // 中点在抛物线外，选择E点(x+1, y)
            d -= 100;            // 增量: -100
            cout << "  选择E点, 增量=-100" << endl;
        }
        x++;
    }

    glEnd();
    cout << "抛物线绘制完成" << endl;
}

// 精确数学绘制抛物线（用于对比验证）
void DrawParabolaExact() {
    glBegin(GL_POINTS);
    for (int x = 0; x <= 300; x++) {
        double y_double = sqrt(100.0 * x);
        int y = (int)(y_double + 0.5);  // 四舍五入

        if (y <= 250) {
            glVertex2i(x, y);
            if (y > 0) glVertex2i(x, -y);
        }
    }
    glEnd();
}

// 绘制坐标网格
void DrawGrid() {
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
    // 垂直网格线
    for (int x = -400; x <= 400; x += 20) {
        glVertex2i(x, -300);
        glVertex2i(x, 300);
    }
    // 水平网格线
    for (int y = -300; y <= 300; y += 20) {
        glVertex2i(-400, y);
        glVertex2i(400, y);
    }
    glEnd();
}

// 绘制坐标轴
void DrawAxes() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2i(-400, 0);
    glVertex2i(400, 0);  // X轴
    glVertex2i(0, -300);
    glVertex2i(0, 300);  // Y轴
    glEnd();

    // 标记区域切换点 (25, 50)
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(25, 50);
    glVertex2i(25, -50);
    glEnd();
}

// 初始化OpenGL
void Initial() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-400.0, 400.0, -300.0, 300.0);
}

// 显示函数
void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. 绘制网格
    DrawGrid();

    // 2. 绘制坐标轴和标记点
    DrawAxes();

    // 3. 绘制精确数学结果（蓝色）
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(1.0f);
    DrawParabolaExact();

    // 4. 绘制中点Bresenham算法结果（红色）
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(3.0f);
    MidBresenhamParabola();

    glFlush();
}

int main(int argc, char* argv[]) {
    cout << "=== 抛物线 100x = y² 的中点Bresenham算法实现 ===" << endl;
    cout << "算法原理:" << endl;
    cout << "1. Region 1 (y < 50): 斜率>1，以y为步进方向" << endl;
    cout << "   - 中点: M(x+0.5, y+1)" << endl;
    cout << "   - 判别式: d = (y+1)² - 100(x+0.5)" << endl;
    cout << "   - d<0选E点(x+1,y+1), d≥0选N点(x,y+1)" << endl;
    cout << endl;
    cout << "2. Region 2 (y ≥ 50): 斜率≤1，以x为步进方向" << endl;
    cout << "   - 中点: M(x+1, y+0.5)" << endl;
    cout << "   - 判别式: d = (y+0.5)² - 100(x+1)" << endl;
    cout << "   - d<0选NE点(x+1,y+1), d≥0选E点(x+1,y)" << endl;
    cout << endl;
    cout << "3. 切换点: y=50 (对应x=25)，斜率dy/dx=1" << endl;
    cout << "4. 绿色点: 标记区域切换位置" << endl;
    cout << "5. 红色: Bresenham算法  蓝色: 精确解" << endl;
    cout << "=============================================" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("正确的抛物线中点Bresenham算法实现");
    glutDisplayFunc(Display);
    Initial();
    glutMainLoop();
    return 0;
}