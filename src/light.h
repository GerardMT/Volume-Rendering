
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    Light();

    glm::vec3 pos_;
    glm::vec3 intensity_;
};

#endif // LIGHT_H
