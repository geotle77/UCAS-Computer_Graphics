#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <string.h>

#define GREY    190.0/255, 190.0/255, 190.0/255
#define DIMGREY 135.0/255, 135.0/255, 135.0/255
#define GOLDEN  255.0/255, 215.0/255, 89.0/255
#define RED     205.0/255, 79.0/255,  57.0/255
#define BLUE    145.0/255, 206.0/255, 222.0/255

void drawLineBresenham(float x1, float y1, float x2, float y2) {
    float dx = abs(x2 - x1), sx = x1 < x2 ? 0.001 : -0.001;
    float dy = -abs(y2 - y1), sy = y1 < y2 ? 0.001 : -0.001; 
    float err = dx + dy, e2;
    float oldX = x1, oldY = y1; // 记录上一个点的坐标
    while(true) {
        if (fabs(x1 - x2) < 0.001 && fabs(y1 - y2) < 0.001) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
        // 使用GL_LINES模式绘制直线
        glBegin(GL_LINES);
        glVertex2f(oldX, oldY);
        glVertex2f(x1, y1);
        glEnd();
        oldX = x1; oldY = y1; // 更新上一个点的坐标
    }
}

void drawRectangle(float x1, float y1, float x2, float y2,int mode) {
    glBegin(mode);
    glVertex2f(x1, y1); // 左下角
    glVertex2f(x2, y1); // 右下角
    glVertex2f(x2, y2); // 右上角
    glVertex2f(x1, y2); // 左上角
    glEnd();
}
//绘制圆形线框
void drawArc(float cx, float cy, float r, float start_angle, float arc_angle,int mode) {
    if(mode==GL_LINE_LOOP)
    {
        glBegin(GL_LINE_STRIP);
    }
    for(int i = 0; i <= arc_angle; ++i) {
        float angle = (start_angle + i) * M_PI / 180.0f; // 将角度转换为弧度
        float x = r * cos(angle) + cx;
        float y = r * sin(angle) + cy;
        glVertex2f(x, y);
    }
    if(mode==GL_LINE_LOOP)
    {
        glEnd();
    }
}
void drawRoundedRectangle(float x, float y, float width, float height, float radius,int mode) {
    // 计算四个圆心的位置
    float cx1 = x + radius, cy1 = y + radius;
    float cx2 = x + width - radius, cy2 = y + radius;
    float cx3 = x + width - radius, cy3 = y + height - radius;
    float cx4 = x + radius, cy4 = y + height - radius;

    // 绘制四个圆弧
    if(mode==GL_POLYGON)
    {
        glBegin(GL_POLYGON);
    }
    drawArc(cx1, cy1, radius, 180, 90,mode);
    drawArc(cx2, cy2, radius, 270, 90,mode);
    drawArc(cx3, cy3, radius, 0, 90,mode);
    drawArc(cx4, cy4, radius, 90, 90,mode);
    
    // 绘制四个矩形边
    if(mode==GL_LINE_LOOP)
    {
        glBegin(GL_LINES);
    }
    glVertex2f(cx1, cy1 - radius);
    glVertex2f(cx2, cy2 - radius);

    glVertex2f(cx2 + radius, cy2);
    glVertex2f(cx3 + radius, cy3);

    glVertex2f(cx3, cy3 + radius);
    glVertex2f(cx4, cy4 + radius);

    glVertex2f(cx4 - radius, cy4);
    glVertex2f(cx1 - radius, cy1);
    glEnd();
}
void drawSymmetricPolygon(float centerX, float centerY, int numSides, float radius) {
    if (numSides < 3) return; // 至少需要3个边才能形成一个多边形

    float angleStep = 2.0 * M_PI / numSides; // 每个角的角度
    float angle = M_PI / 2.0; // 从正上方开始绘制

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSides; ++i) {
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
        angle += angleStep;
    }
    glEnd();
}
//绘制星形线框，注意这里的numSides必须是奇数！
void drawStar(float centerX, float centerY, float radius, int numSides) {
    float angleStep = 2.0 * M_PI / numSides; // 每个角的角度
    float offsetAngle = M_PI / 2.0; // 偏移角度，使一个角位于最上方

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY); // 中心点
    for (int i = 0; i <= numSides; ++i) {
        float angle = offsetAngle + ((i * 2) % numSides) * angleStep; // 跳过一个顶点
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}
void drawCircle(float centerX, float centerY, float radius,int mode) {
    int numSegments = 100; // 分割数，用于近似圆
    float angleStep = 2.0 * M_PI / numSegments; // 每个分割的角度

    glBegin(mode);
    for (int i = 0; i < numSegments; ++i) {
        float angle = i * angleStep;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}
void drawTrapezoid(float centerX, float centerY, float topLength, float bottomLength, float height,int mode) {
    float topStartX = centerX - topLength / 2.0;
    float topEndX = centerX + topLength / 2.0;

    float bottomStartX = centerX - bottomLength / 2.0;
    float bottomEndX = centerX + bottomLength / 2.0;

    float topY = centerY + height / 2.0;
    float bottomY = centerY - height / 2.0;

    glBegin(mode);
    glVertex2f(topStartX, topY);
    glVertex2f(topEndX, topY);
    glVertex2f(bottomEndX, bottomY);
    glVertex2f(bottomStartX, bottomY);
    glEnd();
}
void displaychar(const char *str,float x,float y)
{
    glColor3f(0,0,0); // 设置文本颜色
    glRasterPos2f(x, y); // 设置文本位置
    int len = (int) strlen(str);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
    glutSwapBuffers();
}
void display() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // 设置背景颜色为白色
    glClear(GL_COLOR_BUFFER_BIT);
   
    glColor3f(GREY); 
    glLineWidth(3.0f);
    drawRectangle(-0.8, -0.55, 0.8, 0.55,GL_POLYGON); // 绘制外框
    glColor3f(0.0, 0.0, 0.0); // 设置绘图颜色为黑色
    drawRectangle(-0.8, -0.55, 0.8, 0.55,GL_LINE_LOOP); // 绘制屏幕
    glColor3f(BLUE);
    drawRoundedRectangle(-0.75, -0.5, 1.5, 1.0, 0.1,GL_POLYGON);//绘制屏幕
    glColor3f(0.0, 0.0, 0.0); // 设置绘图颜色为黑色
    drawRoundedRectangle(-0.75, -0.5, 1.5, 1.0, 0.1,GL_LINE_LOOP);//绘制屏幕
    glColor3f(RED);
    drawCircle(0.0, 0.0, 0.2,GL_POLYGON);
    glColor3f(0.0, 0.0, 0.0); // 设置绘图颜色为黑色
    drawCircle(0.0, 0.0, 0.2,GL_LINE_LOOP);
    glColor3f(GOLDEN);
    drawStar(0.0, 0.0, 0.2,7);
    //drawSymmetricPolygon(0.0, 0.0, 5, 0.2);
    //绘制电脑底垫
    glColor3f(DIMGREY);
    glBegin(GL_POLYGON);
    glVertex2f(-0.15, -0.65);
    glVertex2f(-0.40, -0.65);
    drawArc(-0.40, -0.70, 0.05, 90, 90,GL_POLYGON);
    glVertex2f(-0.45, -0.70);
    glVertex2f(-0.45, -0.83);
    drawArc(-0.40, -0.83, 0.05, 180, 90,GL_POLYGON);
    glVertex2f(-0.40, -0.88);
    glVertex2f(0.40, -0.88);
    drawArc(0.40, -0.83, 0.05, 270, 90,GL_POLYGON);
    glVertex2f(0.45, -0.83);
    glVertex2f(0.45, -0.70);
    drawArc(0.40, -0.70, 0.05, 0, 90,GL_POLYGON);
    glVertex2f(0.40, -0.65);
    glVertex2f(0.15, -0.65);
    glEnd();
    glColor3f(0.0, 0.0, 0.0); // 设置绘图颜色为黑色
    //绘制电脑底垫框线
    drawLineBresenham(-0.15, -0.65, -0.40, -0.65);
    drawLineBresenham(-0.45, -0.70, -0.45, -0.83);
    drawLineBresenham(-0.40, -0.88, 0.40, -0.88);
    drawLineBresenham(0.40, -0.65, 0.15, -0.65); 
    drawLineBresenham(0.45, -0.83, 0.45, -0.70);
    drawArc(-0.40, -0.70, 0.05, 90, 90,GL_LINE_LOOP);
    drawArc(-0.40, -0.83, 0.05, 180, 90,GL_LINE_LOOP);
    drawArc(0.40, -0.83, 0.05, 270, 90,GL_LINE_LOOP);
    drawArc(0.40, -0.70, 0.05, 0, 90,GL_LINE_LOOP);
    
    //绘制电脑支架
    glColor3f(DIMGREY); 
    drawTrapezoid(0.0, -0.65, 0.20, 0.4, 0.2,GL_POLYGON);
    //绘制电脑支架框线
    glColor3f(0.0, 0.0, 0.0); // 设置绘图颜色为黑色
    drawTrapezoid(0.0, -0.65, 0.20, 0.4, 0.2,GL_LINE_LOOP);
    
    const char *str="By MDF";
    displaychar(str,0.3,0.8);
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Draw computer");
    glutInitWindowSize(320, 320);
    glutInitWindowPosition(50, 50);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}