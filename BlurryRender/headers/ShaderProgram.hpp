#pragma once
#include <glm/glm.hpp>
#include <string>

class ShaderProgram
{
public:
  ShaderProgram() = default;
  ShaderProgram(std::string vertexPath, std::string fragmentPath);
  ~ShaderProgram() = default;

  void use();
  unsigned int getDescriptor() { return m_descriptor; }

  void setUniform(const std::string &name, bool value) const;
  void setUniform(const std::string &name, int value) const;
  void setUniform(const std::string &name, float value) const;
  void setUniform(const std::string &name, const glm::vec2 &value) const;
  void setUniform(const std::string &name, float x, float y) const;
  void setUniform(const std::string &name, const glm::vec3 &value) const;
  void setUniform(const std::string &name, float x, float y, float z) const;
  void setUniform(const std::string &name, const glm::vec4 &value) const;
  void setUniform(const std::string &name, float x, float y, float z, float w);
  void setUniform(const std::string &name, const glm::mat2 &mat) const;
  void setUniform(const std::string &name, const glm::mat3 &mat) const;
  void setUniform(const std::string &name, const glm::mat4 &mat) const;

  static unsigned int CreateShader(std::string shaderPath, unsigned int type);

private:
  void Initialize(std::string vertexPath, std::string fragmentPath);

private:
  unsigned int m_descriptor;
};
