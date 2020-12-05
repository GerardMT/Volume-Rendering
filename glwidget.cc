
#include "glwidget.h"

#include <QMouseEvent>
#include <QTimer>
#include <glm/gtx/transform.hpp>

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(target_frame_time_);
}

GLWidget::~GLWidget()
{
    delete volume_;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mousePressEvent(QMouseEvent *event) {
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
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
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

void GLWidget::initializeGL()
{
    glewInit();

    camera_.pos_ = glm::vec3(2.5f, 0.0f, 0.0f);
    camera_.fov_x_ = glm::radians(90.0f);
    camera_.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    volume_= new Volume(glm::vec3(0.0f, 0.0f, 0.0f));
    volume_->initialieGL();
}

void GLWidget::resizeGL(int w, int h)
{
    camera_.resize(w, h);
    volume_->resize(camera_);
}

void GLWidget::paintGL()
{
    chrono::steady_clock::time_point time_now = chrono::steady_clock::now();
    float dt_ = chrono::duration_cast<chrono::nanoseconds>(time_now - time_last_).count() * 1e-9;

    if (rotate_) {
        volume_->transform(glm::rotate(rotate_x_ * rotate_sensitivity_, camera_.UP_)
                           * glm::rotate(rotate_y_ * rotate_sensitivity_, camera_.right_));

        rotate_x_ = 0;
        rotate_y_ = 0;
    }

    camera_.compute_view_projection();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    volume_->paintGL(dt_, camera_);

    time_last_ = time_now;
}
