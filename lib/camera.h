#ifndef CAMERA_H
#define CAMERA_H

#include "../include/cglm/struct/cam.h"
#include "../include/cglm/struct/vec3.h"

enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

const float YAW = -90.0f;
const float PITCH = 0.0f;

const float SPEED = 2.5f;
const float SENSIBILITY = 0.1f;

const float FOV = 80.0f;

typedef struct {
  // Camera Atributes
  vec3s Position;
  vec3s Front;
  vec3s Up;
  vec3s Right;

  vec3s WorldUp;

  // Euler Angles
  float Yaw;
  float Pitch;

  // Camera Options
  float MovementSpeed;
  float MouseSensibility;
  float Fov;
} Camera;

Camera new_camera(vec3s position, vec3s up, float yaw, float pitch);
Camera new_camera_default(vec3s position);
inline Camera new_camera_scalar(float pos_x, float pos_y, float pos_z,
                                float up_x, float up_y, float up_z, float yaw,
                                float pitch);
mat4s camera_get_view_matrix(Camera *camera);
void camera_process_keyboard(Camera *camera, enum CameraMovement direction,
                             float deltaTime);
void camera_process_mouse_movement(Camera *camera, float x_offset,
                                   float y_offset, bool constraintPitch);
void camera_process_mouse_scroll(Camera *camera, float y_offset);

// Privates
static void update_camera_vectors(Camera *camera);

#endif // CAMERA_H

// #define CAMERA_IMPLEMENTATION
#ifdef CAMERA_IMPLEMENTATION

// Creates a camera with specified initial parameters and returns it.
Camera new_camera(vec3s position, vec3s up, float yaw, float pitch) {
  Camera camera = {
      .Position = position,
      .Front = (vec3s){{0.0f, 0.0f, -1.0f}},
      .WorldUp = up,
      .Yaw = yaw,
      .Pitch = pitch,
      .MovementSpeed = SPEED,
      .MouseSensibility = SENSIBILITY,
      .Fov = FOV,
  };

  update_camera_vectors(&camera);

  return camera;
}

// Creates a camera with default orientation, using the specified position.
Camera new_camera_default(vec3s position) {
  return new_camera(position, (vec3s){{0.0f, 1.0f, 0.0f}}, YAW, PITCH);
}

// Creates a camera with specified initial parameters using scalar values.
Camera new_camera_scalar(float pos_x, float pos_y, float pos_z, float up_x,
                         float up_y, float up_z, float yaw, float pitch) {
  Camera camera = {
      .Position = (vec3s){{pos_x, pos_y, pos_z}},
      .Front = (vec3s){{0.0f, 0.0f, -1.0f}},
      .WorldUp = (vec3s){{up_x, up_y, up_z}},
      .Yaw = yaw,
      .Pitch = pitch,
      .MovementSpeed = SPEED,
      .MouseSensibility = SENSIBILITY,
      .Fov = FOV,
  };

  update_camera_vectors(&camera);

  return camera;
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
inline mat4s camera_get_view_matrix(Camera *camera) {
  return glms_lookat(camera->Position,
                     glms_vec3_add(camera->Position, camera->Front),
                     camera->Up);
}

// processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void camera_process_keyboard(Camera *camera, enum CameraMovement direction,
                             float deltaTime) {
  float velocity = camera->MovementSpeed * deltaTime;
  if (direction == FORWARD) {
    camera->Position =
        glms_vec3_muladds(camera->Front, velocity, camera->Position);
  }
  if (direction == BACKWARD) {
    camera->Position =
        glms_vec3_mulsubs(camera->Front, velocity, camera->Position);
  }
  if (direction == RIGHT) {
    camera->Position =
        glms_vec3_muladds(camera->Right, velocity, camera->Position);
  }
  if (direction == LEFT) {
    camera->Position =
        glms_vec3_mulsubs(camera->Right, velocity, camera->Position);
  }
}

// Processes mouse movement to update the camera's orientation.
void camera_process_mouse_movement(Camera *camera, float x_offset,
                                   float y_offset, bool constraintPitch) {

  x_offset *= camera->MouseSensibility;
  y_offset *= camera->MouseSensibility;

  camera->Yaw += x_offset;
  camera->Pitch += y_offset;

  if (constraintPitch) {
    if (camera->Pitch > 89.0f) {
      camera->Pitch = 89.0f;
    }
    if (camera->Pitch < -89.0f) {
      camera->Pitch = -89.0f;
    }
  }

  update_camera_vectors(camera);
}

// Processes mouse scroll to update the camera's fov (zoom) level.
void camera_process_mouse_scroll(Camera *camera, float y_offset) {
  camera->Fov -= (float)y_offset;
  if (camera->Fov > 80.0f) {
    camera->Fov = 80.0f;
  }
  if (camera->Fov < 1.0f) {
    camera->Fov = 1.0f;
  }
}

// ------------------------------------------------------------------------

// Updates the camera's internal vectors based on its orientation.
// Calculates the front vector from the Camera's (updated) Euler Angles
static void update_camera_vectors(Camera *camera) {
  vec3s front = {
      .x = cos(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch)),
      .y = sin(glm_rad(camera->Pitch)),
      .z = sin(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch)),
  };
  camera->Front = glms_normalize(front);
  camera->Right = glms_normalize(glms_cross(camera->Front, camera->WorldUp));
  camera->Up = glms_normalize(glms_cross(camera->Right, camera->Front));
}

#endif // CAMERA_IMPLEMENTATION
