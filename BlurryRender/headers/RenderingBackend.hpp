#pragma once
#include "camera.h"
#include "ShaderProgram.hpp"
#include "model.h"
#include "Primitives.hpp"

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

private:
  ShaderProgram m_preProcessShader;
  ShaderProgram m_postProcessShader;
  ShaderProgram m_sceneShader;
  ShaderProgram m_lightSourceShader;

  bool m_postProcessingBlur = true;
  bool m_postProcessingGradient = true;

  UINT m_framebuffer;
  UINT m_textureColorbuffer;

  // Probably should be some vector with primitives for more extensive usage
  Primitive m_cube;
  Primitive m_plane;
  Primitive m_quad;
  LightPrimitive m_lightSource;

  UINT m_cubeTexture;
  UINT m_planeTexture;
  UINT m_maskTexture;

  std::vector<Model> m_models;

  Camera m_camera;
  glm::vec3 m_lightPosition;

  UINT m_width;
  UINT m_height;
};
