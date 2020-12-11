
#include "volume.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QOpenGLContext>
#include <iostream>
#include <QImage>

using namespace std;

Volume::Volume(glm::vec3 pos)
{
    bounding_box_mesh_ = new Mesh();
    bounding_box_mesh_->readPly("../model/cube.ply");

    pos_ = pos;

    model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, pos);

    context_ = QOpenGLContext::currentContext();
}

Volume::~Volume()
{
    delete bounding_box_mesh_;

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &nbo_);
    glDeleteBuffers(1, &fbo_);

    glDeleteFramebuffers(1, &framebuffer_);

    glDeleteTextures(1, &end_texture_);
    glDeleteTextures(1, &noise_texture_);

    glDeleteVertexArrays(1, &vao_line_);
    glDeleteBuffers(1, &vbo_line_);
}

VolumeData *Volume::volumeData()
{
    return volume_data_;
}

void Volume::volumeData(VolumeData &volumeData)
{
    delete volume_data_;

    volume_data_ = &volumeData;
    glUniform1i(program_volume_.uniformLocation("volume_texture"), 1);

    float max_dim = max(max(volumeData.width_, volumeData.height_), volumeData.depth_);
    glUniform1f(program_volume_.uniformLocation("step_size"), 2.0f / max_dim);
    glUniform1f(program_volume_.uniformLocation("texture_max_size"), max_dim);
}

void Volume::light(Light &light)
{
    light_ = &light;
}

void Volume::transform(glm::mat4 m)
{
    pos_ = glm::vec3(m * glm::vec4(pos_, 1.0));
    model_ = m * model_;
}

