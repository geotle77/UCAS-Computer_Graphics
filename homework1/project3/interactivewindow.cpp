#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
int  value, color, shape, drawmod;
bool menuActive = false;
bool shapeChanged = false;
bool circleDrawn = false;
// 保存所有线和点的列表
struct Line {
    std::vector<std::pair<float, float>> points;
    int color;
    int drawmod;
};
struct Rectangle {
    std::pair<float, float> topLeft;
    std::pair<float, float> bottomRight;
    int color;
};
struct Triangle {
    std::pair<float, float> point1;
    std::pair<float, float> point2;
    std::pair<float, float> point3;
    int color;
};
struct Circle {
    std::pair<float, float> center;
    float radius;
    int color;
};

enum BackGroundColor {
    WHITE,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    INDIGO,
    VIOLET,
    BLACK,
    MAGENTA,
    CYAN
};
std::vector<Circle> circles;
std::vector<Triangle> triangles;
std::vector<Rectangle> rectangles;
std::vector<Line> lines;
std::vector<std::pair<float, float>> points;
bool leftButtonDown = false; 
float startX = 0, startY = 0;
void color_menu(int value) {
    
    color = value;
}

void shape_menu(int value) {
    shape = value;
    if (shape != value) {
        shapeChanged = true;
    }
    if (value == 2 || value == 3 || value == 4) {  // 如果选择的是矩形、三角形或圆形
        drawmod = 3;  // 设置 drawmod 的值为 3
    }
}
void changeBackgroundColor(int color) {
    switch (color) {
        case RED:
            glClearColor(1.0, 0.0, 0.0, 1.0);  // 鲜红色
            break;
        case GREEN:
            glClearColor(0.0, 1.0, 0.0, 1.0);  // 鲜绿色
            break;
        case BLUE:
            glClearColor(0.0, 0.0, 1.0, 1.0);  // 鲜蓝色
            break;
        case WHITE:
            glClearColor(1.0, 1.0, 1.0, 1.0);  // 白色
            break;
        case BLACK:
            glClearColor(0.0, 0.0, 0.0, 1.0);  // 黑色
            break;
        case YELLOW:
            glClearColor(1.0, 1.0, 0.0, 1.0);  // 黄色
            break;
        case MAGENTA:
            glClearColor(1.0, 0.0, 1.0, 1.0);  // 洋红色
            break;
        case CYAN:
            glClearColor(0.0, 1.0, 1.0, 1.0);  // 青色
            break;
    }
    glutPostRedisplay();  // 重绘窗口
}
void drawmode_menu(int value) {
     if (!leftButtonDown) {
        // 检查新的绘制模式和当前的绘制模式是否都是自由绘制或直线绘制
        if(((value==2 && drawmod==1) || (value==1 && drawmod==2)) && drawmod!=value)
            lines.clear();  // 清空线列表
        drawmod = value;  // 设置 drawmod 的值
        startX = startY = -2;  // 设置 startX 和 startY 的值为 -2
    }
    Line newLine;
    newLine.drawmod = drawmod;  // 设置新线的 drawmod 值
    lines.push_back(newLine);
}

void mymenu(int value) {
    if (value == 2) {
        lines.clear();  // 清空线列表
        points.clear();  // 清空点列表
        rectangles.clear();  // 清除所有的矩形
        triangles.clear();  // 清除所有的三角形
        circles.clear();  // 清除所有的圆形
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_STENCIL_BUFFER_BIT);
        glutPostRedisplay();  // 重绘窗口
        drawmod = 0;  // 设置 drawmod 为 0，表示不在绘制模式
    }
}

