#include "RenderingBackend.hpp"
#include "Primitives.hpp"
#include "Utility.hpp"
#include <stb_image.h>

// Hard-coded pases for shaders for now
// TODO: Need to be fixed later
namespace
{
constexpr auto PreProcessVertexShaderPath = "shaders/preprocess.vert";
constexpr auto PreProcessFragmentShaderPath = "shaders/preprocess.frag";

constexpr auto SceneVertexShaderPath = "shaders/scene.vert";
constexpr auto SceneFragmentShaderPath = "shaders/scene.frag";

constexpr auto PostProcessVertexShaderPath = "shaders/postprocess.vert";
constexpr auto PostProcessFragmentShaderPath = "shaders/postprocess.frag";
}// namespace

RenderingBackend::RenderingBackend(UINT width, UINT height) : m_width(width), m_height(height), m_camera(), m_models(0)
{
}

void RenderingBackend::Initialize()
{
  m_preProcessShader = ShaderProgram(PreProcessVertexShaderPath, PreProcessFragmentShaderPath);
  m_sceneShader = ShaderProgram(SceneVertexShaderPath, SceneFragmentShaderPath);
  m_postProcessShader = ShaderProgram(PostProcessVertexShaderPath, PostProcessFragmentShaderPath);

  m_cube = Primitive(CubeVertices, CubeVerticesAmount * PrimitiveVertexAttributes);
  m_plane = Primitive(PlaneVertices, PlaneVerticesAmount * PrimitiveVertexAttributes);
  m_quad = Primitive(QuadVertices, PlaneVerticesAmount * PrimitiveVertexAttributes);

  // load textures
  m_cubeTexture = Utility::LoadTextureFromImage("resources/textures/container.jpg");
  m_planeTexture = Utility::LoadTextureFromImage("resources/textures/back.jpg");

  // shader configuration
  m_sceneShader.use();
  m_sceneShader.setUniform("texture1", 0);

  m_postProcessShader.use();
  m_postProcessShader.setUniform("screenTexture", 0);

  // framebuffer configuration
  m_framebuffer = 0;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  // create a color attachment texture
  m_textureColorbuffer = 0;
  glGenTextures(1, &m_textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, m_textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorbuffer, 0);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error, Framebuffer is not complete!\n";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  stbi_set_flip_vertically_on_load(true);
  // TODO: cleanup hardcoded paths
  m_models.push_back(Model("resources/models/backpack/backpack.obj"));
}

void RenderingBackend::Render()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  m_preProcessShader.use();
  glBindVertexArray(m_quad.VAO);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);
  glDepthMask(GL_TRUE);

  glEnable(GL_DEPTH_TEST);// enable depth testing (is disabled for rendering screen-space quad)
  m_sceneShader.use();
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = m_camera.GetViewMatrix();
  glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_width / (float)m_height, 0.1f, 100.0f);
  m_sceneShader.setUniform("view", view);
  m_sceneShader.setUniform("projection", projection);
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

  // render the loaded model
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(1.4f, -0.2f, 0.3f));
  model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
  m_sceneShader.setUniform("model", model);
  m_models[0].Draw(m_sceneShader);

  glBindVertexArray(0);

  // back to default framebuffer and draw a quad plane
  glDisable(GL_DEPTH_TEST);
  m_postProcessShader.use();
  glBindVertexArray(m_quad.VAO);
  //m_postProcessShader.setUniform("applyGradient", m_postProcessingGradient);
  //m_postProcessShader.setUniform("applyBlur", m_postProcessingBlur);
  m_postProcessShader.setUniform("horizontal", 1);
  m_postProcessShader.setUniform("samples", 15);
  m_postProcessShader.setUniform("sigmaFactor", 0.25f);
  // use the color attachment texture as the texture of the quad plane
  glBindTexture(GL_TEXTURE_2D, m_textureColorbuffer);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);


  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // clear all relevant buffers
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  m_postProcessShader.setUniform("horizontal", 0);
  glDrawArrays(GL_TRIANGLES, 0, PlaneVerticesAmount);
}

void RenderingBackend::Cleanup()
{
  glDeleteVertexArrays(1, &m_cube.VAO);
  glDeleteVertexArrays(1, &m_plane.VAO);
  glDeleteVertexArrays(1, &m_quad.VAO);
  glDeleteBuffers(1, &m_cube.VBO);
  glDeleteBuffers(1, &m_plane.VBO);
  glDeleteBuffers(1, &m_quad.VBO);
}

void RenderingBackend::OnKeyDown(UINT key)
{
  switch (key)
  {
  case 'G': {
    m_postProcessingGradient = !m_postProcessingGradient;
  }
  break;
  case 'B': {
    m_postProcessingBlur = !m_postProcessingBlur;
  }
  break;
  }
}
