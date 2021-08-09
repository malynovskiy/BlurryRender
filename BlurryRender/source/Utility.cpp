#include "Utility.hpp"

#include <Windows.h>
#include <glad/glad.h>

#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Utility
{
std::string GetOpenGLContextInformation()
{
  std::string contextInfo = "";
  contextInfo += "OpenGL Context: " + std::to_string(GLVersion.major) + "," + std::to_string(GLVersion.minor) + "\n";
  contextInfo += "OpenGL version: " + std::string((const char *)glGetString(GL_VERSION)) + "\n";
  contextInfo += "GLSL Version: " + std::string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION)) + "\n";
  return contextInfo;
}

std::filesystem::path GetRootPath(std::wstring rootFolderName)
{
  std::filesystem::path rootPath;

  WCHAR *exePath = new WCHAR[MAX_PATH];
  GetModuleFileName(nullptr, exePath, MAX_PATH);
  rootPath = std::filesystem::path(exePath);

  while (rootPath.filename().wstring() != rootFolderName)
    rootPath = rootPath.parent_path();

  return rootPath;
}

std::string ReadContentFromFile(const std::string &filePath)
{
  std::ifstream file(filePath);
  std::string content(std::istreambuf_iterator<char>(file.rdbuf()), std::istreambuf_iterator<char>());
  return content;
}

unsigned int LoadTextureFromImage(char const *path)
{
  unsigned int texture;
  glGenTextures(1, &texture);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format = GL_RGB;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    std::cerr << "Texture failed to load at path: " << path << '\n';
  }

  stbi_image_free(data);
  return texture;
}

float GetElapsedTime()
{
  return static_cast<float>(GetCurrentTime() / 100);
}

}// namespace Utility
