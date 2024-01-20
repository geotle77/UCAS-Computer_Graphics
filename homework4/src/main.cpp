#include "../include/header.h"
#include "../include/material.h"
#include <GL/glut.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"
#define WIDTH 800
#define HEIGHT 800
#define PI 3.14159265358979323846f
int mouseState = GLUT_UP;
int mouseButton = GLUT_LEFT_BUTTON;
#define SCALE_GAP 0.1
#define LOWER_BOUND SCALE_GAP
#define UPPER_BOUND 3
#define ROTATE_GAP 5.0f
extern double scale;
double x_rotate = 0.0;
double y_rotate = 0.0;

Scene mainScene;

using std::vector;

std::string dest_dir = "./outpic/";
std::string texture_dir = "./input/";
ImageTexture* tex[5];
vector<float> vertices;
GLubyte colorData[WIDTH*HEIGHT*3];

void loadPixel(float x,float y,glm::vec3 color){
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(.0f);
    vertices.push_back(color.x);
    vertices.push_back(color.y);
    vertices.push_back(color.z);
}
void makePixel(){
    for(size_t i=0;i<vertices.size();i+=6){
        int x = static_cast<int>((vertices[i] + 1) / 2 * WIDTH);
        int y = static_cast<int>((vertices[i + 1] + 1) / 2 * HEIGHT);
        int index = (y * WIDTH + x) * 3;
        colorData[index] = static_cast<GLubyte>(vertices[i + 3] * 255);
        colorData[index + 1] = static_cast<GLubyte>(vertices[i + 4] * 255);
        colorData[index + 2] = static_cast<GLubyte>(vertices[i + 5] * 255);
    }
}
void flipImage(GLubyte* pixels, int width, int height, int channels) {
    int stride = width * channels;
    GLubyte* row = new GLubyte[stride];
    for (int y = 0; y < height / 2; y++) {
        memcpy(row, pixels + y * stride, stride);
        memcpy(pixels + y * stride, pixels + (height - 1 - y) * stride, stride);
        memcpy(pixels + (height - 1 - y) * stride, row, stride);
    }
    delete[] row;
}

void saveimage(void)
{
    static int index =0;
    // 获取像素数据
    GLubyte* pixels = new GLubyte[3 * WIDTH * HEIGHT];
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // 翻转像素数据
    flipImage(pixels, WIDTH, HEIGHT, 3);

    // 将像素数据保存为PNG图像
    std::string filename = dest_dir + "output_" + std::to_string(index) + ".png";
    stbi_write_png(filename.c_str(), WIDTH, HEIGHT, 3, pixels, WIDTH * 3);
    index++;
    // 释放像素数据
    delete[] pixels;
}

void DrawScene(void){
    glClear(GL_COLOR_BUFFER_BIT);
    renderScene(mainScene,2*WIDTH*scale, 2*HEIGHT*scale,loadPixel);
    makePixel();
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, colorData);
    glFlush();
    glutSwapBuffers();
    saveimage();
}

void makegif(void)
{
    int totalFrames = 10;
    // 初始化图像路径向量
    std::vector<std::string> imagePaths;
    float y_rotate = 0.0f;
    // 循环渲染每一帧
    for (int frame = 0; frame < totalFrames; ++frame) {
        y_rotate += ROTATE_GAP;
        float radian = y_rotate * (std::atan(1)*4 / 180);
        mainScene.camera.rotate(radian);
        // 渲染并保存图像
        DrawScene();

        // 将图像路径添加到向量中
        std::string imagePath = dest_dir + "output_" + std::to_string(frame) + ".png";
        imagePaths.push_back(imagePath);
    }
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

void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) 
        y_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_RIGHT) 
        y_rotate += ROTATE_GAP;
    float radian = y_rotate * (std::atan(1)*4 / 180);
    mainScene.camera.rotate(radian);
    DrawScene();
}

int main(int argc, char **argv){
    // for(int i=0; i<5; i++){
    //     int nx, ny, nn;
    //     std::stringstream sm;
    //     sm << texture_dir << i << ".jpg";
    //     std::string img_name;
    //     sm >> img_name;
    //     unsigned char *img = stbi_load(img_name.data(), &nx, &ny, &nn, 0);
    //     tex[i] = new ImageTexture(img, nx, ny);
    // }

    //Scene mainScene;
    mainScene.setBackground(glm::vec3(.2f, .6f, .8f));
    mainScene.setLight(new Light(glm::vec3(.5f, 1.0f, .5f), glm::vec3(1.5f, 1.5f, 1.5f)));
    mainScene.addSphere(new Sphere(glm::vec3(.2f, .0f, .0f), .1f, roughMaterial(green)));
    mainScene.addSphere(new Sphere(glm::vec3(-.2f, .0f, .0f), .1f, roughMaterial(red)));
    mainScene.addCube(new Cube(glm::vec3(.0f, .0f, -.2f), .1f, roughMaterial(violet)));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, .0f, .2f), .1f, defaultRefractive));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, .2f, .0f), .1f, metalReflective));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, -200.15f, .0f), 200.0f, defaultReflective));
    mainScene.addCube(new Cube(glm::vec3(.0f, .0f, -.8f), .1f, defaultRefractive));
    mainScene.addCube(new Cube(glm::vec3(.4f, .0f, .0f), .1f, metalReflective));

    mainScene.rotateCamera(.125*PI);
    vertices.clear();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Ray Tracing");
    if (argc > 1) {
        // 获取第一个参数
        std::string arg1 = argv[1];

        // 如果参数是"YES"，则调用makegif函数
        if (arg1 == "YES") {
            makegif();
        }
    }
    glutDisplayFunc(DrawScene);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(mouseClick);
    glutTimerFunc(100, timer, 0); // 设置第一次调用定时器的时间
    glutMainLoop();
    return 0;
}