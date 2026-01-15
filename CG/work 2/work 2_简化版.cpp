#include <gl/glut.h>
#include <Windows.h>
#include <math.h>
#include <vector>
#include <set>
#include <string>

// 链接 OpenGL 和 GLUT 库
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

using namespace std;

struct VERTEX { int x, y; };	// 点结构
vector<VERTEX> _polygon;		// 原始多边形
vector<VERTEX> stepResults[5];	// 存储5个步骤的结果

// 裁剪窗口结构
struct RECT_WINDOW {
	int xmin, ymin, xmax, ymax;
};
RECT_WINDOW clipWindow = { 100, 100, 300, 300 };

// 当前显示的步骤 (0-4)
int currentStep = 0;
int maxStep = 4;

int winWidth = 600, winHeight = 600;

// 函数声明
void ScanTransferX(vector<VERTEX> polygon, COLORREF color);
vector<VERTEX> SutherlandHodgeman(vector<VERTEX> polygon, RECT_WINDOW window);
void DrawString(int x, int y, const char* text);

// 边界类型枚举
enum BOUNDARY_TYPE {
	LEFT = 0, BOTTOM = 1, RIGHT = 2, TOP = 3
};

// 判断点是否在指定边界的内侧
bool IsInside(VERTEX p, int boundaryType, RECT_WINDOW window) {
	switch (boundaryType) {
	case LEFT:   return p.x >= window.xmin;
	case BOTTOM: return p.y >= window.ymin;
	case RIGHT:  return p.x <= window.xmax;
	case TOP:    return p.y <= window.ymax;
	default:     return false;
	}
}

// 计算交点
VERTEX GetIntersection(VERTEX p1, VERTEX p2, int boundaryType, RECT_WINDOW window) {
	VERTEX intersection;
	double m;

	switch (boundaryType) {
	case LEFT:
		if (p2.x != p1.x) {
			m = (double)(p2.y - p1.y) / (double)(p2.x - p1.x);
			intersection.x = window.xmin;
			intersection.y = (int)(p1.y + m * (window.xmin - p1.x) + 0.5);
		}
		break;
	case BOTTOM:
		if (p2.y != p1.y) {
			m = (double)(p2.x - p1.x) / (double)(p2.y - p1.y);
			intersection.y = window.ymin;
			intersection.x = (int)(p1.x + m * (window.ymin - p1.y) + 0.5);
		}
		break;
	case RIGHT:
		if (p2.x != p1.x) {
			m = (double)(p2.y - p1.y) / (double)(p2.x - p1.x);
			intersection.x = window.xmax;
			intersection.y = (int)(p1.y + m * (window.xmax - p1.x) + 0.5);
		}
		break;
	case TOP:
		if (p2.y != p1.y) {
			m = (double)(p2.x - p1.x) / (double)(p2.y - p1.y);
			intersection.y = window.ymax;
			intersection.x = (int)(p1.x + m * (window.ymax - p1.y) + 0.5);
		}
		break;
	}
	return intersection;
}

// 对单条边界进行裁剪
vector<VERTEX> ClipByEdge(vector<VERTEX> input, int boundaryType, RECT_WINDOW window) {
	vector<VERTEX> output;
	if (input.size() == 0) return output;

	int n = input.size();
	for (int i = 0; i < n; i++) {
		VERTEX p1 = input[i];
		VERTEX p2 = input[(i + 1) % n];

		bool p1Inside = IsInside(p1, boundaryType, window);
		bool p2Inside = IsInside(p2, boundaryType, window);

		if (p1Inside && p2Inside) {
			output.push_back(p2);
		}
		else if (p1Inside && !p2Inside) {
			VERTEX intersection = GetIntersection(p1, p2, boundaryType, window);
			output.push_back(intersection);
		}
		else if (!p1Inside && !p2Inside) {
			// 不输出
		}
		else {
			VERTEX intersection = GetIntersection(p1, p2, boundaryType, window);
			output.push_back(intersection);
			output.push_back(p2);
		}
	}
	return output;
}

// Sutherland-Hodgeman完整裁剪算法
vector<VERTEX> SutherlandHodgeman(vector<VERTEX> polygon, RECT_WINDOW window) {
	vector<VERTEX> output = polygon;
	stepResults[0] = polygon;

	output = ClipByEdge(output, LEFT, window);
	stepResults[1] = output;

	output = ClipByEdge(output, BOTTOM, window);
	stepResults[2] = output;

	output = ClipByEdge(output, RIGHT, window);
	stepResults[3] = output;

	output = ClipByEdge(output, TOP, window);
	stepResults[4] = output;

	return output;
}