void Volume::initialieGL()
{
    bool res;
    res = program_position_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/position.vert");
    if (!res){
        cout << program_position_.log().toUtf8().constData() << endl;
    }

    res = program_position_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/position.frag");
    if (!res) {
        cout << program_position_.log().toUtf8().constData() << endl;
    }
    program_position_.link();

    res = program_volume_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/volume.vert");
    if (!res){
        cout << program_volume_.log().toUtf8().constData() << endl;
    }

    res = program_volume_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/volume.frag");
    if (!res) {
        cout << program_volume_.log().toUtf8().constData() << endl;
    }
    program_volume_.link();

    // Bounding box
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, bounding_box_mesh_->vertices_.size() * sizeof(decltype(bounding_box_mesh_->vertices_)::value_type), &bounding_box_mesh_->vertices_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &nbo_);
    glBindBuffer(GL_ARRAY_BUFFER, nbo_);
    glBufferData(GL_ARRAY_BUFFER, bounding_box_mesh_->normals_.size() * sizeof(decltype(bounding_box_mesh_->normals_)::value_type), &bounding_box_mesh_->normals_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &fbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fbo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bounding_box_mesh_->faces_.size() * sizeof(decltype(bounding_box_mesh_->faces_)::value_type), &bounding_box_mesh_->faces_[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Framebuffer
    glGenFramebuffers(1, &framebuffer_);

    // Noise texture
    glGenTextures(1, &noise_texture_);
    glBindTexture(GL_TEXTURE_2D, noise_texture_);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    QImage image;
    if (!image.load("../textures/noise.png")) { // http://momentsingraphics.de/BlueNoise.html
        throw 1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, image.width(), image.height(), 0, GL_RED, GL_UNSIGNED_SHORT, image.bits());

    // Volume
    program_volume_.bind();
    glUniform1i(program_volume_.uniformLocation("noise_texture"), 2);

    glUniform1i(program_volume_.uniformLocation("end_texture"), 0);

    res = program_color_.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/color.vert");
    if (!res){
        cout << program_color_.log().toUtf8().constData() << endl;
    }
    res = program_color_.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/color.frag");
    if (!res) {
        cout << program_color_.log().toUtf8().constData() << endl;
    }
    program_color_.link();

    // Line
    glGenVertexArrays(1, &vao_line_);
    glBindVertexArray(vao_line_);

    glGenBuffers(1, &vbo_line_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_line_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Lut
    glGenTextures(1, &lut_texture_);
    glBindTexture(GL_TEXTURE_1D, lut_texture_);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    program_volume_.bind();
    glUniform1i(program_volume_.uniformLocation("lut_texture"), 3);

    if (histogram_data_ != nullptr) {
        initializeHistogramData();
    }

    initialized_ = true;
}

void Volume::resize(Camera &camera_)
{
    glDeleteTextures(1, &end_texture_);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glGenTextures(1, &end_texture_);
    glBindTexture(GL_TEXTURE_2D, end_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, camera_.width_, camera_.height_, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, end_texture_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, context_->defaultFramebufferObject());

    program_volume_.bind();
    glUniform2f(program_volume_.uniformLocation("pixel_size"), 1.0f / camera_.width_, 1.0f / camera_.height_);
}

void Volume::paintGL(__attribute__((unused)) float dt, const Camera &camera)
{
    if (!histogram_widget_->initialized()) {
        return;
    }

    // First pass
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    program_position_.bind();
    glUniformMatrix4fv(program_position_.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model_));
    glUniformMatrix4fv(program_position_.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, bounding_box_mesh_->faces_.size() * 3, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);

    // Second pass
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, context_->defaultFramebufferObject());

    program_volume_.bind();
    glUniformMatrix4fv(program_volume_.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model_));
    glUniformMatrix4fv(program_volume_.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));
    glUniform3fv(program_volume_.uniformLocation("light_pos"), 1, glm::value_ptr(light_->pos_));
    glUniform1i(program_volume_.uniformLocation("shadows"), shadows_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, end_texture_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, volume_data_->texture_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noise_texture_);

    // Lut
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, lut_texture_);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, bounding_box_mesh_->faces_.size() * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_BLEND);

    if (gizmos_) {
        // Light line
        glm::vec3 line[2];
        line[0] = light_->pos_;
        line[1] = pos_;

        glBindBuffer(GL_ARRAY_BUFFER, vbo_line_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

        program_color_.bind();
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 color = glm::vec4(0.0, 1.0, 0.0, 1.0);
        glUniformMatrix4fv(program_color_.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(identity));
        glUniformMatrix4fv(program_color_.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));
        glUniform4fv(program_color_.uniformLocation("color"), 1, glm::value_ptr(color));

        glBindVertexArray(vao_line_);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);

        // Bounding box
        program_color_.bind();
        color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        glUniformMatrix4fv(program_color_.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model_));
        glUniformMatrix4fv(program_color_.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));
        glUniform4fv(program_color_.uniformLocation("color"), 1, glm::value_ptr(color));

        glDisable(GL_CULL_FACE);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, bounding_box_mesh_->faces_.size() * 3, GL_UNSIGNED_INT, (void *) 0);
        glBindVertexArray(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Volume::stepsFactor(double v)
{
    program_volume_.bind();
    glUniform1f(program_volume_.uniformLocation("steps_factor"), v);
}

void Volume::stepsFactorShadow(double v)
{
    program_volume_.bind();
    glUniform1f(program_volume_.uniformLocation("steps_factor_shadow"), v);
}

void Volume::histogramUpdated(vector<glm::vec4> &data)
{
    histogram_data_ = &data;

    if (initialized_) {
        initializeHistogramData();
    }
}

void Volume::histogram(HistogramWidget &histogramWidget)
{
    histogram_widget_ = &histogramWidget;
    histogramWidget.histogram_widget_callback_ = this;
}

void Volume::initializeHistogramData()
{
    glBindTexture(GL_TEXTURE_1D, lut_texture_);
    if (histogram_data_initialized_) {
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, histogram_data_->size(), GL_RGBA, GL_FLOAT, &(*histogram_data_)[0]);
    } else {
        histogram_data_initialized_ = true;
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, histogram_data_->size(), 0, GL_RGBA, GL_FLOAT, &(*histogram_data_)[0]);
    }
}
