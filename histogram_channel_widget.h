
#ifndef HISTOGRAMCHANNELWIDGET_H
#define HISTOGRAMCHANNELWIDGET_H

#include "volume_data.h"

#include <GL/glew.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <glm/glm.hpp>
#include <vector>

using namespace std;

class HistogramWidget;

class HistogramChannelWidget : public QOpenGLWidget
{
public:
    friend HistogramWidget;

    explicit HistogramChannelWidget(int channel, QWidget *parent = nullptr);

    ~HistogramChannelWidget();

    void volumeData(VolumeData &volumeData);

    void histogramWidget(HistogramWidget &histogramWidget);

private:
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void sortPoints();

    void pointsModified();

    void volumeDataUpdated();

    int channel_;

    vector<float> data_;

    // Histogram
    QOpenGLShaderProgram program_histogram_;

    GLuint vao_histogram_;
    GLuint vbo_quad_;
    GLuint fbo_quad_;

    // Points
    QOpenGLShaderProgram program_point_;

    GLuint vao_points_;
    GLuint tbo_points_;

    vector<glm::vec2> points_;

    // Lines
    QOpenGLShaderProgram program_line_;

    GLuint vao_lines_;

    float point_distance_ = 0.05;

    int width_;
    int height_;

    VolumeData *volume_data_ = nullptr;

    bool initialized_ = false;

    HistogramWidget *histogram_widget_;

    int move_point_ = -1;
};

#endif // HISTOGRAMCHANNELWIDGET_H
