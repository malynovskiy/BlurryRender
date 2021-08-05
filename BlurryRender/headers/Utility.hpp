#pragma once
#include <string>
#include <filesystem>

#define W_CHECK(call)    \
  do                     \
  {                      \
    bool result_ = call; \
    assert(result_);     \
  } while (0)

namespace Utility
{
std::string GetOpenGLContextInformation();
std::filesystem::path GetRootPath(std::wstring rootFolderName);
std::string ReadContentFromFile(const std::string &filePath);
unsigned int LoadTextureFromImage(char const *path);
}// namespace Utility
