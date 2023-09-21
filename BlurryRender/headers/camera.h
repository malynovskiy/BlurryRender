#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMove
{
  Forward,
  Backward,
  Left,
  Right
};

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

namespace
{
constexpr glm::vec3 Zero{ 0.0f };
constexpr glm::vec3 DefaultUp{ 0.0f, 1.0f, 0.0f };
constexpr glm::vec3 DefaultFront{ 0.0f, 0.0f, -1.0f };
}// namespace

class Camera
{
public:
  // camera Attributes
  glm::vec3 m_position;
  glm::vec3 m_front;
  glm::vec3 m_up;
  glm::vec3 m_right;
  glm::vec3 m_worldUp;

  // euler Angles
  float m_yaw;
  float m_pitch;

  // camera options
  float m_movementSpeed;
  float m_mouseSensitivity;
  float m_zoom;

  // constructor with vectors
  Camera(glm::vec3 position = Zero)
    : m_position{ position },
      m_front{ DefaultFront },
      m_up{ DefaultUp },
      m_worldUp{ m_up },
      m_yaw{ YAW },
      m_pitch{ PITCH },
      m_movementSpeed(SPEED),
      m_mouseSensitivity(SENSITIVITY),
      m_zoom(ZOOM)
  {
    UpdateCameraVectors();
  }

  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_position{ glm::vec3(posX, posY, posZ) },
      m_front{ DefaultFront },
      m_worldUp{ glm::vec3(upX, upY, upZ) },
      m_yaw{ yaw },
      m_pitch{ pitch },
      m_movementSpeed(SPEED),
      m_mouseSensitivity(SENSITIVITY),
      m_zoom(ZOOM)
  {
    UpdateCameraVectors();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix() { return glm::lookAt(m_position, m_position + m_front, m_up); }
  glm::mat4 LookAt(glm::vec3 target) { return glm::lookAt(target, glm::vec3(0.0f, 0.0f, 0.0f), m_worldUp); }

  // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined
  // ENUM (to abstract it from windowing systems)
  void ProcessKeyboard(CameraMove direction, float deltaTime)
  {
    float velocity = m_movementSpeed * deltaTime;
    if (direction == Forward)
      m_position += m_front * velocity;
    if (direction == Backward)
      m_position -= m_front * velocity;
    if (direction == Left)
      m_position -= m_right * velocity;
    if (direction == Right)
      m_position += m_right * velocity;
  }

  // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
  {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
      if (m_pitch > 89.0f)
        m_pitch = 89.0f;
      if (m_pitch < -89.0f)
        m_pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
  }

  // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset)
  {
    m_zoom -= (float)yoffset;
    if (m_zoom < 1.0f)
      m_zoom = 1.0f;
    if (m_zoom > 45.0f)
      m_zoom = 45.0f;
  }

private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void UpdateCameraVectors()
  {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_right =
      glm::normalize(glm::cross(m_front, m_worldUp));// normalize the vectors, because their length gets closer to 0
                                                     // the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
  }
};
#endif
