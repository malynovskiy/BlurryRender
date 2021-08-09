#pragma once
#include "camera.h"
#include "ShaderProgram.hpp"
#include "model.h"
#include "Primitives.hpp"

#include <array>

class RenderingBackend
{
  using UINT = unsigned int;

public:
  RenderingBackend(UINT width, UINT height);

  UINT GetWidth() const { return m_width; }
  UINT GetHeight() const { return m_height; }

  void SetCameraPosition(glm::vec3 position) { m_camera.Position = position; }

  void OnKeyDown(UINT key);

  void Render();
  void Cleanup();

  void Initialize();
private:

  void RenderScene();
  void RenderBackground();
  void RenderPostProcessing();

private:
  ShaderProgram m_preProcessShader;
  ShaderProgram m_postProcessShader;
  ShaderProgram m_sceneShader;
  ShaderProgram m_lightSourceShader;
  ShaderProgram m_composeShader;

  bool m_postProcessingBlur = true;
  bool m_horizontal = true;

  UINT m_framebuffer;
  UINT m_textureColorbuffer;
  std::array<UINT, 2> m_blurFBO;
  std::array<UINT, 2> m_blurColorBuffers;

  // Probably should be some vector with primitives for more extensive usage
  Primitive m_cube;
  Primitive m_plane;
  Primitive m_quad;
  LightPrimitive m_lightSource;

  UINT m_cubeTexture;
  UINT m_planeTexture;
  UINT m_maskTexture;

  Model m_model;

  Camera m_camera;
  glm::vec3 m_lightPosition;

  UINT m_width;
  UINT m_height;
};
