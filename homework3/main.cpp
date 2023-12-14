#include<GL/glut.h>
#include <vector>
#include <string>
#include <cmath>
#include <Eigen/Dense>
#include "mesh.h"
extern Mesh Dragon;
void GLcube(){

}
void InitScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//设置背景颜色
    glShadeModel(GL_SMOOTH);//设置着色模式
    glEnable(GL_DEPTH_TEST);//开启深度测试
    Dragon.loadmodel("./input_model/dragon.obj");
    Dragon.calNormal();
    Dragon.simplify(10337);
    Dragon.export2Obj(Dragon.vertices, Dragon.faces);
}
void DrawScene (){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓存和深度缓存
    glMatrixMode(GL_MODELVIEW);//设置当前矩阵模式为模型视图
    glLoadIdentity();//初始化模型视图矩阵
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, -1.0, 0, 1, 0);//设置视点
    glColor3f(1.0f, 1.0f, 1.0f);//设置颜色
    GLcube();
    glutSwapBuffers();//交换缓冲区
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutInitWindowSize(600, 600);//设置窗口大小
    //glutInitWindowPosition(100, 100);//设置窗口位置
    //glutCreateWindow("OpenGL");//创建窗口
    InitScene();
    //glutDisplayFunc(DrawScene);//显示回调函数
    //glutMainLoop();//进入主循环
    return 0;
}

