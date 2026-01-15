#include <GL/glut.h>
#include <Windows.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstdio>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

using namespace std;

struct VERTEX { 
	float x, y; 
	int id;  // Vertex ID: negative for original vertices (A,B,C...), positive for intersections (1,2,3...)
};
vector<VERTEX> _polygon;
vector<VERTEX> stepResults[5];

struct RECT_WINDOW {
	float xmin, ymin, xmax, ymax;
};
RECT_WINDOW clipWindow = { 2.0f, 2.0f, 6.0f, 6.0f };

int winWidth = 1400, winHeight = 900;
int nextIntersectionId = 1;  // Counter for intersection points

// Function declarations
vector<VERTEX> SutherlandHodgeman(vector<VERTEX> polygon, RECT_WINDOW window);
void DrawString(float x, float y, const char* text);

enum BOUNDARY_TYPE {
	LEFT = 0, BOTTOM = 1, RIGHT = 2, TOP = 3
};

bool IsInside(VERTEX p, int boundaryType, RECT_WINDOW window) {
	switch (boundaryType) {
	case LEFT:   return p.x >= window.xmin;
	case BOTTOM: return p.y >= window.ymin;
	case RIGHT:  return p.x <= window.xmax;
	case TOP:    return p.y <= window.ymax;
	default:     return false;
	}
}

VERTEX GetIntersection(VERTEX p1, VERTEX p2, int boundaryType, RECT_WINDOW window) {
	VERTEX intersection;
	intersection.id = nextIntersectionId++;  // Assign new intersection ID
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	const float eps = 1e-6f;

	switch (boundaryType) {
	case LEFT:
		intersection.x = window.xmin;
		intersection.y = (fabsf(dx) < eps) ? p1.y : (p1.y + dy * (window.xmin - p1.x) / dx);
		break;
	case BOTTOM:
		intersection.y = window.ymin;
		intersection.x = (fabsf(dy) < eps) ? p1.x : (p1.x + dx * (window.ymin - p1.y) / dy);
		break;
	case RIGHT:
		intersection.x = window.xmax;
		intersection.y = (fabsf(dx) < eps) ? p1.y : (p1.y + dy * (window.xmax - p1.x) / dx);
		break;
	case TOP:
		intersection.y = window.ymax;
		intersection.x = (fabsf(dy) < eps) ? p1.x : (p1.x + dx * (window.ymax - p1.y) / dy);
		break;
	}
	return intersection;
}

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
			// No output
		}
		else {
			VERTEX intersection = GetIntersection(p1, p2, boundaryType, window);
			output.push_back(intersection);
			output.push_back(p2);
		}
	}
	return output;
}

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

