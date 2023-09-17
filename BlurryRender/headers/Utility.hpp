#pragma once
#include <string>
#include <filesystem>
#include <functional>

#define W_CHECK(call)    \
  do                     \
  {                      \
    bool result_ = call; \
    assert(result_);     \
  } while (0)

namespace Utility
{
class Non_copyable
{
private:
  auto operator=(Non_copyable const &) -> Non_copyable & = delete;
  Non_copyable(Non_copyable const &) = delete;

public:
  auto operator=(Non_copyable &&) -> Non_copyable & = default;
  Non_copyable() = default;
  Non_copyable(Non_copyable &&) = default;
};

class Scope_guard : public Non_copyable
{
public:
  template<class Func>
  Scope_guard(Func const &cleanup) : cleanup_(cleanup)
  {
  }

  Scope_guard(Scope_guard &&other) noexcept : cleanup_(move(other.cleanup_)) { dismiss(other); }
  ~Scope_guard() { cleanup_(); }

  friend void dismiss(Scope_guard &g)
  {
    g.cleanup_ = [] {};
  }

private:
  std::function<void()> cleanup_;
};

std::string GetOpenGLContextInformation();
std::filesystem::path GetRootPath(std::wstring rootFolderName);
std::string ReadContentFromFile(const std::string &filePath);
unsigned int LoadTextureFromImage(char const *path);

long long milliseconds_now();
double seconds_now();

}// namespace Utility
