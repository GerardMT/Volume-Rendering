#ifndef VOLUME_H
#define VOLUME_H

#include "mesh.h"
#include "transform.h"
#include "paint_gl.h"
#include "volume_data.h"
#include "light.h"
#include "histogram_widget_callback.h"
#include "histogram_widget.h"
#include "line.h"

#include <QOpenGLShaderProgram>

class Volume : public PaintGL, public Transform
{
public:
    Volume(glm::vec3 pos);

    ~Volume();

    void volumeData(VolumeData &volumeData);

    void light(Light &light);

    void transform(glm::mat4 m) override;

    void initialieGL() override;

    void resize(Camera &camera_) override;

    void paintGL(float dt, Camera &camera) override;

    void stepsFactor(double v);

    void stepsFactorShadow(double v);

    void histogramUpdated(vector<glm::vec4> &data);

    void histogram(HistogramWidget &histogramWidget);

    void ambient(float v);

    void diffuse(float v);

    void specular(float v);

    void shininess(float v);

    void noiseFactor(float v);

    void shadows(bool v);

    void gizmos(bool v);

    void OpenGLWidget(QOpenGLWidget &opengl_widget);

private:
    void initializeHistogramData();

    QOpenGLWidget *opengl_widget_;

    Mesh *bounding_box_mesh_;

    VolumeData *volume_data_ = nullptr;

    Light *light_;

    glm::vec3 pos_;
    glm::mat4 model_;

    // Volume bounding mesh
    GLuint vao_;
    GLuint vbo_;
    GLuint nbo_;
    GLuint fbo_;

    // 1st pass
    GLuint framebuffer_ = 0;
    QOpenGLShaderProgram program_position_;
    GLuint end_texture_;

    // 2n pass
    QOpenGLShaderProgram program_volume_;

    // Noise
    GLuint noise_texture_;

    // Lut
    GLuint lut_texture_;

    // Histogram
    HistogramWidget *histogram_widget_;
    vector<glm::vec4> *histogram_data_ = nullptr;
    bool histogram_data_initialized_ = false;

    bool initialized_ = false;

    // Gizmos
    bool gizmos_;
    QOpenGLShaderProgram program_color_;
    Line light_line_;
};

#endif // VOLUME_H
