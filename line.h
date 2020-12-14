
#ifndef LINE_H
#define LINE_H

#include "paint_gl.h"

#include <QOpenGLShaderProgram>

using namespace std;

class Line : public PaintGL
{
public:
    Line();

    ~Line();

    void initialieGL();

    void resize(Camera &camera);

    void paintGL(float dt, Camera &camera);

    void data(glm::vec3 *data);

    void color(const glm::vec4 &color);

private:
    static QOpenGLShaderProgram *program_;
    static unsigned int instances_;

    GLuint vao_;
    GLuint vbo_;

    glm::vec4 color_;
};

#endif // LINE_H
