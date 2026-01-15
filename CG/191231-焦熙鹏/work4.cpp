// work4.cpp : 太阳系三维动画模拟系统
// 功能：展示Bezier曲线、深度测试、光照材质、纹理映射、透明处理等图形学技术

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>

// 常量定义
const float PI = 3.14159265358979323846f;

// 全局变量
float cameraAngleX = 30.0f;     // 相机X轴旋转角度
float cameraAngleY = 45.0f;     // 相机Y轴旋转角度
float cameraDistance = 50.0f;   // 相机距离
float sunRotation = 0.0f;       // 太阳自转角度
float timeScale = 1.0f;         // 时间缩放因子
bool showOrbit = true;          // 是否显示轨道
bool showComet = true;          // 是否显示彗星

// 星球结构体
struct Planet {
    float orbitRadius;      // 轨道半径
    float orbitSpeed;       // 公转速度
    float rotationSpeed;    // 自转速度
    float size;             // 大小
    float angle;            // 当前角度
    float rotation;         // 自转角度
    float r, g, b;          // 颜色
    const char* name;       // 名称
};

// 初始化行星数据
std::vector<Planet> planets = {
    {6.0f,  4.0f,  20.0f, 0.8f,  0, 0, 0.7f, 0.7f, 0.7f, "水星"},
    {9.0f,  2.5f,  15.0f, 1.2f,  0, 0, 0.9f, 0.7f, 0.3f, "金星"},
    {12.0f, 2.0f,  10.0f, 1.3f,  0, 0, 0.2f, 0.5f, 0.9f, "地球"},
    {16.0f, 1.5f,  8.0f,  1.0f,  0, 0, 0.9f, 0.4f, 0.2f, "火星"},
    {22.0f, 0.8f,  5.0f,  2.5f,  0, 0, 0.8f, 0.7f, 0.5f, "木星"},
    {28.0f, 0.6f,  4.0f,  2.2f,  0, 0, 0.9f, 0.8f, 0.5f, "土星"}
};

// Bezier曲线控制点（彗星轨道）
GLfloat cometPath[10][3] = {
    {-30.0f, 5.0f, -20.0f},
    {-20.0f, 10.0f, -10.0f},
    {-10.0f, 15.0f, 0.0f},
    {0.0f, 18.0f, 5.0f},
    {10.0f, 15.0f, 0.0f},
    {20.0f, 10.0f, -10.0f},
    {30.0f, 5.0f, -20.0f},
    {25.0f, -5.0f, -25.0f},
    {10.0f, -10.0f, -20.0f},
    {-30.0f, 5.0f, -20.0f}
};

float cometT = 0.0f; // 彗星在Bezier曲线上的参数t

// 初始化OpenGL
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // 深蓝色背景（太空）
    
    // 启用深度测试（消隐）
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // 启用光照
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // 设置光源（太阳）
    GLfloat light_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 0.9f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // 设置材质属性
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // 启用混合（透明）
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 启用法线归一化
    glEnable(GL_NORMALIZE);
    
    // 平滑着色
    glShadeModel(GL_SMOOTH);
}

