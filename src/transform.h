#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

class Transform
{
public:
    virtual ~Transform() {};

    virtual void transform(glm::mat4 m) = 0;
};

#endif // TRANSFORM_H
