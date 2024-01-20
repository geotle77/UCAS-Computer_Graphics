#ifndef material_aux_h
#define material_aux_h
#include "../include/header.h"

glm::vec3 red(1.0f, .0f, .0f);
glm::vec3 green(.0f, 1.0f, .0f);
glm::vec3 blue(.0f, .0f, 1.0f);
glm::vec3 yellow(1.0f, 1.0f, .0f);
glm::vec3 cyan(.0f, 1.0f, 1.0f);
glm::vec3 violet(.53f, .0f, .48f);
glm::vec3 orange(1.0f, 1.0f, .0f);
glm::vec3 white(1.0f, 1.0f, 1.0f);
glm::vec3 black(.0f, .0f, .0f);
glm::vec3 brown(.83f, .69f, .21f);


#define roughMaterial(color) (new Material(color, 32.0f))
#define defaultReflective (new Material(glm::vec3(.48f, .48f, .48f)))
#define metalReflective (new Material(glm::vec3(.86f, .87f, .88f)))
#define defaultRefractive (new Material(1.5f))

#endif /* material_aux_h */