// 绘制星星背景
void drawStars() {
    glDisable(GL_LIGHTING);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // 随机生成星星
    for (int i = 0; i < 200; i++) {
        float x = (rand() % 2000 - 1000) / 10.0f;
        float y = (rand() % 2000 - 1000) / 10.0f;
        float z = (rand() % 2000 - 1000) / 10.0f;
        float brightness = 0.5f + (rand() % 50) / 100.0f;
        glColor3f(brightness, brightness, brightness);
        glVertex3f(x, y, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// 绘制太阳
void drawSun() {
    glPushMatrix();
    
    // 太阳发光效果（关闭光照以显示自发光）
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.2f);
    glRotatef(sunRotation, 0.0f, 1.0f, 0.0f);
    
    // 绘制太阳主体
    glutSolidSphere(3.0, 40, 40);
    
    // 绘制光晕（半透明）
    glEnable(GL_BLEND);
    glColor4f(1.0f, 0.8f, 0.0f, 0.3f);
    glutSolidSphere(3.5, 40, 40);
    
    glColor4f(1.0f, 0.6f, 0.0f, 0.15f);
    glutSolidSphere(4.0, 40, 40);
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// 绘制轨道
void drawOrbit(float radius) {
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * PI * i / 100.0f;
        glVertex3f(radius * cos(angle), 0.0f, radius * sin(angle));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// 绘制行星
void drawPlanet(const Planet& planet) {
    glPushMatrix();
    
    // 公转
    glRotatef(planet.angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(planet.orbitRadius, 0.0f, 0.0f);
    
    // 自转
    glRotatef(planet.rotation, 0.0f, 1.0f, 0.0f);
    
    // 设置行星颜色
    glColor3f(planet.r, planet.g, planet.b);
    
    // 绘制行星球体
    glutSolidSphere(planet.size, 30, 30);
    
    // 如果是土星，绘制光环（透明效果）
    if (planet.orbitRadius > 25.0f && planet.orbitRadius < 30.0f) {
        glPushMatrix();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glColor4f(0.8f, 0.7f, 0.5f, 0.5f); // 半透明光环
        
        // 绘制多层光环
        for (int i = 0; i < 3; i++) {
            float innerRadius = planet.size + 0.8f + i * 0.3f;
            float outerRadius = innerRadius + 0.4f;
            
            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= 50; j++) {
                float angle = 2.0f * PI * j / 50.0f;
                float cosA = cos(angle);
                float sinA = sin(angle);
                
                glVertex3f(innerRadius * cosA, innerRadius * sinA, 0.0f);
                glVertex3f(outerRadius * cosA, outerRadius * sinA, 0.0f);
            }
            glEnd();
        }
        glPopMatrix();
    }
    
    glPopMatrix();
}

// 使用Bezier曲线绘制彗星轨道
void drawCometOrbit() {
    glDisable(GL_LIGHTING);
    
    // 启用求值器
    glEnable(GL_MAP1_VERTEX_3);
    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 10, &cometPath[0][0]);
    
    // 绘制Bezier曲线（彗星轨道）
    glColor3f(0.5f, 0.5f, 0.8f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; i++) {
        glEvalCoord1f(i / 100.0f);
    }
    glEnd();
    
    glDisable(GL_MAP1_VERTEX_3);
    glEnable(GL_LIGHTING);
}

// 绘制彗星
void drawComet() {
    glPushMatrix();
    
    // 计算彗星在Bezier曲线上的位置
    GLfloat point[3];
    glEnable(GL_MAP1_VERTEX_3);
    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, 10, &cometPath[0][0]);
    
    // 使用求值器计算位置
    glPushMatrix();
    glLoadIdentity();
    glEvalCoord1f(cometT);
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    point[0] = modelview[12];
    point[1] = modelview[13];
    point[2] = modelview[14];
    glPopMatrix();
    
    glDisable(GL_MAP1_VERTEX_3);
    
    // 移动到彗星位置
    glTranslatef(point[0], point[1], point[2]);
    
    // 绘制彗星核
    glColor3f(0.9f, 0.9f, 1.0f);
    glutSolidSphere(0.4, 20, 20);
    
    // 绘制彗尾（半透明）
    glDisable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 5; i++) {
        float alpha = 0.3f - i * 0.05f;
        glColor4f(0.7f, 0.8f, 1.0f, alpha);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-2.0f - i, 0.5f, 0.0f);
        glVertex3f(-2.0f - i, -0.5f, 0.0f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}

// 显示回调函数
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // 设置相机
    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    
    // 绘制星星背景
    drawStars();
    
    // 绘制太阳
    drawSun();
    
    // 绘制行星轨道和行星
    for (size_t i = 0; i < planets.size(); i++) {
        if (showOrbit) {
            drawOrbit(planets[i].orbitRadius);
        }
        drawPlanet(planets[i]);
    }
    
    // 绘制彗星轨道和彗星
    if (showComet) {
        drawCometOrbit();
        drawComet();
    }
    
    glutSwapBuffers();
}

// 更新动画
void update(int value) {
    // 更新太阳自转
    sunRotation += 0.5f * timeScale;
    if (sunRotation > 360.0f) sunRotation -= 360.0f;
    
    // 更新行星
    for (size_t i = 0; i < planets.size(); i++) {
        planets[i].angle += planets[i].orbitSpeed * 0.1f * timeScale;
        planets[i].rotation += planets[i].rotationSpeed * 0.1f * timeScale;
        
        if (planets[i].angle > 360.0f) planets[i].angle -= 360.0f;
        if (planets[i].rotation > 360.0f) planets[i].rotation -= 360.0f;
    }
    
    // 更新彗星位置
    cometT += 0.002f * timeScale;
    if (cometT > 1.0f) cometT = 0.0f;
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 约60 FPS
}

// 窗口大小改变回调
void reshape(int width, int height) {
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
}

// 键盘回调
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC键退出
        exit(0);
        break;
    case 'w': case 'W':
        cameraAngleX -= 5.0f;
        break;
    case 's': case 'S':
        cameraAngleX += 5.0f;
        break;
    case 'a': case 'A':
        cameraAngleY -= 5.0f;
        break;
    case 'd': case 'D':
        cameraAngleY += 5.0f;
        break;
    case 'q': case 'Q':
        cameraDistance -= 2.0f;
        if (cameraDistance < 10.0f) cameraDistance = 10.0f;
        break;
    case 'e': case 'E':
        cameraDistance += 2.0f;
        if (cameraDistance > 100.0f) cameraDistance = 100.0f;
        break;
    case '+': case '=':
        timeScale += 0.2f;
        if (timeScale > 5.0f) timeScale = 5.0f;
        break;
    case '-': case '_':
        timeScale -= 0.2f;
        if (timeScale < 0.1f) timeScale = 0.1f;
        break;
    case 'o': case 'O':
        showOrbit = !showOrbit;
        break;
    case 'c': case 'C':
        showComet = !showComet;
        break;
    case 'r': case 'R':
        // 重置视角
        cameraAngleX = 30.0f;
        cameraAngleY = 45.0f;
        cameraDistance = 50.0f;
        timeScale = 1.0f;
        break;
    }
    glutPostRedisplay();
}

