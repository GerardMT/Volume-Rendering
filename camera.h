#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 UP_ = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view_projection;

    glm::vec3 pos_;

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;

    float azimuth_;
    float inclination_;

    GLuint width_;
    GLuint height_;

    GLfloat ratio_;
    GLfloat fov_y_;
    GLfloat fov_x_;

    GLfloat near_ = 0.1f;
    GLfloat far_ = 100.0f;

    float speed_ = 5.0;
    float sensitivity_ = 10.0;

    void lookAt(glm::vec3 pos);

    void forward(float dt);

    void backwards(float dt);

    void left(float dt);

    void right(float dt);

    void rotate(int x, int y, float dt);

    void compute_view_projection();

    void resize(int width, int height);
};

#endif // CAMERA_H
