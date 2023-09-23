#include "Shader.hpp"
#include <glad/glad.h>

#include "Utility.hpp"

#include <string>
#include <cassert>
#include <iostream>
#include <Windows.h>

inline constexpr uint32_t InfoBufferSize = 512;

Shader::Shader(std::string vertexPath, std::string fragmentPath) : m_descriptor(0)
{
  Initialize(vertexPath, fragmentPath);
}

void Shader::Initialize(std::string vertexPath, std::string fragmentPath)
{
  GLuint vertex = CreateShader(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);

  // Shader Program itself
  m_descriptor = glCreateProgram();
  glAttachShader(m_descriptor, vertex);
  glAttachShader(m_descriptor, fragment);
  glLinkProgram(m_descriptor);

  GLint success = 0;
  glGetProgramiv(m_descriptor, GL_LINK_STATUS, &success);
  if (!success)
  {
    std::string infoLog;
    infoLog.resize(InfoBufferSize);
    glGetProgramInfoLog(m_descriptor, InfoBufferSize, nullptr, infoLog.data());

    std::string output{};
    output += "Failed linkage of GLSL shaders into a shader program.\n";
    output += "Program info log:\n" + infoLog + '\n';
    OutputDebugStringA(output.c_str());
  }

  // shaders linked to our program and no longer need to keep them
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

GLuint Shader::CreateShader(std::string shaderPath, unsigned int type)
{
  const std::string shaderCode = Utility::ReadContentFromFile(shaderPath);
  const char *cShaderCode = shaderCode.c_str();

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &cShaderCode, nullptr);
  glCompileShader(shader);

  std::string infoLog;
  infoLog.resize(InfoBufferSize);
  int success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  assert(success);
  if (!success)
  {
    glGetShaderInfoLog(shader, InfoBufferSize, nullptr, infoLog.data());
    const std::string shaderTypeStr = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
    std::string output = "";
    output += "GLSL compile error" + shaderTypeStr + " shader: '" + shaderPath + "'\n\n";
    output += "Shader info log:\n" + infoLog + '\n';
    OutputDebugStringA(output.c_str());
  }

  return shader;
}

void Shader::use()
{
  glUseProgram(m_descriptor);
}

void Shader::setUniform(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(m_descriptor, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(m_descriptor, name.c_str()), value);
}

void Shader::setUniform(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(m_descriptor, name.c_str()), value);
}

void Shader::setUniform(const std::string &name, const glm::vec2 &value) const
{
  glUniform2fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string &name, float x, float y) const
{
  glUniform2f(glGetUniformLocation(m_descriptor, name.c_str()), x, y);
}

void Shader::setUniform(const std::string &name, const glm::vec3 &value) const
{
  glUniform3fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string &name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(m_descriptor, name.c_str()), x, y, z);
}

void Shader::setUniform(const std::string &name, const glm::vec4 &value) const
{
  glUniform4fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string &name, float x, float y, float z, float w)
{
  glUniform4f(glGetUniformLocation(m_descriptor, name.c_str()), x, y, z, w);
}

void Shader::setUniform(const std::string &name, const glm::mat2 &mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const std::string &name, const glm::mat3 &mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const std::string &name, const glm::mat4 &mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(m_descriptor, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

