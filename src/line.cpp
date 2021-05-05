
#include "line.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

QOpenGLShaderProgram *Line::program_ = nullptr;
unsigned int Line::instances_ = 0;

Line::Line()
{
}

Line::~Line()
{
    if (instances_ == 0) {
        delete program_;
    }

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void Line::initialieGL()
{
    if (instances_ == 0) {
        program_ = new QOpenGLShaderProgram();
        bool res = program_->addShaderFromSourceFile(QOpenGLShader::Vertex, "../../res/shader/line.vert");
        if (!res){
            cout << program_->log().toUtf8().constData() << endl;
        }
        res = program_->addShaderFromSourceFile(QOpenGLShader::Fragment, "../../res/shader/line.frag");
        if (!res) {
            cout << program_->log().toUtf8().constData() << endl;
        }
        program_->link();
        ++instances_;
    }


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Line::resize(__attribute__((unused)) Camera &camera)
{
}

void Line::data(glm::vec3 *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, &(*data)[0], GL_STATIC_DRAW);
}

void Line::color(const glm::vec4 &color)
{
    color_ = color;
}

void Line::paintGL(__attribute__((unused)) float dt, Camera &camera)
{
    program_->bind();
    glUniformMatrix4fv(program_->uniformLocation("view_projection"), 1, GL_FALSE, glm::value_ptr(camera.view_projection));
    glUniform4fv(program_->uniformLocation("color"), 1, glm::value_ptr(color_));

    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