void DrawString(float x, float y, const char* text) {
	glRasterPos2f(x, y);
	for (const char* c = text; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
}

void DrawPolygon(const vector<VERTEX>& poly, float r, float g, float b) {
	if (poly.empty()) return;

	glColor3f(r, g, b);
	glLineWidth(2.5f);
	glBegin(GL_LINE_LOOP);
	for (const auto& p : poly) {
		glVertex2f(p.x, p.y);
	}
	glEnd();
	glLineWidth(1.0f);
}

// Draw a single step
void DrawStep(int stepIndex, int offsetX, int offsetY, float scale) {
	if (stepIndex < 0 || stepIndex > 4 || stepResults[stepIndex].empty()) return;

	glPushMatrix();
	glTranslatef(offsetX, offsetY, 0);
	glScalef(scale, scale, 1.0f);

	// Draw axes
	glColor3f(0.6f, 0.6f, 0.6f);
	glLineWidth(1.5f);
	glBegin(GL_LINES);
	glVertex2f(0, 0); glVertex2f(10, 0);
	glVertex2f(0, 0); glVertex2f(0, 10);
	glEnd();

	// Draw tick marks (every 1 unit)
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = 1; i <= 9; i++) {
		glBegin(GL_LINES);
		glVertex2f(i, -0.1f); glVertex2f(i, 0.1f);
		glVertex2f(-0.1f, i); glVertex2f(0.1f, i);
		glEnd();

		char num[8];
		sprintf_s(num, "%d", i);
		glColor3f(0.3f, 0.3f, 0.3f);
		glRasterPos2f(i - 0.08f, -0.4f);
		for (const char* c = num; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
		}
		glRasterPos2f(-0.4f, i - 0.08f);
		for (const char* c = num; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
		}
		glColor3f(0.5f, 0.5f, 0.5f);
	}

	glLineWidth(1.0f);
	glColor3f(0.2f, 0.2f, 0.2f);
	glRasterPos2f(10.2f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
	glRasterPos2f(-0.4f, 10.2f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
	glRasterPos2f(-0.3f, -0.3f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'O');

	// Draw clip window
	glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(clipWindow.xmin, clipWindow.ymin);
	glVertex2f(clipWindow.xmax, clipWindow.ymin);
	glVertex2f(clipWindow.xmax, clipWindow.ymax);
	glVertex2f(clipWindow.xmin, clipWindow.ymax);
	glEnd();
	glLineWidth(1.0f);


	// Different colors for different steps
	float colors[5][3] = {
		{0.0f, 0.0f, 1.0f},   // Blue
		{1.0f, 0.5f, 0.0f},   // Orange
		{1.0f, 0.8f, 0.0f},   // Gold
		{0.6f, 0.0f, 0.8f},   // Purple
		{0.0f, 0.7f, 0.0f}    // Green
	};

	// Draw polygon
	DrawPolygon(stepResults[stepIndex],
		colors[stepIndex][0],
		colors[stepIndex][1],
		colors[stepIndex][2]);

	// Draw vertices
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < stepResults[stepIndex].size(); i++) {
		glVertex2f(stepResults[stepIndex][i].x, stepResults[stepIndex][i].y);
	}
	glEnd();
	glPointSize(1.0f);

	// Draw vertex labels (only letters A,B,C,D,E, no numbers)
	glColor3f(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < stepResults[stepIndex].size(); i++) {
		int vid = stepResults[stepIndex][i].id;
		if (vid < 0) {
			// Original vertex: use letter A, B, C...
			char label[8];
			sprintf_s(label, "%c", 'A' + (-vid - 1));
			glRasterPos2f(stepResults[stepIndex][i].x + 0.15f, stepResults[stepIndex][i].y + 0.15f);
			for (const char* c = label; *c != '\0'; c++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
			}
		}
		// Skip intersection points (positive id) - don't draw their labels
	}

	glPopMatrix();

	// Draw title (outside matrix transform)
	const char* stepTitles[] = {
		"Step 0: Original",
		"Step 1: Left (x>=2)",
		"Step 2: Bottom (y>=2)",
		"Step 3: Right (x<=6)",
		"Step 4: Top (y<=6)"
	};

	glColor3f(0.0f, 0.0f, 0.0f);
	DrawString(offsetX + 5, offsetY + scale * 10 + 15, stepTitles[stepIndex]);

	// Show vertex labels (only original vertices with letters)
	char info[128];
	string labelList = "Vertices: ";
	bool first = true;
	for (size_t i = 0; i < stepResults[stepIndex].size(); i++) {
		int vid = stepResults[stepIndex][i].id;
		if (vid < 0) {  // Only show original vertices
			if (!first) labelList += ", ";
			char label[8];
			sprintf_s(label, "%c", 'A' + (-vid - 1));
			labelList += label;
			first = false;
		}
	}
	glColor3f(0.5f, 0.5f, 0.5f);
	DrawString(offsetX + 5, offsetY + scale * 10, labelList.c_str());
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Main title
	glColor3f(0.0f, 0.0f, 0.6f);
	DrawString(winWidth / 2 - 170, winHeight - 30, "Sutherland-Hodgeman Polygon Clipping Algorithm");

	char windowInfo[128];
	sprintf_s(windowInfo, "Clip Window: (%.1f,%.1f) to (%.1f,%.1f)",
		clipWindow.xmin, clipWindow.ymin, clipWindow.xmax, clipWindow.ymax);
	glColor3f(0.6f, 0.0f, 0.0f);
	DrawString(winWidth / 2 - 100, winHeight - 55, windowInfo);

	// Layout parameters
	int cellWidth = 420;
	int cellHeight = 400;
	int spacingX = 40;
	int spacingY = 40;
	int marginX = 30;
	int marginTop = 90;
	float scale = 35.0f;  // Scale for coordinate system (10 units = 350 pixels)

	// Row 1: Steps 0, 1, 2
	int row1Y = winHeight - marginTop - cellHeight;
	DrawStep(0, marginX, row1Y, scale);
	DrawStep(1, marginX + cellWidth + spacingX, row1Y, scale);
	DrawStep(2, marginX + 2 * (cellWidth + spacingX), row1Y, scale);

	// Row 2: Steps 3, 4
	int row2Y = row1Y - cellHeight - spacingY;
	int centerOffset = (cellWidth + spacingX) / 2;
	DrawStep(3, marginX + centerOffset, row2Y, scale);
	DrawStep(4, marginX + centerOffset + cellWidth + spacingX, row2Y, scale);

	glutSwapBuffers();
}

void ChangeSize(int w, int h) {
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, winWidth, 0, winHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void KeyEvent(unsigned char key, int x, int y) {
	if (key == 27) exit(0);  // ESC
}

void Initial() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main(int argc, char* argv[]) {
	// Initialize polygon with labeled vertices (A, B, C, D, E) from work2p.cpp
	VERTEX p[5] = { 
		{4.0f, 1.0f, -1},  // A
		{5.0f, 4.0f, -2},  // B
		{8.0f, 1.0f, -3},  // C
		{6.0f, 8.0f, -4},  // D
		{1.0f, 5.0f, -5}   // E
	};
	for (int i = 0; i < 5; i++) _polygon.push_back(p[i]);

	// Reset intersection counter
	nextIntersectionId = 1;

	// Execute clipping
	SutherlandHodgeman(_polygon, clipWindow);

	// Print info
	printf("\n==========================================\n");
	printf("  Sutherland-Hodgeman Polygon Clipping\n");
	printf("==========================================\n");
	printf("Clip Window: (%.1f,%.1f) to (%.1f,%.1f)\n\n",
		clipWindow.xmin, clipWindow.ymin, clipWindow.xmax, clipWindow.ymax);

	for (int i = 0; i <= 4; i++) {
		printf("Step %d vertices: [", i);
		bool first = true;
		for (size_t j = 0; j < stepResults[i].size(); j++) {
			int vid = stepResults[i][j].id;
			if (vid < 0) {  // Only show original vertices
				if (!first) printf(", ");
				printf("%c", 'A' + (-vid - 1));
				first = false;
			}
		}
		printf("]\n");
	}

	printf("\nOriginal vertices displayed: A, B, C, D, E\n");
	printf("All steps displayed. Press ESC to exit.\n");
	printf("==========================================\n\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Sutherland-Hodgeman Polygon Clipping");
	glutDisplayFunc(Display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyEvent);
	Initial();

	glutMainLoop();
	return 0;
}
