#ifndef VOLUME_H
#define VOLUME_H

#include "mesh.h"
#include "transform.h"
#include "paint_gl.h"
#include "volume_data.h"

#include <QOpenGLShaderProgram>

class Volume : public PaintGL, public Transform
{
public:
    Volume(glm::vec3 pos);

    ~Volume();

    void setVolumeData(VolumeData &volumeData);

    void transform(glm::mat4 m) override;

    void initialieGL() override;

    void resize(Camera &camera_) override;

    void paintGL(float dt, const Camera &camera) override;

private:
    Mesh *bounding_box_mesh_;

    VolumeData *volume_data_ = nullptr;

    glm::mat4 model_;

    QOpenGLShaderProgram program_position;
    QOpenGLShaderProgram program_volume;

    GLuint vao_;
    GLuint vbo_;
    GLuint nbo_;
    GLuint fbo_;

    GLuint framebuffer_ = 0;

    GLuint end_texture_;

    GLuint noise_texture_;

    QOpenGLContext *context_;
};

#endif // VOLUME_H