// 绘制文字
void DrawString(int x, int y, const char* text) {
	glRasterPos2i(x, y);
	for (const char* c = text; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
}

// 绘制坐标轴（简洁版）
void DrawAxes() {
	glColor3f(0.6f, 0.6f, 0.6f);
	glLineWidth(1.5f);

	// X轴
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(400, 0);
	glEnd();

	// Y轴
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(0, 400);
	glEnd();

	// 刻度
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = 50; i <= 350; i += 50) {
		glBegin(GL_LINES);
		glVertex2i(i, -3);
		glVertex2i(i, 3);
		glEnd();

		char num[8];
		sprintf_s(num, "%d", i);
		glColor3f(0.3f, 0.3f, 0.3f);
		DrawString(i - 8, -15, num);
		glColor3f(0.5f, 0.5f, 0.5f);

		glBegin(GL_LINES);
		glVertex2i(-3, i);
		glVertex2i(3, i);
		glEnd();

		glColor3f(0.3f, 0.3f, 0.3f);
		DrawString(-25, i - 4, num);
		glColor3f(0.5f, 0.5f, 0.5f);
	}

	glLineWidth(1.0f);
	glColor3f(0.2f, 0.2f, 0.2f);
	DrawString(405, -5, "X");
	DrawString(-10, 405, "Y");
	DrawString(-15, -20, "O");
}

// X-扫描线填充算法
void ScanTransferX(vector<VERTEX> polygon, COLORREF color) {
	if (polygon.size() < 3) return;
	
	int ymin = 10000, ymax = -10000, nVertex = polygon.size();
	for (int i = 0; i < nVertex; i++) {
		if (polygon[i].y < ymin) ymin = polygon[i].y;
		if (polygon[i].y > ymax) ymax = polygon[i].y;
	}
	polygon.push_back(polygon[0]);

	for (int yi = ymin; yi <= ymax; yi++) {
		multiset<int> vaXs;
		for (int i = 0, xi; i < nVertex; i++) {
			VERTEX p1 = polygon[i], p2 = polygon[i + 1];
			if ((yi >= p1.y && yi < p2.y) || (yi >= p2.y && yi < p1.y)) {
				if (yi == p1.y) xi = p1.x;
				else if (yi == p2.y) xi = p2.x;
				else xi = (int)((double)((yi - p1.y) * p2.x + (p2.y - yi) * p1.x) / (double)(p2.y - p1.y) + 0.5);
				vaXs.insert(xi);
			}
			else if (yi == p1.y && yi == p2.y) {
				vaXs.insert(p1.x);
				vaXs.insert(p2.x);
			}
		}

		BYTE r = GetRValue(color), g = GetGValue(color), b = GetBValue(color);
		glColor3ub(r, g, b);
		glBegin(GL_POINTS);
		for (multiset<int>::iterator it = vaXs.begin(); it != vaXs.end(); ++it) {
			int nBeginX = (*it), nEndX = *(++it);
			for (int j = nBeginX; j <= nEndX; j++) {
				glVertex2i(j, yi);
			}
		}
		glEnd();
	}
}

