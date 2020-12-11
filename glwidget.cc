
#include "glwidget.h"

#include "histogram_widget.h"
#include "volume_data.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <QTimer>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
    delete volume_;
    delete timer_;

    glDeleteVertexArrays(1, &vao_axis_);
    glDeleteBuffers(1, &vbo_axis_);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mousePressEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::LeftButton:
    rotate_ = true;
    rotate_last_x_ = event->x();
    rotate_last_y_ = event->y();
    break;
  }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (rotate_) {
    rotate_x_ = event->x() - rotate_last_x_;
    rotate_y_ = event->y() - rotate_last_y_;

    rotate_last_x_ = event->x();
    rotate_last_y_ = event->y();
  }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::LeftButton:
    rotate_ = false;
    break;
  }
}
#pragma GCC diagnostic pop

void GLWidget::loadVolumeData(const string &filename)
{
    VolumeData *volumeData = new VolumeData();
    volumeData->readFromDicom(filename);

    volume_->volumeData(*volumeData);
}

void GLWidget::pause()
{
    timer_->stop();
}

void GLWidget::play()
{
    timer_->start(target_frame_time_);
}

void GLWidget::initializeGL()
{
    glewInit();

    render_loop_ = false;

    camera_.fov_x_ = glm::radians(90.0f);
    camera_.position(glm::vec3(2.5f, 0.0f, 0.0f));
    camera_.center(glm::vec3(0.0f, 0.0f, 0.0f));

    light_.pos_ = glm::vec3(10.0f, 0.0f, 0.0f);

    VolumeData *volumeData = new VolumeData();
    volumeData->readFromDicom("../../Models/Test 1/");

    topLevelWidget()->findChild<HistogramWidget *>("histogram")->volumeData(*volumeData);

    volume_= new Volume(glm::vec3(0.0f, 0.0f, 0.0f));
    volume_->initialieGL();
    volume_->volumeData(*volumeData);
    volume_->light(light_);

    volume_->stepsFactor((topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxStepsFactor"))->value());
    volume_->stepsFactorShadow((topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxStepsFactorShadow"))->value());
    volume_->shadows_ = topLevelWidget()->findChild<QCheckBox *>("checkBoxShadows")->isChecked();
    volume_->histogram(*topLevelWidget()->findChild<HistogramWidget *>("histogram"));

    gizmos(topLevelWidget()->findChild<QCheckBox *>("checkBoxGizmos")->isChecked());

    bool res = program_color_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/color.vert");
    if (!res){
        cout << program_color_.log().toUtf8().constData() << endl;
    }
    res = program_color_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/color.frag");
    if (!res) {
        cout << program_color_.log().toUtf8().constData() << endl;
    }
    program_color_.link();

    glGenVertexArrays(1, &vao_axis_);
    glBindVertexArray(vao_axis_);

    glGenBuffers(1, &vbo_axis_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_axis_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
    render_loop_ = topLevelWidget()->findChild<QCheckBox *>("checkBoxRenderLoop")->isChecked();
    renderLoop(render_loop_);
    if (!render_loop_) {
        update();
    }
}

void GLWidget::resizeGL(int w, int h)
{
    camera_.resize(w, h);
    volume_->resize(camera_);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::paintGL()
{
    chrono::steady_clock::time_point time_now = chrono::steady_clock::now();
    float dt = chrono::duration_cast<chrono::nanoseconds>(time_now - time_last_).count() * 1e-9;

    time_ += dt;

    if (rotate_) {
        camera_.rotate(rotate_x_, rotate_y_, dt);

        rotate_x_ = 0;
        rotate_y_ = 0;
    }

    //light_.pos_.x = cos(time_ * 0.5) * 10.0f;
    //light_.pos_.z = sin(time_ * 0.5) * 10.0f;

    camera_.compute_view_projection();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    volume_->paintGL(dt, camera_);

    if (gizmos_) {
        glm::vec3 line[2];
        line[0] = glm::vec3(0.0f);

        for (int i = 0; i < 3; ++i) {
            line[1] = glm::vec3(0.0f);
            line[1][i] = 1.0;

            glBindBuffer(GL_ARRAY_BUFFER, vbo_axis_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

            program_color_.bind();
            glm::mat4 identity = glm::mat4(1.0);
            glUniformMatrix4fv(program_color_.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(identity));
            glUniformMatrix4fv(program_color_.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera_.view_projection));
            glUniform4fv(program_color_.uniformLocation("color"), 1, glm::value_ptr(line[1]));

            glBindVertexArray(vao_axis_);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }
    }

    time_last_ = time_now;
}

void GLWidget::stepsFactor(double v)
{
    volume_->stepsFactor(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::stepsFactorShadow(double v)
{
    volume_->stepsFactorShadow(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::gizmos(bool v)
{
    gizmos_ = v;
    volume_->gizmos_ = v;

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::shadows(bool v)
{
    volume_->shadows_ = v;

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::renderLoop(bool v)
{
    render_loop_ = v;

    if (v) {
        time_last_ = chrono::steady_clock::now();
        play();
    } else {
        pause();
    }
}
