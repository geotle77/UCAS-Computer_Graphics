#ifndef SHOWMODEL_H
#define SHOWMODEL_H
#include <GL/glut.h>
#include "mesh.h"
#define SCALE_GAP 0.05
#define LOWER_BOUND SCALE_GAP
#define UPPER_BOUND 3
#define ROTATE_GAP 5.0f
#define USE_FACE_NORMAL 0

extern int kind;
extern int mouseState;
extern int mouseButton;
extern double x_rotate;
extern double y_rotate;
extern double scale;
extern std::vector<Face> faces;
extern std::vector<Vertex> vertices;
extern std::vector<Normal> normals;
extern std::vector<Normal> vertexnormals;

void  F_GLCube();//面法线
void  V_GLCube();//顶点法线
void InitScene();
void DrawScene();
void keyboard(unsigned char key, int x, int y);
void SpecialKeys(int key, int x, int y) ;
void timer(int value);
void mouseClick(int button, int state, int x, int y) ;
void SetMaterier(int kind);
#endif