// 主显示函数
void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	// 绘制坐标轴
	DrawAxes();

	// 绘制裁剪窗口
	glColor3f(0.9f, 0.1f, 0.1f);
	glLineWidth(2.5f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(clipWindow.xmin, clipWindow.ymin);
	glVertex2i(clipWindow.xmax, clipWindow.ymin);
	glVertex2i(clipWindow.xmax, clipWindow.ymax);
	glVertex2i(clipWindow.xmin, clipWindow.ymax);
	glEnd();
	glLineWidth(1.0f);

	// 裁剪窗口标注
	glColor3f(0.9f, 0.1f, 0.1f);
	char windowInfo[64];
	sprintf_s(windowInfo, "裁剪窗口 (%d,%d)-(%d,%d)", 
		clipWindow.xmin, clipWindow.ymin, clipWindow.xmax, clipWindow.ymax);
	DrawString(clipWindow.xmin + 5, clipWindow.ymax + 15, windowInfo);

	// 根据当前步骤绘制多边形
	if (currentStep >= 0 && currentStep <= maxStep && stepResults[currentStep].size() > 0) {
		// 颜色随步骤变化
		COLORREF colors[] = {
			RGB(173, 216, 230),  // 步骤0 - 淡蓝色
			RGB(255, 218, 185),  // 步骤1 - 浅橙色
			RGB(255, 228, 181),  // 步骤2 - 浅黄色
			RGB(221, 160, 221),  // 步骤3 - 浅紫色
			RGB(144, 238, 144)   // 步骤4 - 浅绿色
		};

		// 填充
		ScanTransferX(stepResults[currentStep], colors[currentStep]);

		// 轮廓
		glColor3f(0.2f, 0.2f, 0.2f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		for (size_t i = 0; i < stepResults[currentStep].size(); i++) {
			glVertex2i(stepResults[currentStep][i].x, stepResults[currentStep][i].y);
		}
		glEnd();
		glLineWidth(1.0f);

		// 顶点
		glColor3f(0.8f, 0.0f, 0.0f);
		glPointSize(8.0f);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < stepResults[currentStep].size(); i++) {
			glVertex2i(stepResults[currentStep][i].x, stepResults[currentStep][i].y);
		}
		glEnd();
		glPointSize(1.0f);

		// 顶点编号
		glColor3f(0.0f, 0.0f, 0.0f);
		for (size_t i = 0; i < stepResults[currentStep].size(); i++) {
			char label[32];
			sprintf_s(label, "V%d", (int)i);
			DrawString(stepResults[currentStep][i].x + 8, 
				stepResults[currentStep][i].y + 8, label);
		}
	}

	// 显示步骤信息
	const char* stepTitles[] = {
		"步骤 0: 原始多边形",
		"步骤 1: 左边界裁剪后 (x >= 100)",
		"步骤 2: 下边界裁剪后 (y >= 100)",
		"步骤 3: 右边界裁剪后 (x <= 300)",
		"步骤 4: 上边界裁剪后 (y <= 300) - 最终结果"
	};

	// 顶部标题
	glColor3f(0.0f, 0.0f, 0.7f);
	DrawString(20, winHeight - 20, stepTitles[currentStep]);

	// 顶点数信息
	char info[128];
	sprintf_s(info, "顶点数: %d", (int)stepResults[currentStep].size());
	glColor3f(0.3f, 0.3f, 0.3f);
	DrawString(20, winHeight - 40, info);

	// 如果不是第一步，显示输入输出对比
	if (currentStep > 0) {
		sprintf_s(info, "输入: %d 个顶点 → 输出: %d 个顶点", 
			(int)stepResults[currentStep - 1].size(),
			(int)stepResults[currentStep].size());
		DrawString(20, winHeight - 60, info);
	}

	// 底部操作提示
	glColor3f(0.5f, 0.5f, 0.5f);
	DrawString(20, 30, "操作: [空格/回车]-下一步 [退格]-上一步 [0-4]-跳转 [R]-重置 [ESC]-退出");

	// 进度条
	int barX = 20, barY = 10, barWidth = winWidth - 40, barHeight = 15;
	glColor3f(0.7f, 0.7f, 0.7f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(barX, barY);
	glVertex2i(barX + barWidth, barY);
	glVertex2i(barX + barWidth, barY + barHeight);
	glVertex2i(barX, barY + barHeight);
	glEnd();

	int fillWidth = (int)(barWidth * currentStep / (float)maxStep);
	glColor3f(0.2f, 0.7f, 0.2f);
	glBegin(GL_QUADS);
	glVertex2i(barX + 1, barY + 1);
	glVertex2i(barX + fillWidth, barY + 1);
	glVertex2i(barX + fillWidth, barY + barHeight - 1);
	glVertex2i(barX + 1, barY + barHeight - 1);
	glEnd();

	glFlush();
}

// 窗口大小变化
void ChangeSize(int w, int h) {
	winWidth = w; winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-30.0, winWidth - 30, -30.0, winHeight - 30);
}

// 键盘响应
void KeyEvent(unsigned char key, int x, int y) {
	switch (key) {
	case ' ':
	case 13:
		if (currentStep < maxStep) {
			currentStep++;
			printf(">>> 进入步骤 %d\n", currentStep);
		}
		else {
			printf(">>> 已是最后一步！\n");
		}
		glutPostRedisplay();
		break;
	case 8:
		if (currentStep > 0) {
			currentStep--;
			printf(">>> 返回步骤 %d\n", currentStep);
		}
		glutPostRedisplay();
		break;
	case '0': currentStep = 0; glutPostRedisplay(); break;
	case '1': currentStep = 1; glutPostRedisplay(); break;
	case '2': currentStep = 2; glutPostRedisplay(); break;
	case '3': currentStep = 3; glutPostRedisplay(); break;
	case '4': currentStep = 4; glutPostRedisplay(); break;
	case 'r':
	case 'R':
		currentStep = 0;
		printf(">>> 重置\n");
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	}
}

void Initial() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main(int argc, char* argv[]) {
	// 初始化多边形
	VERTEX p[7] = { {210, 160}, {90, 240}, {30, 140}, {90, 20}, {180, 100}, {240, 20}, {360, 180} };
	for (int i = 0; i < 7; i++) _polygon.push_back(p[i]);

	// 执行裁剪
	SutherlandHodgeman(_polygon, clipWindow);

	// 打印信息
	printf("\n==========================================\n");
	printf("  Sutherland-Hodgeman 多边形裁剪算法\n");
	printf("==========================================\n");
	printf("裁剪窗口: (%d,%d) 到 (%d,%d)\n\n", 
		clipWindow.xmin, clipWindow.ymin, clipWindow.xmax, clipWindow.ymax);
	
	for (int i = 0; i <= 4; i++) {
		printf("步骤 %d: %d 个顶点\n", i, (int)stepResults[i].size());
	}
	
	printf("\n操作说明:\n");
	printf("  [空格] 或 [回车] - 下一步\n");
	printf("  [退格] - 上一步\n");
	printf("  [0-4] - 跳转到指定步骤\n");
	printf("  [R] - 重置\n");
	printf("  [ESC] - 退出\n");
	printf("==========================================\n\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Sutherland-Hodgeman 多边形裁剪算法 - 按空格查看步骤");
	glutDisplayFunc(Display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyEvent);
	Initial();

	glutMainLoop();
	return 0;
}

