
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::compute_view_projection()
{
    glm::mat4 view = glm::lookAt(pos_, pos_ + front_, UP_);

    glm::mat4 projection = glm::perspective(fov_y_, ratio_, near_, far_);

    view_projection = projection * view;
}

void Camera::position(glm::vec3 pos) {
    pos_ = pos;
    distance_ = glm::length(center_ - pos);
}

void Camera::center(glm::vec3 center)
{
    center_ = center;
    distance_ = glm::length(center - pos_);

    front_ = glm::normalize(center - pos_);
    right_ = glm::normalize(glm::cross(front_, UP_));
    up_ = glm::cross(right_, front_);

    inclination_= glm::degrees(acos(front_.y));
    azimuth_ = glm::degrees(atan(front_.z / front_.x));
}

void Camera::forward(float dt)
{
    pos_ += dt * speed_ * front_;
}

void Camera::backwards(float dt)
{
    pos_ -= dt * speed_ * front_;
}

void Camera::left(float dt)
{
    pos_ -= dt * speed_ * right_;
}

void Camera::right(float dt)
{
    pos_ += dt * speed_ * right_;
}

void Camera::rotate(int x, int y, float dt)
{
    azimuth_ += x * dt * sensitivity_ ;
    inclination_ += -y * dt * sensitivity_;

    if (inclination_ > 180.0f) {
        inclination_ = 179.0f;
    }
    if (inclination_ < 0.1f) {
        inclination_ = 0.1f;
    }

    azimuth_ = azimuth_ - 360.0f * floor(azimuth_ / 360.0f);
    inclination_ = inclination_ - 180.0f * floor(inclination_ / 180.0f);

    pos_.x = sin(glm::radians(inclination_)) * cos(glm::radians(azimuth_)) * distance_;
    pos_.y = cos(glm::radians(inclination_)) * distance_;
    pos_.z = sin(glm::radians(inclination_)) * sin(glm::radians(azimuth_)) * distance_;

    front_ = glm::normalize(-pos_);

    pos_ += center_;
    right_ = glm::normalize(glm::cross(front_, UP_));
    up_ = glm::cross(right_, front_);
}

void Camera::resize(int width, int height)
{
    width_ = width;
    height_ = height;

    glViewport(0, 0, width, height);

    ratio_ = static_cast<float>(width) / static_cast<float>(height);

    if (!resized_) {
        resized_ = true;
        fov_y_ = 2.0f * atan(tan(fov_x_ / 2.0f) * ratio_);
    }
}