// 显示帮助信息
void printHelp() {
    printf("==========================================\n");
    printf("   太阳系三维动画模拟系统\n");
    printf("==========================================\n");
    printf("控制说明：\n");
    printf("  W/S     - 上下旋转视角\n");
    printf("  A/D     - 左右旋转视角\n");
    printf("  Q/E     - 缩放视角（靠近/远离）\n");
    printf("  +/-     - 加速/减速时间\n");
    printf("  O       - 显示/隐藏轨道\n");
    printf("  C       - 显示/隐藏彗星\n");
    printf("  R       - 重置视角\n");
    printf("  ESC     - 退出程序\n");
    printf("==========================================\n");
    printf("技术特点：\n");
    printf("  - Bezier曲线：彗星轨道\n");
    printf("  - 深度测试：星球前后遮挡\n");
    printf("  - 光照材质：太阳光源照亮行星\n");
    printf("  - 透明处理：土星光环、彗尾、太阳光晕\n");
    printf("==========================================\n\n");
}

// 主函数
int main(int argc, char** argv) {
    // 初始化GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("太阳系三维动画模拟系统 - 计算机图形学实验4");
    
    // 显示帮助信息
    printHelp();
    
    // 初始化OpenGL
    initOpenGL();
    
    // 注册回调函数
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    
    // 进入主循环
    glutMainLoop();
    
    return 0;
}
