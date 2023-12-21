#include<GL/glut.h>
#include "mesh.h"
#define SCALE_GAP 0.05
#define LOWER_BOUND SCALE_GAP
#define UPPER_BOUND 3
#define ROTATE_GAP 5.0f
#define USE_MYGLCUBE 1

int kind = 0;//材质种类
int mouseState = GLUT_UP;
int mouseButton = GLUT_LEFT_BUTTON;
using namespace std;
float x_rotate = 0.0;
float y_rotate = 0.0;
float scale = 1;


void myGLCube(){

}
void  GLCube(){

}

void InitScene(){
    model.loadFile("bunny.obj");
    Nomalize();

    CalNormals();
    glClearColor(0.0, 0.0, 0.0, 1.0);//white background
   
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
}

void DrawScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();//重置当前的模型观察矩阵
    
    // 定义材质
    SetMaterier(kind);

    GLfloat light_position[] = {  -1.0f, 1.0f, -1.0f, 1.0f };//光源位置，第四个参数为0表示光源位于无穷远处
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    GLfloat light_intensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };//光源强度
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    glRotatef(x_rotate, 1.0f, 0.0f, 0.0f);
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    //设置相机位置和旋转角度
    glTranslatef(-0.4f, -0.4f, -1.0f);
    glScalef(scale, scale, scale);

    //绘制物质
    #if USE_MYGLCUBE
    myGLCube();
    #else
    GLCube();
    #endif
    glFlush();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            kind = 0;
            break;
        case 's':
            kind = 1;
            break;
        case 'a':
            kind = 2;
            break;
        case 'd':
            kind = 3;
            break;
        default:
            return;
    }
    DrawScene();
}

void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) 
        x_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_DOWN) 
        x_rotate += ROTATE_GAP;
    else if (key == GLUT_KEY_LEFT) 
        y_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_RIGHT) 
        y_rotate += ROTATE_GAP;
    DrawScene();
}

// 鼠标左击缩小，右击放大，每次操作步幅为SCALE_GAP
void timer(int value) { // 定时器函数，按下鼠标后每隔100ms调用一次即可实现连续缩放
    if (mouseState == GLUT_DOWN) {
        if (mouseButton == GLUT_RIGHT_BUTTON) {
            if(scale>=UPPER_BOUND)
                printf("Reach the scale upper bound!\n");
            else
                scale += SCALE_GAP;
        } else if (mouseButton == GLUT_LEFT_BUTTON) {
            if(scale<=LOWER_BOUND)    // 避免scale减小为非正值
                printf("Reach the scale lower bound!\n");
            else
                scale -= SCALE_GAP;
        }
        DrawScene();
    }
    glutTimerFunc(100, timer, 0); // 设置下一次调用定时器的时间
}

void mouseClick(int button, int state, int x, int y) {
    mouseState = state;
    if (state == GLUT_DOWN) {
        mouseButton = button;
    }
}
void SetMaterier(int kind)
{
    GLfloat brass_ambient[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
    GLfloat brass_diffuse[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
    GLfloat brass_specular[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
    GLfloat brass_shininess = 60.8974f;

    GLfloat white_ambient[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
    GLfloat white_diffuse[] = { 1.0f, 0.829f, 0.829f, 1.0f };
    GLfloat white_specular[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
    GLfloat white_shininess = 11.264f;

    GLfloat red_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat red_diffuse[] = { 0.5f, 0.0f, 0.0f, 1.0f };
    GLfloat red_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
    GLfloat red_shininess = 0.25f;

    GLfloat blue_ambient[] = { 0.0215f, 0.0215f, 0.1745f, 0.55f };
    GLfloat blue_diffuse[] = { 0.07568f, 0.07568f, 0.61424f, 0.55f };
    GLfloat blue_specular[] = { 0.633f, 0.633f, 0.727811f, 0.55f };
    GLfloat blue_shininess = 76.8f;
    switch (kind)
    {
        case 0:
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, brass_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, brass_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, brass_specular);
                glMaterialf(GL_FRONT, GL_SHININESS, brass_shininess);
            }
            break;
        case 1:
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, white_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, white_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, white_specular);
                glMaterialf(GL_FRONT, GL_SHININESS, white_shininess);
            }
            break;
        case 2:
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, red_specular);
                glMaterialf(GL_FRONT, GL_SHININESS, red_shininess);
            }
            break;
        case 3:
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, blue_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
                glMaterialf(GL_FRONT, GL_SHININESS, blue_shininess);
            }
            break;

    }

}