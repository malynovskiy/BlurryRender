#include "RenderingBackend.hpp"
#include "Primitives.hpp"
#include "Utility.hpp"

#include <stb_image.h>

// Hard-coded pases for shaders for now
// TODO: Need to be fixed later
namespace
{
constexpr auto BlurVertexShaderPath = "shaders/chessboard.vert";
constexpr auto BlurFragmentShaderPath = "shaders/chessboard.frag";
constexpr auto SceneVertexShaderPath = "shaders/scene.vert";
constexpr auto SceneFragmentShaderPath = "shaders/scene.frag";
constexpr auto BackgroundVertexShaderPath = "shaders/blur.vert";
constexpr auto BackgroundFragmentShaderPath = "shaders/blur.frag";
constexpr auto LightSourceVertexShaderPath = "shaders/light_source.vert";
constexpr auto LightSourceFragmentShaderPath = "shaders/light_source.frag";
constexpr auto ComposeVertShaderPath = "shaders/compose.vert";
constexpr auto ComposeFragShaderPath = "shaders/compose.frag";
}// namespace

float sigmaFactor = 0.25f;
UINT BlurPasses = 10;

RenderingBackend::RenderingBackend(UINT width, UINT height) : m_width(width), m_height(height), m_camera() {}

void RenderingBackend::Initialize()
{
  m_backgroundShader = ShaderProgram(BlurVertexShaderPath, BlurFragmentShaderPath);
  m_sceneShader = ShaderProgram(SceneVertexShaderPath, SceneFragmentShaderPath);
  m_blurShader = ShaderProgram(BackgroundVertexShaderPath, BackgroundFragmentShaderPath);
  m_lightSourceShader = ShaderProgram(LightSourceVertexShaderPath, LightSourceFragmentShaderPath);
  m_composeShader = ShaderProgram(ComposeVertShaderPath, ComposeFragShaderPath);

  m_cube = Primitive(CubeVertices, CubeVerticesAmount * PositionNormalTextureAttrib, Primitive::PositionNormalTexture);
  m_plane =
    Primitive(PlaneVertices, PlaneVerticesAmount * PositionNormalTextureAttrib, Primitive::PositionNormalTexture);
  m_quad = Primitive(QuadVertices, PlaneVerticesAmount * PositionTextureAttrib, Primitive::PositionTexture);
  m_lightSource = LightPrimitive(CubeVertices, CubeVerticesAmount * PositionNormalTextureAttrib);

  // load textures
  m_cubeTexture = Utility::LoadTextureFromImage("resources/textures/container.jpg");
  m_planeTexture = Utility::LoadTextureFromImage("resources/textures/back.jpg");
  m_maskTexture = Utility::LoadTextureFromImage("resources/textures/gradient_mask.png");

  // shader configuration
  m_backgroundShader.use();
  glm::vec2 resolution = glm::vec2(static_cast<float>(m_width), static_cast<float>(m_height));
  m_backgroundShader.setUniform("resolution", resolution);

  m_sceneShader.use();
  m_sceneShader.setUniform("objectTexture", 0);

  m_blurShader.use();
  m_blurShader.setUniform("resolution", resolution);
  m_blurShader.setUniform("screenTexture", 0);
  m_blurShader.setUniform("maskTexture", 1);

  m_composeShader.setUniform("screenTexture", 0);

  // Background framebuffer configuration
  glGenFramebuffers(1, &m_sceneFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
  glGenTextures(1, &m_sceneColorBuffer);
  glBindTexture(GL_TEXTURE_2D, m_sceneColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // attach texture to framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneColorBuffer, 0);

  UINT rbo{};
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
  W_CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error, Framebuffer is not complete!\n";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Blur framebuffers configuration
  glGenFramebuffers(2, m_blurFBO.data());
  glGenTextures(2, m_blurColorBuffers.data());
  for (size_t i = 0; i < 2; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[i]);
    // create a color attachment texture
    glBindTexture(GL_TEXTURE_2D, m_blurColorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurColorBuffers[i], 0);

    W_CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cerr << "Error, Framebuffer is not complete!\n";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  stbi_set_flip_vertically_on_load(true);
  // TODO: cleanup hardcoded paths
  m_model = Model("resources/models/backpack/backpack.obj");

  m_lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
}

void RenderingBackend::RenderBackground()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  m_backgroundShader.use();
  glBindVertexArray(m_quad.VAO);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
}

void RenderingBackend::RenderScene()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);

  m_sceneShader.use();
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = m_camera.GetViewMatrix();
  glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_width / (float)m_height, 0.1f, 100.0f);
  m_sceneShader.setUniform("view", view);
  m_sceneShader.setUniform("projection", projection);
  m_sceneShader.setUniform("lightColor", 1.0f, 1.0f, 1.0f);
  m_sceneShader.setUniform("lightPos", m_lightPosition);
  m_sceneShader.setUniform("viewPos", m_camera.Position);

  // cubes
  glBindVertexArray(m_cube.VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
  model = glm::translate(model, glm::vec3(-1.6f, -0.5f, -1.0f));
  m_sceneShader.setUniform("model", model);
  glDrawArrays(GL_TRIANGLES, 0, CubeVerticesAmount);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -0.5f, -0.5f));
  m_sceneShader.setUniform("model", model);
  glDrawArrays(GL_TRIANGLES, 0, CubeVerticesAmount);
  // floor
  glBindVertexArray(m_plane.VAO);
  glBindTexture(GL_TEXTURE_2D, m_planeTexture);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
  m_sceneShader.setUniform("model", model);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);

  // model
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(1.4f, -0.2f, 0.3f));
  model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
  m_sceneShader.setUniform("model", model);
  m_model.Draw(m_sceneShader);

  // Light source
  m_lightSourceShader.use();
  m_lightSourceShader.setUniform("projection", projection);
  m_lightSourceShader.setUniform("view", view);
  model = glm::mat4(1.0f);
  /* m_lightPosition.x = 1.0f + sin(Utility::GetElapsedTime()) * 2.0f;
   m_lightPosition.y = sin(Utility::GetElapsedTime() / 2.0f) * 1.0f;*/
  model = glm::translate(model, m_lightPosition);
  model = glm::scale(model, glm::vec3(0.2f));
  m_lightSourceShader.setUniform("model", model);
  glBindVertexArray(m_lightSource.VAO);
  glDrawArrays(GL_TRIANGLES, 0, CubeVerticesAmount);
}

