#pragma once
#include "camera.h"
#include "Shader.hpp"
#include "Model.h"
#include "Primitives.hpp"

#include <array>

class GLRenderer
{
  using u32 = uint32_t;

public:
  GLRenderer(u32 width, u32 height);
  ~GLRenderer();

  inline u32 GetWidth() const { return m_width; }
  inline u32 GetHeight() const { return m_height; }

  void SetCameraPosition(glm::vec3 position) { m_camera.m_position = position; }

  void OnKeyDown(u32 key);

  void Render();

  void Initialize();
private:
  void CreateModels();

  void LoadTextures();

  void CreateShaders();
  void ConfigureShaders();

  void ConfigureFramebuffer();

  inline void ClearFrame() const;

  void RenderScene();
  void RenderBackground();
  void RenderPostProcessing();

private:
  Shader m_backgroundShader;
  Shader m_blurShader;
  Shader m_sceneShader;
  Shader m_lightSourceShader;
  Shader m_composeShader;

  bool m_postProcessingBlur = true;
  bool m_horizontal = true;

  u32 m_sceneFBO;
  u32 m_sceneColorBuffer;
  static constexpr u32 BlurFramebuffersCount = 2;
  std::array<u32, BlurFramebuffersCount> m_blurFBO;
  std::array<u32, BlurFramebuffersCount> m_blurColorBuffers;

  float m_blurSigma = 0.4f;
  u32 m_blurPasses = 25;

  // Probably should be some vector with primitives for more extensive usage
  Primitive m_cube;
  Primitive m_plane;
  Primitive m_quad;
  LightPrimitive m_lightSource;

  u32 m_cubeTexture;
  u32 m_planeTexture;
  u32 m_maskTexture;

  Model m_model;

  Camera m_camera;
  glm::vec3 m_lightPosition;

  u32 m_width;
  u32 m_height;
};
