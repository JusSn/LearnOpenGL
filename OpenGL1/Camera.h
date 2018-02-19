/* Loosely based off Camera class at https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h */

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

// options for camera movement
enum class CameraMove {
    kForward, kBackward,
    kLeft, kRight
};

/* Camera class 
* Since our code has no "model" class and only one "view" (for MVC)
* treat Camera as a Singleton for program-wide access
*/

class Camera {
    // current camera position
    glm::vec3 camera_pos_;
    // current camera front direction (normalized)
    glm::vec3 camera_front_;
    // current world up direction (normalized)
    glm::vec3 world_up_;

    // speed to strafe camera 
    float move_speed_;
    // mouse sensitivity
    float look_sensitivity_;
    // current camera field of view
    double fov_;
    // Euler angles
    double yaw_;
    double pitch_;

    // Last known mouse positions
    double last_x_;
    double last_y_;

    // is true if first time mouse is interacting with this camera
    bool first_mouse_;
public:
    // Default camera values
    Camera() :
        camera_pos_{ glm::vec3{ 0.f, 0.f, 3.f } },
        camera_front_{ glm::vec3{ 0.f, 0.f, -1.f} },
        world_up_{ glm::vec3{ 0.f, 1.f, 0.f } },
        move_speed_{ 2.5f },
        look_sensitivity_{ 0.05f },
        fov_{ 45 },
        yaw_{},
        pitch_{},
        last_x_{ 400 },
        last_y_{ 300 },
        first_mouse_{ true } {}

    /* Accessors */
    // @return: Singleton Camera reference
    static Camera& GetInstance();

    // @return: field of view in degrees
    double GetZoom() const { return fov_; }
    // @return: view matrix based on mouse input
    glm::mat4 GetViewTransform() const;
    
    /* User Input */
    // Specify horizontal direction to move camera in
    void Move(CameraMove dir, double delta_time);
    // Pass in mouse position to rotate camera to cursor
    void Look(double x_pos, double y_pos);
    // Pass in scroll wheel value to alter FOV
    void Zoom(double y_offset);
};

#endif // !CAMERA_H