void RenderingBackend::RenderPostProcessing()
{
  bool first_iteration = true;
  m_blurShader.use();
  m_blurShader.setUniform("samples", 5);
  m_blurShader.setUniform("sigmaFactor", sigmaFactor);
  glBindVertexArray(m_quad.VAO);
  for (size_t i = 0; i < BlurPasses; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO[m_horizontal]);
    m_blurShader.setUniform("horizontal", m_horizontal);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? m_sceneColorBuffer : m_blurColorBuffers[!m_horizontal]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);

    glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);

    m_horizontal = !m_horizontal;
    if (first_iteration)
      first_iteration = false;
  }
  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingBackend::Render()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderBackground();
  RenderScene();
  RenderPostProcessing();

  // Composing everything into default framebuffer for presentation
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_composeShader.use();
  glBindTexture(GL_TEXTURE_2D, m_blurColorBuffers[!m_horizontal]);
  glBindVertexArray(m_quad.VAO);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);
  glBindVertexArray(0);
}

void RenderingBackend::Cleanup()
{
  glDeleteVertexArrays(1, &m_cube.VAO);
  glDeleteVertexArrays(1, &m_plane.VAO);
  glDeleteVertexArrays(1, &m_quad.VAO);
  glDeleteVertexArrays(1, &m_lightSource.VAO);
  glDeleteBuffers(1, &m_cube.VBO);
  glDeleteBuffers(1, &m_plane.VBO);
  glDeleteBuffers(1, &m_quad.VBO);
  glDeleteBuffers(1, &m_lightSource.VBO);
}

namespace
{
void ProcessKeyboard(glm::vec3 &position, Camera_Movement direction, float deltaTime)
{
  constexpr float SPEED = 2.5f;
  constexpr glm::vec3 Forward(0.0f, 0.0f, -1.0f);
  constexpr glm::vec3 Up(0.0f, 1.0f, 0.0f);
  const glm::vec3 Right(1.0f, 1.0f, 0.0f);

  const float velocity = SPEED * deltaTime;

  if (direction == FORWARD)
    position += Forward * velocity;
  if (direction == BACKWARD)
    position -= Forward * velocity;
  if (direction == LEFT)
    position -= Right * velocity;
  if (direction == RIGHT)
    position += Right * velocity;
}
}// namespace

void RenderingBackend::OnKeyDown(UINT key)
{
  switch (key)
  {
  case 'W': {
    m_camera.ProcessKeyboard(FORWARD, 0.1);
  }
  break;
  case 'A': {
    m_camera.ProcessKeyboard(LEFT, 0.1);
  }
  break;
  case 'S': {
    m_camera.ProcessKeyboard(BACKWARD, 0.1);
  }
  break;
  case 'D': {
    m_camera.ProcessKeyboard(RIGHT, 0.1);
  }
  break;

  case 'I': {
    ProcessKeyboard(m_lightPosition, FORWARD, 0.2);
  }
  break;
  case 'J': {
    ProcessKeyboard(m_lightPosition, LEFT, 0.2);
  }
  break;
  case 'K': {
    ProcessKeyboard(m_lightPosition, BACKWARD, 0.2);
  }
  break;
  case 'L': {
    ProcessKeyboard(m_lightPosition, RIGHT, 0.2);
  }
  break;

  case '1': {
    BlurPasses -= 1;
  }
  break;

  case '2': {
    BlurPasses += 1;
  }
  break;

  case '3': {
    sigmaFactor -= 0.1;
  }
  break;
  
  case '4': {
    sigmaFactor += 0.1;
  }
  break;
    /*case 'G': {
      m_horizontal = !m_horizontal;
    }
    break;
    case 'B': {
      m_postProcessingBlur = !m_postProcessingBlur;
    }
    break;*/
  }
}
