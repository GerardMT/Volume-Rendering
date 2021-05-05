#ifndef PAINTGL_H
#define PAINTGL_H

#include "camera.h"

class PaintGL
{
public:
    virtual ~PaintGL() {};

    virtual void initialieGL() = 0;

    virtual void resize(Camera &camera) = 0;

    virtual void paintGL(float dt, Camera &camera) = 0;
};

#endif // PAINTGL_H