void menu() {
    menuActive = true;
    int r1 = glutCreateMenu(color_menu);
    glutAddMenuEntry("White", 0);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Orange", 2);
    glutAddMenuEntry("Yellow", 3);
    glutAddMenuEntry("Green", 4);
    glutAddMenuEntry("Blue", 5);
    glutAddMenuEntry("Indigo", 6);
    glutAddMenuEntry("Violet", 7);

    int r2 = glutCreateMenu(shape_menu);
    glutAddMenuEntry("Rect", 2);
    glutAddMenuEntry("Triangle", 3);
    glutAddMenuEntry("Circle", 4);  

    int r3 = glutCreateMenu(changeBackgroundColor);
    glutAddMenuEntry("RED",RED);
    glutAddMenuEntry("GREEN",GREEN);
    glutAddMenuEntry("BLUE",BLUE);
    glutAddMenuEntry("WHITE",WHITE);
    glutAddMenuEntry("BLACK",BLACK);
    glutAddMenuEntry("YELLOW",YELLOW);
    glutAddMenuEntry("MAGENTA",MAGENTA);
    glutAddMenuEntry("CYAN",CYAN);

    

    int r4 = glutCreateMenu(drawmode_menu);
    glutAddMenuEntry("Line Draw", 1);
    glutAddMenuEntry("Free Draw", 2);
    glutAddMenuEntry("Disable Draw", 0);  

    glutCreateMenu(mymenu);
    glutAddSubMenu("BackGroundColor",r3);
    glutAddSubMenu("Choose Draw Color", r1);
    glutAddSubMenu("Choose Shape", r2);
    glutAddMenuEntry("Clear Screen", 2);
    glutAddSubMenu("Draw Mode", r4);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void setColor(int color) {
    switch (color) {
    case 0: glColor3f(1.0, 1.0, 1.0); break;  // White
    case 1: glColor3f(1.0, 0.0, 0.0); break;  // Red
    case 2: glColor3f(1.0, 0.5, 0.0); break;  // Orange
    case 3: glColor3f(1.0, 1.0, 0.0); break;  // Yellow
    case 4: glColor3f(0.0, 1.0, 0.0); break;  // Green
    case 5: glColor3f(0.0, 0.0, 1.0); break;  // Blue
    case 6: glColor3f(0.3, 0.0, 0.5); break;  // Indigo
    case 7: glColor3f(0.5, 0.0, 0.5); break;  // Violet
    default: glColor3f(1.0, 1.0, 1.0); break;  // Default to white
    }
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& line : lines) {
    setColor(line.color);
    if (line.drawmod == 1) {  // 直线绘制模式
        for (size_t i = 0; i + 1 < line.points.size(); i += 2) {
            glBegin(GL_LINES);
            glVertex2f(line.points[i].first, line.points[i].second);
            glVertex2f(line.points[i + 1].first, line.points[i + 1].second);
            glEnd();
        }
    } else if (line.drawmod == 2) {  // 自由绘制模式
        glBegin(GL_LINE_STRIP);
        for (const auto& point : line.points) {
            glVertex2f(point.first, point.second);
        }
        glEnd();
    }
}
    for (const auto& rectangle : rectangles) {
        setColor(rectangle.color);
       if (rectangle.bottomRight.first != 0) {  // 只有当矩形的右下角已经被设置时，才绘制矩形
            glBegin(GL_POLYGON);
            glVertex2f(rectangle.topLeft.first, rectangle.topLeft.second);
            glVertex2f(rectangle.bottomRight.first, rectangle.topLeft.second);
            glVertex2f(rectangle.bottomRight.first, rectangle.bottomRight.second);
            glVertex2f(rectangle.topLeft.first, rectangle.bottomRight.second);
            glEnd();
        }
    }
    for (const auto& triangle : triangles) {
            setColor(triangle.color);
            glBegin(GL_TRIANGLES);
            glVertex2f(triangle.point1.first, triangle.point1.second);
        if (triangle.point2.first != 0) {
            glVertex2f(triangle.point2.first, triangle.point2.second);
        }
        if (triangle.point3.first != 0) {
            glVertex2f(triangle.point3.first, triangle.point3.second);
        }
            glEnd();
        }
        // 绘制圆形
    for (size_t i = 0; i < circles.size(); i++) {
    const auto& circle = circles[i];
    // 检查圆形的半径是否为零，如果是，且这个圆形不是最后一个圆形，那么跳过
    if (circle.radius == 0 && i != circles.size() - 1) continue;

    // 绘制填充的圆形
    setColor(circle.color);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159f / 180.0f;  // 将角度转换为弧度
        glVertex2f(circle.center.first + cos(degInRad) * circle.radius, circle.center.second + sin(degInRad) * circle.radius);
    }
    glEnd();

    // 绘制边框
    glColor3f(0.0, 0.0, 0.0);  // 设置边框颜色为黑色
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159f / 180.0f;  // 将角度转换为弧度
        glVertex2f(circle.center.first + cos(degInRad) * circle.radius, circle.center.second + sin(degInRad) * circle.radius);
    }
    glEnd();
    }
    glutPostRedisplay();  // 重绘窗口
    glFlush();
}
void addPoint(std::vector<Line>& lines, float fx, float fy) {
    if (!lines.empty()) {
        lines.back().points.push_back(std::make_pair(fx, fy));
        glutPostRedisplay();
    }
}
void mouse(int button, int state, int x, int y) {
    float fx = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
    float fy = 1 - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2;
     if (menuActive) {
        menuActive = false;
        return;
    }
    if (button != GLUT_LEFT_BUTTON) return;

    if (drawmod == 1) {
        if (state == GLUT_DOWN) {
            leftButtonDown = true;
            lines.push_back(Line());
            lines.back().drawmod = drawmod;  // 设置新线的 drawmod 值
            addPoint(lines, fx, fy);
            lines.back().color = color;
        } else if (state == GLUT_UP) {
            leftButtonDown = false;
            addPoint(lines, fx, fy);
        }
    } else if (drawmod == 2) {
        if (state == GLUT_DOWN) {
            leftButtonDown = true;
            lines.push_back(Line());
            lines.back().drawmod = drawmod;  // 设置新线的 drawmod 值
            addPoint(lines, fx, fy);
            lines.back().color = color;
        } else if (state == GLUT_UP) {
            leftButtonDown = false;
        }
    }else if (drawmod == 3) {
        if (shape == 2) {
            if (state == GLUT_DOWN && (rectangles.empty() || rectangles.back().bottomRight.first != 0)) {
                rectangles.push_back(Rectangle());
                rectangles.back().topLeft = std::make_pair(fx, fy);
                rectangles.back().color = color;
            } else if (state == GLUT_UP && !rectangles.empty()) {
                rectangles.back().bottomRight = std::make_pair(fx, fy);
                glutPostRedisplay();  // 重绘窗口
            }
        } else if (shape == 3) {
            if (state == GLUT_DOWN) {
                // 当左键被按下时，开始一个新的三角形或者添加点到当前的三角形
                if (triangles.empty() || triangles.back().point3.first != 0) {
                    triangles.push_back(Triangle());
                    triangles.back().point1 = std::make_pair(fx, fy);
                    triangles.back().color = color;
                    glutPostRedisplay();  // 重绘窗口
                } else if (triangles.back().point2.first == 0) {
                    triangles.back().point2 = std::make_pair(fx, fy);
                    glutPostRedisplay();  // 重绘窗口
                } else {
                    triangles.back().point3 = std::make_pair(fx, fy);
                    glutPostRedisplay();  // 重绘窗口
                }
            }
        }else if (drawmod == 3 && shape == 4) {
            if (state == GLUT_DOWN) {
        // 鼠标左键按下时，只有当 circleDrawn 为 false 时，才设置圆心并创建新的圆形
                if (!circleDrawn) {
                    circles.push_back(Circle());
                    circles.back().center = std::make_pair(fx, fy);
                    circles.back().color = color;
                    circleDrawn = true;
                }
            }  else if (state == GLUT_UP) {
                // 鼠标左键释放时，设置半径并创建新的圆形
                if (!circles.empty()) {  // 检查 circles 列表是否为空
                    float dx = fx - circles.back().center.first;
                    float dy = fy - circles.back().center.second;
                    circles.back().radius = sqrt(dx * dx + dy * dy);
                    circleDrawn = false;
                }
            }
        }
    }
}

void motion(int x, int y) {
    float fx = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
    float fy = 1 - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2;

    if (!leftButtonDown) return;

    if (drawmod == 2) {
        lines.back().drawmod = drawmod;  // 设置新线的 drawmod 值
        addPoint(lines, fx, fy);
    } else if (drawmod == 3) {
        if (shape == 2) {
            lines.back().drawmod = drawmod;  // 设置新线的 drawmod 值
            addPoint(lines, fx, fy);
        } else if (shape == 4 && !menuActive) {
           if (!circles.empty() && circles.back().radius == 0) {
            float dx = fx - circles.back().center.first;
            float dy = fy - circles.back().center.second;
            circles.back().radius = sqrt(dx * dx + dy * dy);
            glutPostRedisplay();  // 重绘窗口
            circleDrawn = false;  // 设置 circleDrawn 为 false
            }
        }
    }
}
void init() {
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(500, 500);

    glutCreateWindow("Interactive Window");
    menu();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);  // 注册鼠标移动的回调函数
    glutMainLoop();
    return 0;
}