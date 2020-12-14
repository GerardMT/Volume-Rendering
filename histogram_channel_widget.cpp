
#include "histogram_channel_widget.h"
#include "histogram_widget.h"

#include <iostream>
#include <QMouseEvent>

const GLfloat quad_vertices_[] = {
    -1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f,
     1.0f, -1.0f
};

const GLuint quad_indices_[] = {0, 1, 2, 0, 2, 3};

HistogramChannelWidget::HistogramChannelWidget(int channel, QWidget *parent) : QOpenGLWidget(parent)
{
    channel_ = channel;
}

HistogramChannelWidget::~HistogramChannelWidget()
{
    // Histogram
    glDeleteVertexArrays(1, &vao_histogram_);
    glDeleteBuffers(1, &vbo_quad_);
    glDeleteBuffers(1, &fbo_quad_);

    // Points
    glDeleteVertexArrays(1, &vao_points_);
    glDeleteBuffers(1, &tbo_points_);

    // Lines
    glDeleteVertexArrays(1, &vao_lines_);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void HistogramChannelWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (move_point_ == -1) {
        return;
    }

    glm::vec2 pos(event->x() / static_cast<float>(width_), 1.0f - event->y() / static_cast<float>(height_));

    points_[move_point_].y = min(max(pos.y, 0.0f), 1.0f);
    if (move_point_ != 0 && static_cast<unsigned long>(move_point_) != points_.size() - 1) {
        points_[move_point_].x = min(max(pos.x, points_[move_point_ - 1].x +  point_distance_), points_[move_point_ + 1].x - point_distance_);
    }

    pointsModified();
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void HistogramChannelWidget::mousePressEvent(QMouseEvent *event) {
    glm::vec2 click(event->x() / static_cast<float>(width_), 1.0f - event->y() / static_cast<float>(height_));
    if (click.x < 0.0 || click.x > 1.0 || click.y < 0.0 || click.y > 1.0 ) {
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:
    {
        for (unsigned int i = 0; i < points_.size(); ++i) {
            if (abs(points_[i].x - click.x) <= point_distance_) {
                move_point_ = i;
                break;
            }
        }

        if (move_point_ == -1) {
            points_.push_back(click);
            sortPoints();
            pointsModified();

            // Find point again once sorted
            for (unsigned int i = 0; i < points_.size(); ++i) {
                if (abs(points_[i].x - click.x) <= point_distance_) {
                    move_point_ = i;
                    break;
                }
            }
        }

        break;
    }
    }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void HistogramChannelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (move_point_  != -1) {
        move_point_ = -1;
        return;
    }

    glm::vec2 click(event->x() / static_cast<float>(width_), 1.0f - event->y() / static_cast<float>(height_));
    if (click.x < 0.0 || click.x > 1.0 || click.y < 0.0 || click.y > 1.0 ) {
        return;
    }

    switch (event->button()) {
    case Qt::RightButton:
    {
        for (unsigned int i = 0; i < points_.size(); ++i) {
            if (glm::length(points_[i] - click) <= point_distance_) {
                if (move_point_ == 0) {
                    points_[i].y = 0.0;
                } else if (i ==  points_.size() - 1) {
                    points_[i].y = 1.0;
                } else {
                    points_.erase(points_.begin() + i);
                }
                pointsModified();
                break;
            }
        }

        break;
    }
    }
}
#pragma GCC diagnostic pop

void HistogramChannelWidget::initializeGL()
{
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Histogram
    bool res = program_histogram_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/histogram.vert");
    if (!res){
        cout << program_histogram_.log().toUtf8().constData() << endl;
    }
    res = program_histogram_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/histogram.frag");
    if (!res) {
        cout << program_histogram_.log().toUtf8().constData() << endl;
    }
    program_histogram_.link();

    glGenVertexArrays(1, &vao_histogram_);
    glBindVertexArray(vao_histogram_);

    glGenBuffers(1, &vbo_quad_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices_), quad_vertices_, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &fbo_quad_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbo_quad_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices_), quad_indices_, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (volume_data_ != nullptr) {
        volumeDataUpdated();
    }

    // Points
    res = program_point_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/histogram_point.vert");
    if (!res){
        cout << program_point_.log().toUtf8().constData() << endl;
    }
    res = program_point_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/histogram_point.frag");
    if (!res) {
        cout << program_point_.log().toUtf8().constData() << endl;
    }
    program_point_.link();

    glGenVertexArrays(1, &vao_points_);
    glBindVertexArray(vao_points_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &tbo_points_);
    glBindBuffer(GL_ARRAY_BUFFER, tbo_points_);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbo_quad_);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    program_point_.bind();
    glUniform1f(program_point_.uniformLocation("size"), point_distance_);

    // Lines
    res = program_line_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/histogram_line.vert");
    if (!res){
        cout << program_line_.log().toUtf8().constData() << endl;
    }
    res = program_line_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/histogram_line.frag");
    if (!res) {
        cout << program_line_.log().toUtf8().constData() << endl;
    }
    program_line_.link();

    glGenVertexArrays(1, &vao_lines_);
    glBindVertexArray(vao_lines_);

    glBindBuffer(GL_ARRAY_BUFFER, tbo_points_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized_ = true;
    histogram_widget_->initialized(channel_);

    points_.push_back(glm::vec2(0.0, 0.0));
    points_.push_back(glm::vec2(1.0, 1.0));
    data_.resize(256);
    pointsModified();
}

void HistogramChannelWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    width_ = w;
    height_ = h;

    program_point_.bind();
    glUniform1f(program_point_.uniformLocation("ratio"), static_cast<float>(w) / static_cast<float>(h));
}

void HistogramChannelWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Histogram
    program_histogram_.bind();

    glBindVertexArray(vao_histogram_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // Lines
    program_line_.bind();

    glBindVertexArray(vao_lines_);
    glDrawArrays(GL_LINE_STRIP, 0, points_.size());
    glBindVertexArray(0);

    // Points
    program_point_.bind();

    glBindVertexArray(vao_points_);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, points_.size());
    glBindVertexArray(0);
}

void HistogramChannelWidget::sortPoints()
{
    sort(points_.begin(), points_.end(), [](const glm::vec2 &a, const glm::vec2 &b) -> bool
    {
        return a.x < b.x;
    });
}

void HistogramChannelWidget::pointsModified()
{
    glBindBuffer(GL_ARRAY_BUFFER, tbo_points_);
    glBufferData(GL_ARRAY_BUFFER, points_.size() * sizeof(decltype(points_)::value_type), &points_[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Interpolate points
    unsigned int p = 0;
    for (unsigned int i = 0; i < 256; ++i) {
        float p_x_range = points_[p].x * 255.0f;
        float p_next_xrange = points_[p + 1].x * 255.0f;

        float t = (i - p_x_range) / (p_next_xrange - p_x_range);
        data_[i] = points_[p].y + t * (points_[p + 1].y - points_[p].y); // lerp

        if (p + 1 < points_.size() - 1 && p_next_xrange >= i + 1) {
            ++p;
        }
    }

    histogram_widget_->dataUpdated();

    update();
}

void HistogramChannelWidget::volumeDataUpdated()
{
    program_histogram_.bind();
    glUniform1fv(program_histogram_.uniformLocation("histogram"), volume_data_->histogram_.size(), &volume_data_->histogram_[0]);
}

void HistogramChannelWidget::volumeData(VolumeData &volumeData)
{
    volume_data_ = &volumeData;

    if (initialized_) {
        volumeDataUpdated();
    }
}

void HistogramChannelWidget::histogramWidget(HistogramWidget &histogramWidget)
{
    histogram_widget_ = &histogramWidget;
}
