//#include <GL/glut.h>
//#include <vector>
//#include <iostream>
//#include <cstdio>
//#include <Windows.h>
//
//using namespace std;
//
//// ��ṹ
//struct Point {
//   float x, y;
//   Point() : x(0), y(0) {}
//   Point(float _x, float _y) : x(_x), y(_y) {}
//};
//
//// �ü����ڱ߽�
//float xMin = 2, xMax = 6, yMin = 2, yMax = 6;
//
//// ���Զ���Σ�����ͼƬ�еĶ��㣬�ߣ�AB, BC, CD, DE, EA��
//vector<Point> polygon = {
//   Point(4, 1),  // A
//   Point(5, 4),  // B
//   Point(8, 1),  // C
//   Point(6, 8),  // D
//   Point(1, 5)   // E
//};
//
//// �ü���Ķ����
//vector<Point> clippedPolygon;
//
//// �Ƿ���ִ�вü�
//bool isClipped = false;
//
//// �ߵ�����
//enum EdgeType { LEFT, RIGHT, BOTTOM, TOP };
//
//// ���������ڱߵ�λ�ã��жϵ��Ƿ��ڱߵ��ڲࣩ
//bool isInside(Point p, EdgeType edge) {
//   switch (edge) {
//   case LEFT:   return p.x >= xMin;
//   case RIGHT:  return p.x <= xMax;
//   case BOTTOM: return p.y >= yMin;
//   case TOP:    return p.y <= yMax;
//   }
//   return false;
//}
//
//// ���㽻��
//Point computeIntersection(Point p1, Point p2, EdgeType edge) {
//   float x, y;
//   float dx = p2.x - p1.x;
//   float dy = p2.y - p1.y;
//
//   switch (edge) {
//   case LEFT:
//       x = xMin;
//       y = p1.y + dy * (xMin - p1.x) / dx;
//       break;
//   case RIGHT:
//       x = xMax;
//       y = p1.y + dy * (xMax - p1.x) / dx;
//       break;
//   case BOTTOM:
//       y = yMin;
//       x = p1.x + dx * (yMin - p1.y) / dy;
//       break;
//   case TOP:
//       y = yMax;
//       x = p1.x + dx * (yMax - p1.y) / dy;
//       break;
//   }
//   return Point(x, y);
//}
//
//// ��Ե����߽��вü�
//vector<Point> clipPolygonByEdge(vector<Point> inputPolygon, EdgeType edge) {
//   vector<Point> outputPolygon;
//
//   if (inputPolygon.empty()) return outputPolygon;
//
//   // ��������ε�ÿ����
//   for (size_t i = 0; i < inputPolygon.size(); i++) {
//       Point current = inputPolygon[i];
//       Point next = inputPolygon[(i + 1) % inputPolygon.size()];
//
//       bool currentInside = isInside(current, edge);
//       bool nextInside = isInside(next, edge);
//
//       // ���������
//       if (currentInside && nextInside) {
//           // ���1����ǰ�����һ�㶼���ڲ࣬�����һ��
//           outputPolygon.push_back(next);
//       }
//       else if (currentInside && !nextInside) {
//           // ���2����ǰ�����ڲ࣬��һ������࣬�������
//           outputPolygon.push_back(computeIntersection(current, next, edge));
//       }
//       else if (!currentInside && nextInside) {
//           // ���3����ǰ������࣬��һ�����ڲ࣬����������һ��
//           outputPolygon.push_back(computeIntersection(current, next, edge));
//           outputPolygon.push_back(next);
//       }
//       // ���4�����㶼����࣬������κε�
//   }
//
//   return outputPolygon;
//}
//
//// Sutherland-Hodgeman �㷨������
//vector<Point> sutherlandHodgeman(vector<Point> inputPolygon) {
//   vector<Point> output = inputPolygon;
//
//   // ���ζ������߽��вü�
//   output = clipPolygonByEdge(output, LEFT);
//   output = clipPolygonByEdge(output, RIGHT);
//   output = clipPolygonByEdge(output, BOTTOM);
//   output = clipPolygonByEdge(output, TOP);
//
//   return output;
//}
//
//// ���ƶ����
//void drawPolygon(const vector<Point>& poly, float r, float g, float b) {
//   if (poly.empty()) return;
//
//   // ֻ���Ʊ߿򣨲���䣬���ⰼ�����������⣩
//   glColor3f(r, g, b);
//   glLineWidth(2.5f);
//   glBegin(GL_LINE_LOOP);
//   for (const auto& p : poly) {
//       glVertex2f(p.x, p.y);
//   }
//   glEnd();
//   glLineWidth(1.0f);
//}
//
//// ���Ʋü�����
//void drawClipWindow() {
//   glColor3f(1.0f, 0.0f, 0.0f);  // ��ɫ
//   glLineWidth(2.0f);
//   glBegin(GL_LINE_LOOP);
//   glVertex2f(xMin, yMin);
//   glVertex2f(xMax, yMin);
//   glVertex2f(xMax, yMax);
//   glVertex2f(xMin, yMax);
//   glEnd();
//   glLineWidth(1.0f);
//}
//
//// ����������
//void drawAxes() {
//   glColor3f(0.5f, 0.5f, 0.5f);  // ��ɫ
//   glLineWidth(1.5f);
//
//   // X��
//   glBegin(GL_LINES);
//   glVertex2f(0, 0);
//   glVertex2f(10, 0);
//   glEnd();
//
//   // Y��
//   glBegin(GL_LINES);
//   glVertex2f(0, 0);
//   glVertex2f(0, 10);
//   glEnd();
//
//   // X���ͷ
//   glBegin(GL_TRIANGLES);
//   glVertex2f(10, 0);
//   glVertex2f(9.8f, 0.1f);
//   glVertex2f(9.8f, -0.1f);
//   glEnd();
//
//   // Y���ͷ
//   glBegin(GL_TRIANGLES);
//   glVertex2f(0, 10);
//   glVertex2f(-0.1f, 9.8f);
//   glVertex2f(0.1f, 9.8f);
//   glEnd();
//
//   // ���ƿ̶��ߺͱ�ע
//   glColor3f(0.3f, 0.3f, 0.3f);
//   for (int i = 1; i < 10; i++) {
//       // X��̶�
//       glBegin(GL_LINES);
//       glVertex2f(i, -0.1f);
//       glVertex2f(i, 0.1f);
//       glEnd();
//
//       // X�����ֱ�ע
//       char numStr[3];
//       sprintf_s(numStr, "%d", i);
//       glRasterPos2f(i - 0.1f, -0.4f);
//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, numStr[0]);
//
//       // Y��̶�
//       glBegin(GL_LINES);
//       glVertex2f(-0.1f, i);
//       glVertex2f(0.1f, i);
//       glEnd();
//
//       // Y�����ֱ�ע
//       glRasterPos2f(-0.4f, i - 0.1f);
//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, numStr[0]);
//   }
//
//   glLineWidth(1.0f);
//
//   // �������ǩ
//   glColor3f(0.2f, 0.2f, 0.2f);
//   glRasterPos2f(9.7f, -0.4f);
//   glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'x');
//   glRasterPos2f(-0.4f, 9.7f);
//   glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'y');
//
//   // ԭ���ע
//   glRasterPos2f(-0.3f, -0.3f);
//   glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'O');
//}
//
//// ��������˵��
//void drawText(float x, float y, const char* text) {
//   glRasterPos2f(x, y);
//   for (const char* c = text; *c != '\0'; c++) {
//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
//   }
//}
//
//// ���ƶ����ǩ
//void drawVertexLabels() {
//   glColor3f(0.0f, 0.0f, 0.0f);  // ��ɫ
//   const char* labels[] = { "A(4,1)", "B(5,4)", "C(8,1)", "D(6,8)", "E(1,5)" };
//
//   for (size_t i = 0; i < polygon.size(); i++) {
//       // ����СԲ���Ƕ���
//       glPointSize(6.0f);
//       glBegin(GL_POINTS);
//       glVertex2f(polygon[i].x, polygon[i].y);
//       glEnd();
//
//       // ���Ʊ�ǩ
//       glRasterPos2f(polygon[i].x + 0.15f, polygon[i].y + 0.15f);
//       for (const char* c = labels[i]; *c != '\0'; c++) {
//           glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
//       }
//   }
//   glPointSize(1.0f);
//}
//
//// ��ʾ����
//void display() {
//   glClear(GL_COLOR_BUFFER_BIT);
//
//   // ����������
//   drawAxes();
//
//   // ���Ʋü����ڣ���ɫ��
//   drawClipWindow();
//
//   if (isClipped) {
//       // �ü���ֻ��ʾ�ü��������ɫ��
//       drawPolygon(clippedPolygon, 0.0f, 0.6f, 0.0f);
//   }
//   else {
//       // �ü�ǰ����ʾԭʼ����Σ���ɫ��
//       drawPolygon(polygon, 0.0f, 0.0f, 1.0f);
//       // ���ƶ����ǩ
//       drawVertexLabels();
//   }
//
//   // ���Ʊ����˵��
//   glColor3f(0.0f, 0.0f, 0.0f);  // ��ɫ����
//   //if (isClipped) {
//   //    drawText(2, 9.2, "Clipped Result (Green)");
//   //    drawText(2, 8.7, "Press 'r' to reset");
//   //} else {
//   //    drawText(2, 9.2, "Original Polygon (Blue)");
//   //    drawText(2, 8.7, "Press 'c' to clip");
//   //}
//
//   glutSwapBuffers();
//}
//
//// ��ʼ�� OpenGL
//void init() {
//   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // ��ɫ����
//   glMatrixMode(GL_PROJECTION);
//   glLoadIdentity();
//   gluOrtho2D(-1, 10, -1, 10);  // ��������ϵͳ����չ��ʾ��Χ�Ա㿴��������
//
//   // ��ʼ���ü����
//   clippedPolygon = polygon;
//}
//
//// �����¼�����
//void keyboard(unsigned char key, int x, int y) {
//   switch (key) {
//   case 'c':
//   case 'C':
//       // ִ�вü�
//       clippedPolygon = sutherlandHodgeman(polygon);
//       isClipped = true;
//       cout << "Clipping completed!" << endl;
//       cout << "Original vertices: " << polygon.size() << endl;
//       cout << "Clipped vertices: " << clippedPolygon.size() << endl;
//       glutPostRedisplay();
//       break;
//   case 'r':
//   case 'R':
//       // ����
//       clippedPolygon = polygon;
//       isClipped = false;
//       cout << "Reset to original polygon" << endl;
//       glutPostRedisplay();
//       break;
//   case 27:  // ESC ��
//       exit(0);
//       break;
//   }
//}
//
//// ����¼���������ѡ�����ڽ���ʽ�������Σ�
//void mouse(int button, int state, int x, int y) {
//   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
//       // �������ӽ�������
//   }
//}
//
//// ������
//int main(int argc, char** argv) {
//   // ���ÿ���̨����ҳΪUTF-8
//   SetConsoleOutputCP(65001);
//
//   cout << "========================================" << endl;
//   cout << "  Sutherland-Hodgeman Polygon Clipping" << endl;
//   cout << "========================================" << endl;
//   cout << "Controls:" << endl;
//   cout << "  'c' - performance" << endl;
//   cout << "  'r' - reset" << endl;
//   cout << "  ESC - exit" << endl;
//   cout << "========================================" << endl;
//
//   glutInit(&argc, argv);
//   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//   glutInitWindowSize(500, 500);
//   glutInitWindowPosition(100, 100);
//   glutCreateWindow("Sutherland-Hodgeman Polygon Clipping");
//
//   init();
//
//   glutDisplayFunc(display);
//   glutKeyboardFunc(keyboard);
//   glutMouseFunc(mouse);
//
//   glutMainLoop();
//   return 0;
//}