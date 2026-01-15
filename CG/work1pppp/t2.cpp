//#include <iostream>
//#include <vector>
//#include <algorithm>
//#include <cmath>
//using namespace std;
//
//struct Point {
//    double x, y;
//    Point(double x = 0, double y = 0) : x(x), y(y) {}
//};
//
//struct Line {
//    Point p1, p2;
//    Line(Point p1, Point p2) : p1(p1), p2(p2) {}
//};
//
//// 裁剪窗口
//struct ClipWindow {
//    double xmin, xmax, ymin, ymax;
//    ClipWindow(double xmin, double xmax, double ymin, double ymax)
//        : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax) {
//    }
//};
//
//// Cohen-Sutherland编码
//const int INSIDE = 0; // 0000
//const int LEFT = 1;   // 0001
//const int RIGHT = 2;  // 0010
//const int BOTTOM = 4; // 0100
//const int TOP = 8;    // 1000
//
//int computeCode(const Point& p, const ClipWindow& w) {
//    int code = INSIDE;
//    if (p.x < w.xmin) code |= LEFT;
//    else if (p.x > w.xmax) code |= RIGHT;
//    if (p.y < w.ymin) code |= BOTTOM;
//    else if (p.y > w.ymax) code |= TOP;
//    return code;
//}
//
//// Cohen-Sutherland裁剪算法
//bool cohenSutherlandClip(Line& line, const ClipWindow& w) {
//    int code1 = computeCode(line.p1, w);
//    int code2 = computeCode(line.p2, w);
//    bool accept = false;
//
//    while (true) {
//        if ((code1 | code2) == 0) {
//            // 两端点都在窗口内
//            accept = true;
//            break;
//        }
//        else if (code1 & code2) {
//            // 两端点在窗口同一侧外
//            break;
//        }
//        else {
//            // 需要裁剪
//            int codeOut = code1 ? code1 : code2;
//            double x, y;
//
//            if (codeOut & TOP) {
//                x = line.p1.x + (line.p2.x - line.p1.x) * (w.ymax - line.p1.y) / (line.p2.y - line.p1.y);
//                y = w.ymax;
//            }
//            else if (codeOut & BOTTOM) {
//                x = line.p1.x + (line.p2.x - line.p1.x) * (w.ymin - line.p1.y) / (line.p2.y - line.p1.y);
//                y = w.ymin;
//            }
//            else if (codeOut & RIGHT) {
//                y = line.p1.y + (line.p2.y - line.p1.y) * (w.xmax - line.p1.x) / (line.p2.x - line.p1.x);
//                x = w.xmax;
//            }
//            else {
//                y = line.p1.y + (line.p2.y - line.p1.y) * (w.xmin - line.p1.x) / (line.p2.x - line.p1.x);
//                x = w.xmin;
//            }
//
//            if (codeOut == code1) {
//                line.p1 = Point(x, y);
//                code1 = computeCode(line.p1, w);
//            }
//            else {
//                line.p2 = Point(x, y);
//                code2 = computeCode(line.p2, w);
//            }
//        }
//    }
//    return accept;
//}
//
//// 中点分割裁剪算法
//bool midpointClip(Line& line, const ClipWindow& w) {
//    int code1 = computeCode(line.p1, w);
//    int code2 = computeCode(line.p2, w);
//
//    if ((code1 | code2) == 0) return true;
//    if (code1 & code2) return false;
//
//    // 计算中点
//    Point mid((line.p1.x + line.p2.x) / 2, (line.p1.y + line.p2.y) / 2);
//
//    // 递归裁剪
//    Line line1(line.p1, mid);
//    Line line2(mid, line.p2);
//
//    bool accept1 = midpointClip(line1, w);
//    bool accept2 = midpointClip(line2, w);
//
//    if (accept1 && accept2) {
//        line.p1 = line1.p1;
//        line.p2 = line2.p2;
//        return true;
//    }
//    else if (accept1) {
//        line = line1;
//        return true;
//    }
//    else if (accept2) {
//        line = line2;
//        return true;
//    }
//    return false;
//}
//
//// 梁友栋-Barsky裁剪算法
//bool liangBarskyClip(Line& line, const ClipWindow& w) {
//    double dx = line.p2.x - line.p1.x;
//    double dy = line.p2.y - line.p1.y;
//    double p[4] = { -dx, dx, -dy, dy };
//    double q[4] = { line.p1.x - w.xmin, w.xmax - line.p1.x,
//                   line.p1.y - w.ymin, w.ymax - line.p1.y };
//
//    double u1 = 0.0, u2 = 1.0;
//
//    for (int i = 0; i < 4; i++) {
//        if (p[i] == 0) {
//            if (q[i] < 0) return false;
//        }
//        else {
//            double r = q[i] / p[i];
//            if (p[i] < 0) {
//                u1 = max(u1, r);
//            }
//            else {
//                u2 = min(u2, r);
//            }
//        }
//    }
//
//    if (u1 > u2) return false;
//
//    Point newP1(line.p1.x + u1 * dx, line.p1.y + u1 * dy);
//    Point newP2(line.p1.x + u2 * dx, line.p1.y + u2 * dy);
//    line.p1 = newP1;
//    line.p2 = newP2;
//    return true;
//}
//
//// 折线裁剪（使用Cohen-Sutherland）
//vector<Line> clipPolyline(const vector<Point>& polyline, const ClipWindow& w) {
//    vector<Line> clippedLines;
//
//    for (size_t i = 0; i < polyline.size() - 1; i++) {
//        Line segment(polyline[i], polyline[i + 1]);
//        if (cohenSutherlandClip(segment, w)) {
//            clippedLines.push_back(segment);
//        }
//    }
//
//    return clippedLines;
//}
//
//int main() {
//    ClipWindow window(0, 100, 0, 100);
//
//    cout << "=== 直线段裁剪算法演示 ===" << endl;
//
//    // 测试线段
//    Line testLine(Point(-10, 50), Point(110, 75));
//
//    // Cohen-Sutherland
//    Line line1 = testLine;
//    if (cohenSutherlandClip(line1, window)) {
//        cout << "Cohen-Sutherland: (" << line1.p1.x << "," << line1.p1.y
//            << ") to (" << line1.p2.x << "," << line1.p2.y << ")" << endl;
//    }
//
//    // 梁友栋-Barsky
//    Line line2 = testLine;
//    if (liangBarskyClip(line2, window)) {
//        cout << "Liang-Barsky: (" << line2.p1.x << "," << line2.p1.y
//            << ") to (" << line2.p2.x << "," << line2.p2.y << ")" << endl;
//    }
//
//    // 折线裁剪
//    cout << "\n=== 折线裁剪演示 ===" << endl;
//    vector<Point> polyline = { Point(-10, 50), Point(50, 50), Point(50, 110), Point(110, 110) };
//    vector<Line> clipped = clipPolyline(polyline, window);
//
//    cout << "裁剪后的线段数量: " << clipped.size() << endl;
//    for (size_t i = 0; i < clipped.size(); i++) {
//        cout << "线段" << i + 1 << ": (" << clipped[i].p1.x << "," << clipped[i].p1.y
//            << ") to (" << clipped[i].p2.x << "," << clipped[i].p2.y << ")" << endl;
//    }
//
//    return 0;
//}
