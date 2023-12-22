#include<GL/glut.h>
#include "mesh.h"
#include "showmodel.h"
int kind = 0;//材质种类
int mouseState = GLUT_UP;
int mouseButton = GLUT_LEFT_BUTTON;
using namespace std;
double x_rotate = 0.0;
double y_rotate = 0.0;
double scale = 1;
std::vector<Face> faces;
std::vector<Vertex> vertices;
std::vector<Normal> normals;
std::vector<Normal> vertexnormals;
Normal normalize(const Normal& n) {
    float length = sqrt(n.nx * n.nx + n.ny * n.ny + n.nz * n.nz);
    return {n.nx / length, n.ny / length, n.nz / length};
}
float length(const Normal& n) {
    return sqrt(n.nx * n.nx + n.ny * n.ny + n.nz * n.nz);
}
Normal cross(const Vertex& v1, const Vertex& v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}
void CalNormals() {
    // 初始化 calculatedNormals
    vertexnormals.resize(vertices.size());

    // 遍历每个面，计算面的法线，然后将这个法线乘以面的面积加到面的每个顶点的法线上
    for (Face& f : faces) {
        Vertex v1 = vertices[f.vertex[0]];
        Vertex v2 = vertices[f.vertex[1]];
        Vertex v3 = vertices[f.vertex[2]];

        Normal normal = cross(v2 - v1, v3 - v1);
        float area = length(normal);

        vertexnormals[f.vertex[0]].nx += normal.nx * area;
        vertexnormals[f.vertex[0]].ny += normal.ny * area;
        vertexnormals[f.vertex[0]].nz += normal.nz * area;
        vertexnormals[f.vertex[1]].nx += normal.nx * area;
        vertexnormals[f.vertex[1]].ny += normal.ny * area;
        vertexnormals[f.vertex[1]].nz += normal.nz * area;
        vertexnormals[f.vertex[2]].nx += normal.nx * area;
        vertexnormals[f.vertex[2]].ny += normal.ny * area;
        vertexnormals[f.vertex[2]].nz += normal.nz * area;
    }

    // 遍历每个顶点，将每个顶点的法线归一化
    for (Normal& n : vertexnormals) {
        n = normalize(n);
    }
}

void F_GLCube(){
    glBegin(GL_TRIANGLES);
    for(int i=0;i<faces.size();i++){
        Normal normal1 = vertexnormals[faces[i].vertex[0]];
        Normal normal2 = vertexnormals[faces[i].vertex[1]];
        Normal normal3 = vertexnormals[faces[i].vertex[2]];
        Normal mean_normal;
        mean_normal.nx = (normal1.nx+normal2.nx+normal3.nx)/3;
        mean_normal.ny = (normal1.ny+normal2.ny+normal3.ny)/3;
        mean_normal.nz = (normal1.nz+normal2.nz+normal3.nz)/3;
        for(int j=0;j<3;j++){
            glNormal3d(mean_normal.nx, mean_normal.ny, mean_normal.nz);
            glVertex3d(vertices[faces[i].vertex[j]].x, vertices[faces[i].vertex[j]].y, vertices[faces[i].vertex[j]].z);
        }
    }
    glEnd();
}
void  V_GLCube(){
    glBegin(GL_TRIANGLES);
    for(int i=0;i<faces.size();i++){
        Normal normal1 = vertexnormals[faces[i].normal[0]];
        Normal normal2 = vertexnormals[faces[i].normal[1]];
        Normal normal3 = vertexnormals[faces[i].normal[2]];
        glNormal3d(normal1.nx, normal1.ny, normal1.nz);
        glVertex3d(vertices[faces[i].vertex[0]].x, vertices[faces[i].vertex[0]].y, vertices[faces[i].vertex[0]].z);
        glNormal3d(normal2.nx, normal2.ny, normal2.nz);
        glVertex3d(vertices[faces[i].vertex[1]].x, vertices[faces[i].vertex[1]].y, vertices[faces[i].vertex[1]].z);
        glNormal3d(normal3.nx, normal3.ny, normal3.nz);
        glVertex3d(vertices[faces[i].vertex[2]].x, vertices[faces[i].vertex[2]].y, vertices[faces[i].vertex[2]].z);
    }
    glEnd();
}

void InitScene(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    CalNormals();
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

    GLfloat glfLight[] = { -4.0f, 4.0f, -4.0f, 0.0f };;//光源位置，第四个参数为0表示光源位于无穷远处
    glLightfv(GL_LIGHT0, GL_POSITION, glfLight);
    GLfloat light_intensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };//光源强度
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    glRotatef(x_rotate, 1.0f, 0.0f, 0.0f);
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f);
    //设置相机位置和旋转角度
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(scale, scale, scale);

    //绘制物质
    #if USE_FACE_NORMAL
    F_GLCube();
    #else
    V_GLCube();
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
    GLfloat glfMatAmbient[] = { 0.135f,0.2225f,0.1575f,0.95f };
    GLfloat glfMatDiffuse[] = { 0.54f,0.89f,0.63f,0.95f };
    GLfloat glfMatSpecular[] = { 0.316228f,0.316228f,0.316228f,0.95f };
    GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
    GLfloat fShininess = 12.800f;

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
                glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
                glMaterialf(GL_FRONT, GL_SHININESS, fShininess);
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