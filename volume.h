#ifndef VOLUME_H
#define VOLUME_H

#include "mesh.h"
#include "transform.h"
#include "paint_gl.h"
#include "volume_data.h"
#include "light.h"
#include "histogram_widget_callback.h"
#include "histogram_widget.h"

#include <QOpenGLShaderProgram>

class Volume : public PaintGL, public Transform, public HistogramWidgetCallback
{
public:
    Volume(glm::vec3 pos);

    ~Volume();

    VolumeData *volumeData();

    void volumeData(VolumeData &volumeData);

    void light(Light &light);

    void transform(glm::mat4 m) override;

    void initialieGL() override;

    void resize(Camera &camera_) override;

    void paintGL(float dt, const Camera &camera) override;

    void stepsFactor(double v);

    void stepsFactorShadow(double v);

    void histogramUpdated(vector<glm::vec4> &data) override;

    void histogram(HistogramWidget &histogramWidget);

    bool gizmos_;

    bool shadows_;

private:
    void initializeHistogramData();

    Mesh *bounding_box_mesh_;

    VolumeData *volume_data_ = nullptr;

    Light *light_;

    glm::vec3 pos_;
    glm::mat4 model_;

    QOpenGLShaderProgram program_position_;
    QOpenGLShaderProgram program_volume_;

    QOpenGLShaderProgram program_color_;

    GLuint vao_;
    GLuint vbo_;
    GLuint nbo_;
    GLuint fbo_;

    GLuint vao_line_;
    GLuint vbo_line_;

    GLuint framebuffer_ = 0;

    GLuint end_texture_;

    GLuint noise_texture_;

    GLuint lut_texture_;

    QOpenGLContext *context_;

    HistogramWidget *histogram_widget_;

    vector<glm::vec4> *histogram_data_ = nullptr;

    bool histogram_data_initialized_ = false;

    bool initialized_ = false;
};

#endif // VOLUME_H
