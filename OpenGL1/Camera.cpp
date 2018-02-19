#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

// @return: singleton Camera reference
Camera& Camera::GetInstance() {
    static Camera c;
    return c;
}

// @return: calculated lookat view matrix for camera
glm::mat4 Camera::GetViewTransform() const {
    return glm::lookAt(camera_pos_, camera_pos_ + camera_front_, world_up_);
}

/* User Input */
// Specify horizontal direction to move camera in
void Camera::Move(CameraMove dir, double delta_time) {
    float camera_speed = move_speed_ * delta_time;
    switch (dir) {
        case CameraMove::kBackward: {
            camera_pos_ -= camera_speed * camera_front_;
            break;
        }
        case CameraMove::kForward: {
            camera_pos_ += camera_speed * camera_front_;
            break;
        }
        case CameraMove::kLeft: {
            // Left is always perpendicular to up and front
            camera_pos_ -= glm::normalize(glm::cross(camera_front_, world_up_)) * camera_speed;
            break;
        }
        case CameraMove::kRight: {
            camera_pos_ += glm::normalize(glm::cross(camera_front_, world_up_)) * camera_speed;
            break;
        }
    }
}

// Pass in mouse position to rotate camera to cursor
// Prevents looking above/below vertical to prevent flipping
void Camera::Look(double x_pos, double y_pos) {
    // first time mouse has been captured by window
    if (first_mouse_) {
        last_x_ = x_pos;
        last_y_ = y_pos;
        first_mouse_ = false;
    }

    double x_offset{ (x_pos - last_x_) * look_sensitivity_ };
    double y_offset{ (last_y_ - y_pos) * look_sensitivity_ }; // since horizontal axis is L->R
    last_x_ = x_pos;
    last_y_ = y_pos;

    yaw_ += x_offset;
    pitch_ += y_offset;

    // prevent pitching above/below 89 degrees
    if (pitch_ > 89.f) { pitch_ = 89.f; }
    if (pitch_ < -89.f) { pitch_ = -89.f; }

    // new front direction
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    camera_front_ = glm::normalize(front);
}

// Pass in scroll wheel value to alter FOV
void Camera::Zoom(double y_offset) {
    float kMaxZoom{ 45.f };
    float kMinZoom{ 1.f };

    fov_ -= y_offset;
    if (fov_ <= 1.f) { fov_ = 1.f; }
    if (fov_ >= 45.f) { fov_ = 45.f; }
}