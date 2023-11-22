#include <GL/glut.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

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

struct Vertex{
    float x, y, z;
    Vertex operator-(const Vertex& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }//定义重载减法
};
struct Face {
    int v1, v2, v3;  // Vertex 
    int vn1, vn2, vn3;  // Normal 
};
struct Normal{
    float nx, ny, nz;
};
class Model{
 public:
    vector<Vertex> vertices;
    vector<Face> faces;
    vector<Normal> normals;

    void loadFile(string filename);
};
Model model;

struct CalculatedNormal {
    float nx, ny, nz;
};
vector<CalculatedNormal> calculatedNormals;

void InitScene();
void DrawScene();
void GLCube();
void myGLCube();
void Nomalize();

void Model::loadFile(string filename){
    ifstream fin(filename.c_str());
    string line;
    while(getline(fin, line)){
        if (line.substr(0, 2) == "v ") {
            istringstream s(line.substr(2));
            Vertex v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            vertices.push_back(v);
        } else if (line.substr(0, 2) == "f ") {
            istringstream s(line.substr(2));
            Face f;
            char slash;  // To ignore the '/' characters
            s >> f.v1 >> slash >> slash >> f.vn1;
            s >> f.v2 >> slash >> slash >> f.vn2;
            s >> f.v3 >> slash >> slash >> f.vn3;
            faces.push_back(f);
        }else if (line.substr(0, 3) == "vn ") {
            istringstream s(line.substr(3));
            Normal n;
            s >> n.nx;
            s >> n.ny;
            s >> n.nz;
            normals.push_back(n);
        }
    }
}

Normal cross(const Vertex& v1, const Vertex& v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}
CalculatedNormal normalize(const CalculatedNormal& n) {
    float length = sqrt(n.nx * n.nx + n.ny * n.ny + n.nz * n.nz);
    return {n.nx / length, n.ny / length, n.nz / length};
}
float length(const Normal& n) {
    return sqrt(n.nx * n.nx + n.ny * n.ny + n.nz * n.nz);
}
void CalNormals() {
    // 初始化 calculatedNormals
    calculatedNormals.resize(model.vertices.size());

    // 遍历每个面，计算面的法线，然后将这个法线乘以面的面积加到面的每个顶点的法线上
    for (Face& f : model.faces) {
        Vertex v1 = model.vertices[f.v1 - 1];
        Vertex v2 = model.vertices[f.v2 - 1];
        Vertex v3 = model.vertices[f.v3 - 1];

        Normal normal = cross(v2 - v1, v3 - v1);
        float area = length(normal);

        calculatedNormals[f.v1 - 1].nx += normal.nx * area;
        calculatedNormals[f.v1 - 1].ny += normal.ny * area;
        calculatedNormals[f.v1 - 1].nz += normal.nz * area;

        calculatedNormals[f.v2 - 1].nx += normal.nx * area;
        calculatedNormals[f.v2 - 1].ny += normal.ny * area;
        calculatedNormals[f.v2 - 1].nz += normal.nz * area;

        calculatedNormals[f.v3 - 1].nx += normal.nx * area;
        calculatedNormals[f.v3 - 1].ny += normal.ny * area;
        calculatedNormals[f.v3 - 1].nz += normal.nz * area;
    }

    // 遍历每个顶点，将每个顶点的法线归一化
    for (CalculatedNormal& n : calculatedNormals) {
        n = normalize(n);
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

void Nomalize(){
    float minX = model.vertices[0].x, maxX = model.vertices[0].x;
    float minY = model.vertices[0].y, maxY = model.vertices[0].y;
    float minZ = model.vertices[0].z, maxZ = model.vertices[0].z;

    for (const auto& vertex : model.vertices) {
        minX = std::min(minX, vertex.x);
        maxX = std::max(maxX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxY = std::max(maxY, vertex.y);
        minZ = std::min(minZ, vertex.z);
        maxZ = std::max(maxZ, vertex.z);
    }

    // 计算坐标范围
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;

    // 将每个坐标归一化到[0,1]的范围内
    for (auto& vertex : model.vertices) {
        vertex.x = (vertex.x - minX) / rangeX;
        vertex.y = (vertex.y - minY) / rangeY;
        vertex.z = (vertex.z - minZ) / rangeZ;
    }
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

void myGLCube(){
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model.faces.size(); i++) {
        Face f = model.faces[i];
        CalculatedNormal n1 = calculatedNormals[f.v1 - 1];
        CalculatedNormal n2 = calculatedNormals[f.v2 - 1];
        CalculatedNormal n3 = calculatedNormals[f.v3 - 1];
        glNormal3f(n1.nx, n1.ny, n1.nz);
        glVertex3f(model.vertices[f.v1 - 1].x, model.vertices[f.v1 - 1].y, model.vertices[f.v1 - 1].z);
        glNormal3f(n2.nx, n2.ny, n2.nz);
        glVertex3f(model.vertices[f.v2 - 1].x, model.vertices[f.v2 - 1].y, model.vertices[f.v2 - 1].z);
        glNormal3f(n3.nx, n3.ny, n3.nz);
        glVertex3f(model.vertices[f.v3 - 1].x, model.vertices[f.v3 - 1].y, model.vertices[f.v3 - 1].z);
    }
    glEnd();
}
void  GLCube(){
    glBegin(GL_TRIANGLES); 
    for (int i = 0; i < model.faces.size(); i++) {
        Face f = model.faces[i];
        Normal n1 = model.normals[f.vn1 - 1];
        Normal n2 = model.normals[f.vn2 - 1];
        Normal n3 = model.normals[f.vn3 - 1];
        glNormal3f(n1.nx, n1.ny, n1.nz);
        glVertex3f(model.vertices[f.v1 - 1].x, model.vertices[f.v1 - 1].y, model.vertices[f.v1 - 1].z);
        glNormal3f(n2.nx, n2.ny, n2.nz);
        glVertex3f(model.vertices[f.v2 - 1].x, model.vertices[f.v2 - 1].y, model.vertices[f.v2 - 1].z);
        glNormal3f(n3.nx, n3.ny, n3.nz);
        glVertex3f(model.vertices[f.v3 - 1].x, model.vertices[f.v3 - 1].y, model.vertices[f.v3 - 1].z);
    }
    glEnd();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);//设置窗口大小
    glutInitWindowPosition(100, 100);//设置窗口位置
    glutCreateWindow("OpenGL");//创建窗口
    InitScene();
    glutDisplayFunc(DrawScene);//显示回调函数
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutTimerFunc(100, timer, 0); // 设置第一次调用定时器的时间
    glutMainLoop();//进入主循环
    return 0;
}