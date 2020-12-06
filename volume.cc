#include "volume.h"

#include <glm/gtc/type_ptr.hpp>
#include <QOpenGLContext>
#include <iostream>
#include <QImage>

using namespace std;

Volume::Volume(glm::vec3 pos)
{
    bounding_box_mesh_ = new Mesh();
    bounding_box_mesh_->readPly("../model/cube.ply");

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
}

void Volume::setVolumeData(VolumeData &volumeData)
{
    delete volume_data_;

    volume_data_ = &volumeData;
    glUniform1i(program_volume.uniformLocation("volume_texture"), 1);
    glUniform1f(program_volume.uniformLocation("step_size"), 1.0f / max(max(volumeData.width_, volumeData.height_), volumeData.depth_));
    glUniform1f(program_volume.uniformLocation("max_density"), volumeData.histogram_[0]);
}

void Volume::transform(glm::mat4 m)
{
    model_ = m * model_;
}

void Volume::initialieGL()
{
    bool res;
    res = program_position.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/position.vert");
    if (!res){
        cout << program_position.log().toUtf8().constData() << endl;
    }

    res = program_position.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/position.frag");
    if (!res) {
        cout << program_position.log().toUtf8().constData() << endl;
    }
    program_position.link();

    res = program_volume.addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/volume.vert");
    if (!res){
        cout << program_volume.log().toUtf8().constData() << endl;
    }

    res = program_volume.addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/volume.frag");
    if (!res) {
        cout << program_volume.log().toUtf8().constData() << endl;
    }
    program_volume.link();

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
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    QImage image;
    if (!image.load("../textures/noise.png")) { // http://momentsingraphics.de/BlueNoise.html
        throw 1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, image.width(), image.height(), 0, GL_RED, GL_UNSIGNED_SHORT, image.bits());

    program_volume.bind();
    glUniform1i(program_volume.uniformLocation("noise_texture"), 2);

    glUniform1i(program_volume.uniformLocation("end_texture"), 0);
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

    program_volume.bind();
    glUniform2f(program_volume.uniformLocation("pixel_size"), 1.0f / camera_.width_, 1.0f / camera_.height_);
}

void Volume::paintGL(__attribute__((unused)) float dt, const Camera &camera)
{
    // First pass
    glDisable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    program_position.bind();
    glUniformMatrix4fv(program_position.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model_));
    glUniformMatrix4fv(program_position.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, bounding_box_mesh_->faces_.size() * 3, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);

    // Second pass
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, context_->defaultFramebufferObject());

    program_volume.bind();
    glUniformMatrix4fv(program_volume.uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model_));
    glUniformMatrix4fv(program_volume.uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));

    glActiveTexture(GL_TEXTURE0); // TODO Move outside loop
    glBindTexture(GL_TEXTURE_2D, end_texture_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, volume_data_->texture_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noise_texture_);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, bounding_box_mesh_->faces_.size() * 3, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
}
