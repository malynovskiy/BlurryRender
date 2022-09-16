#pragma once
#include "camera.h"
#include "ShaderProgram.hpp"
#include "Model.h"
#include "Primitives.hpp"

#include <array>

class GLRenderer
{
  using UINT = unsigned int;

public:
  GLRenderer(UINT width, UINT height);
  ~GLRenderer();

  inline UINT GetWidth() const { return m_width; }
  inline UINT GetHeight() const { return m_height; }

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
  ShaderProgram m_backgroundShader;
  ShaderProgram m_blurShader;
  ShaderProgram m_sceneShader;
  ShaderProgram m_lightSourceShader;
  ShaderProgram m_composeShader;

  bool m_postProcessingBlur = true;
  bool m_horizontal = true;

  UINT m_sceneFBO;
  UINT m_sceneColorBuffer;
  std::array<UINT, 2> m_blurFBO;
  std::array<UINT, 2> m_blurColorBuffers;

  float m_blurSigma = 0.4f;
  UINT m_blurPasses = 25;

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
