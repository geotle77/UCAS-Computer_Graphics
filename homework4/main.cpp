#include "header.h"
#include "material.h"
#include "stb_image.h"
#include <string>

char* dest_dir = "./outpic/";
char* texture_dir = "./input/";
ImageTexture* tex[5];
int main(int argc, char **argv){
    for(int i=0; i<5; i++){
        int nx, ny, nn;
        std::stringstream sm;
        sm << texture_dir << i << ".jpg";
        std::string img_name;
        sm >> img_name;
        unsigned char *img = stbi_load(img_name.data(), &nx, &ny, &nn, 0);
        tex[i] = new ImageTexture(img, nx, ny);
    }
    Scene mainScene;
    mainScene.setBackground(glm::vec3(.461f, .141f, .062f));
    mainScene.setLight(new Light(glm::vec3(.5f, 1.0f, .5f), glm::vec3(1.5f, 1.5f, 1.5f)));
    //mainScene.addLight(new Light(glm::vec3(.0f, .0f, 1.0f), glm::vec3(.8f, .8f, .8f)));
    mainScene.addSphere(new Sphere(glm::vec3(.2f, .0f, .0f), .1f, roughMaterial(blue)));
    mainScene.addSphere(new Sphere(glm::vec3(-.2f, .0f, .0f), .1f, roughMaterial(red)));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, .0f, -.2f), .1f, roughMaterial(brown)));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, .0f, .2f), .1f, defaultRefractive));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, .2f, .0f), .1f, metalReflective));
    mainScene.addSphere(new Sphere(glm::vec3(.0f, -200.15f, .0f), 200.0f, defaultReflective));
}