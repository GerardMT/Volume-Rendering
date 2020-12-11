
#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include "camera.h"
#include "volume.h"

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QPushButton>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);

    ~GLWidget();

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void loadVolumeData(const string &filename);

    void pause();

    void play();

private:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    QTimer *timer_;

    float target_frame_time_ = 1.0f / 60.0f * 1000.0f;

    float time_ = 0.0;

    Camera camera_;

    Light light_;

    Volume *volume_;

    chrono::steady_clock::time_point  time_last_;

    bool rotate_ = false;

    int rotate_x_ = 0;
    int rotate_y_ = 0;

    int rotate_last_x_;
    int rotate_last_y_;

    float rotate_sensitivity_ = 0.01f;

    bool render_loop_;

    bool gizmos_;

    QOpenGLShaderProgram program_color_;

    GLuint vao_axis_;
    GLuint vbo_axis_;

private slots:
    void stepsFactor(double v);

    void stepsFactorShadow(double v);

    void gizmos(bool v);

    void shadows(bool v);

    void renderLoop(bool v);
};

#endif  //  GLWIDGET_H_
