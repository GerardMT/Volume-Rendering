#ifndef VOLUME_H
#define VOLUME_H

#include "mesh.h"
#include "transform.h"
#include "paint_gl.h"

#include <QOpenGLShaderProgram>

class Volume : public PaintGL, public Transform
{
public:
    Volume(glm::vec3 pos);

    ~Volume();

    void transform(glm::mat4 m) override;

    void initialieGL() override;

    void resize(Camera &camera_) override;

    void paintGL(float dt, const Camera &camera) override;

private:
    Mesh *mesh_;

    glm::mat4 model_;

    QOpenGLShaderProgram program_position;
    QOpenGLShaderProgram program_volume;

    GLuint vao_;
    GLuint vbo_;
    GLuint nbo_;
    GLuint fbo_;

    GLuint framebuffer_ = 0;
    GLuint texture_;

    QOpenGLContext *context_;
};

#endif // VOLUME_H
