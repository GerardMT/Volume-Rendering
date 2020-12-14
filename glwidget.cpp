
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

static const unsigned int AXIS_N_LINES = 3;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
    delete volume_;
    delete timer_;
}

void GLWidget::histogramUpdated(vector<glm::vec4> &data)
{
    volume_->histogramUpdated(data);

    if (!render_loop_ ) {
        update();
    }
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

  if (!render_loop_ ) {
      update();
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

  if (!render_loop_ ) {
      update();
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

  if (!render_loop_ ) {
      update();
  }
}
#pragma GCC diagnostic pop

void GLWidget::loadVolumeData(const string &filename)
{
    volume_data_->readFromDicom(filename);
    histogram_widget_->volumeDataUpdated();
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

    render_loop_ = false; // To avoid any value calling update()

    // Camera
    camera_.fov_x_ = glm::radians(90.0f);
    camera_.position(glm::vec3(2.5f, 0.0f, 0.0f));
    camera_.center(glm::vec3(0.0f, 0.0f, 0.0f));

    // Light
    light_.pos_ = glm::vec3(10.0f, 0.0f, 0.0f);

    // Histogram
    histogram_widget_ = topLevelWidget()->findChild<HistogramWidget *>("histogram");
    histogram_widget_->histogram_widget_callback_ = this;

    // Volume data
    volume_data_ = new VolumeData();
    volume_data_->readFromDicom("../../Models/Test 1/");
    // Attach histogram and volume data
    histogram_widget_->volumeData(*volume_data_);

    // Volume
    volume_= new Volume(glm::vec3(0.0f, 0.0f, 0.0f));
    volume_->OpenGLWidget(*this);
    volume_->initialieGL();
    volume_->volumeData(*volume_data_);
    volume_->light(light_);
    volume_->stepsFactor(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxStepsFactor")->value());
    volume_->stepsFactorShadow(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxStepsFactorShadow")->value());
    volume_->shadows(topLevelWidget()->findChild<QCheckBox *>("checkBoxShadows")->isChecked());
    volume_->ambient(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxAmbient")->value());
    volume_->diffuse(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxDiffuse")->value());
    volume_->specular(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxSpecular")->value());
    volume_->shininess(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxShininess")->value());
    volume_->noiseFactor(topLevelWidget()->findChild<QDoubleSpinBox *>("doubleSpinBoxNoiseFactor")->value());
    // Attach histogram to volume and this
    volume_->histogram(*histogram_widget_);

    // Axis
    glm::vec3 line_data[2];
    line_data[0] = glm::vec3(0.0f);

    lines_.resize(AXIS_N_LINES);
    for (unsigned int i = 0; i < AXIS_N_LINES; ++i) {
        lines_[i].initialieGL();

        line_data[1] = glm::vec3(0.0f);
        line_data[1][i] = 1.0f;

        lines_[i].data(line_data);
        lines_[i].color(glm::vec4(line_data[1], 1.0f));
    }

    // Options
    gizmos(topLevelWidget()->findChild<QCheckBox *>("checkBoxGizmos")->isChecked());
    render_tiled_ = topLevelWidget()->findChild<QCheckBox *>("checkBoxRenderTiled")->isChecked();
    rotate_light_ = topLevelWidget()->findChild<QCheckBox *>("checkBoxRotateLight")->isChecked();

    // Set timer for render loop
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
    render_loop_ = topLevelWidget()->findChild<QCheckBox *>("checkBoxRenderLoop")->isChecked();
    renderLoop(render_loop_);
    // If render loop is disabled, render a first time
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
    if (render_tiled_ ) {
        // Render the viewpoet into tails to avoid crashing OpenGL in a heavy calculation
        int width = camera_.width_;
        int height = camera_.height_;

        const int KERNEL_SIZE = 50;

        int tiles = ceil(static_cast<float>(width) / KERNEL_SIZE) * ceil(static_cast<float>(height) / KERNEL_SIZE);

        int tile = 1;
        for(int32_t start_x = 0; start_x < width; start_x += KERNEL_SIZE) {
            for(int32_t start_y = 0; start_y < height; start_y += KERNEL_SIZE) {
                int end_x = min(width, start_x + KERNEL_SIZE);
                int end_y = min(height, start_y + KERNEL_SIZE);

                cout << "Tiled rendering: (" << tile << "/" << tiles << ")" << endl;

                glScissor(start_x, start_y, end_x, end_y);
                paintGLImmersive();
                glFinish();

                ++tile;
            }
        }
    } else {
        paintGLImmersive();
    }
}

void GLWidget::paintGLImmersive()
{
    chrono::steady_clock::time_point time_now = chrono::steady_clock::now();
    float dt = chrono::duration_cast<chrono::nanoseconds>(time_now - time_last_).count() * 1e-9;

    time_ += dt;

    if (rotate_) {
        camera_.rotate(rotate_x_, rotate_y_, dt);

        rotate_x_ = 0;
        rotate_y_ = 0;
    }

    if (rotate_light_) {
        light_x_ += dt * 0.5;
        light_.pos_.x = cos(light_x_) * 10.0f;
        light_.pos_.z = sin(light_x_) * 10.0f;
    }

    camera_.compute_view_projection();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    volume_->paintGL(dt, camera_);

    if (gizmos_) {
        for (auto &l : lines_) {
            l.paintGL(dt, camera_);
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
    volume_->gizmos(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::ambient(double v)
{
    volume_->ambient(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::diffuse(double v)
{
    volume_->diffuse(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::specular(double v)
{
    volume_->specular(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::shininess(double v)
{
    volume_->shininess(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::shadows(bool v)
{
    volume_->shadows(v);

    if (!render_loop_ ) {
        update();
    }
}

void GLWidget::rotateLight(bool v)
{
    rotate_light_ = v;
}

void GLWidget::renderLoop(bool v)
{
    render_loop_ = v;

    if (v) {
        play();
    } else {
        pause();
    }
}

void GLWidget::renderTiled(bool v)
{
    render_tiled_ = v;
}

void GLWidget::noiseFactor(double v)
{
    volume_->noiseFactor(v);

    if (!render_loop_ ) {
        update();
    }
}
