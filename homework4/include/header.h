#ifndef HEADER
#define HEADER 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#define ROUGH 0
#define REFLECTIVE 1
#define REFRACTIVE 2

#define TRACE_MIN_T 0.0001f
#define TRACE_MAX_DEPTH 5
#define EPSILON 0.00001f

//结构体：光线
struct Ray{
    glm::vec3 ori;
    glm::vec3 dir;
    
    Ray(glm::vec3 ori, glm::vec3 dir) : ori(ori), dir(dir) {}
};


//物体材质
struct Material{
    int type;
    
    //Phong模型参数
    //float ambientStrength;
    glm::vec3 objColor;
    //float specularStrength;
    float shininess;
    
    //反射计算参数：基础折射率
    //glm::vec3 R0;
    
    //折射计算参数：材料折射率
    float n;
    
    Material(int type) : type(type) {}
    // 粗糙材质
    Material(glm::vec3 color, float shininess):type(ROUGH),
    objColor(color), shininess(shininess){}//, R0(glm::vec3(.0f, .0f, .0f)){}
    // 反射材质
    Material(glm::vec3 color) : type(REFLECTIVE), objColor(color){}//, R0(R0) {}
    // 折射材质
    Material(float n) : type(REFRACTIVE), n(n) {}//float r0 = pow((n - 1) / (n + 1), 2); R0 = glm::vec3(r0, r0, r0);}
};

//场景光源
struct Light{
    glm::vec3 dir;
    glm::vec3 color;
    Light(glm::vec3 dir, glm::vec3 color) : dir(glm::normalize(dir)), color(color) {}
};

// 相机
class Camera{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    Camera(glm::vec3 position = glm::vec3(.0f, .0f, 1.0f))
    : front(-glm::normalize(position)), position(position), worldUp(glm::vec3(.0f, 1.0f, .0f)) {updateCameraVectors();}
        
    void rotate(float radian);
    
    Ray getRay(float viewx, float viewy, float fov);
    
private:
    void updateCameraVectors(void);
};

class ImageTexture
{
public:
    unsigned char* data;
    int nx, ny;

    ImageTexture(){}
    ImageTexture(unsigned char* pixels, int A, int B) : data(pixels), nx(A), ny(B) {}

    //输入u和v，输出对应图片像素的rgb值
    virtual glm::vec3 value(float u, float v) const
    {
        int i = int((u)* nx);//求出像素索引
        int j = int((1 - v)*ny - 0.001f);
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > nx - 1) i = nx - 1;
        if (j > ny - 1) j = ny - 1;
        float r = int(data[3 * i + 3 * nx*j]) / 255.0f;
        float g = int(data[3 * i + 3 * nx*j + 1]) / 255.0f;
        float b = int(data[3 * i + 3 * nx*j + 2]) / 255.0f;
        return glm::vec3(r, g, b);
    }
};
class Shape {
public:
    Material *material;
    ImageTexture *tex;

    Shape(Material *material) : material(material), tex(nullptr) {}

    virtual ~Shape() {}

    virtual float Intersect(const Ray &ray, glm::vec3 *hitNorm, glm::vec3 *hitPos) = 0;
};

// 球体
class Sphere : public Shape {
    glm::vec3 center;
    float radius;

public:
    Sphere(glm::vec3 center, float radius, Material *material)
        : center(center), radius(radius), Shape(material) {}
    float Intersect(const Ray &ray, glm::vec3 *hitNorm, glm::vec3 *hitPos) override;
};

class Cube : public Shape {
    glm::vec3 center;
    float edgeLength;

public:
    Cube(glm::vec3 center, float edgeLength, Material *material)
        : center(center), edgeLength(edgeLength), Shape(material) {}
    float Intersect(const Ray &ray, glm::vec3 *hitNorm, glm::vec3 *hitPos) override;
};
//光线与球体碰撞
//参数分别为：球体、光线、碰撞处法向量、碰撞位置，返回值为碰撞位置的t值（光线表达式p=e+td）
//float Intersect(const Sphere &sphere, const Ray &ray, glm::vec3 *hitNorm, glm::vec3 *hitPos);

//场景
class Scene{
    std::vector<Shape*> objects;
    std::vector<Light*> light;
    glm::vec3 background_color;
    
public:
    Scene();
    ~Scene();
    Camera camera;
    //设置场景参数
    void setLight(Light *light);
    void addLight(Light *light);
    void addSphere(Sphere *sphere);
    void addCube(Cube *cube);
    void setBackground(glm::vec3 bgcolor = glm::vec3(.0f, .0f, .0f));
    
    //检测光线与场景中物体的碰撞
    bool SceneIntersect(const Ray &ray,
                        int *hitObject, glm::vec3 *hitNorm, glm::vec3 *hitPos);
    //阴影检测
    bool ShadowRayIntersect(const Ray &shadow_ray);
    //光线追踪
    glm::vec3 Trace(const Ray& ray, int depth = 0);
        
    //void renderScene(float viewWidth, float viewHeight, void(*handleFunc) (float, float, glm::vec3));
    
    void rotateCamera(float radian);
};
void renderScene(Scene& mainscene,float viewWidth, float viewHeight, void(*handleFunc) (float, float, glm::vec3));

#endif
