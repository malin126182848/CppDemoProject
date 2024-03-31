#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

#include "IPlugin.h"

constexpr size_t MACOS_TYPE = 1;
constexpr size_t WINDOWS_TYPE = 2;
constexpr size_t LINUX_TYPE = 3;

template<typename T, size_t W = 0>
struct someClass
{
protected:
  template<typename Function>
  std::enable_if_t<std::is_invocable_v<Function, T*>>
  exec(Function&& func, T* numbA)
  {
    std::invoke(std::forward<Function>(func), numbA);
  }
};

template<>
struct someClass<int, MACOS_TYPE>
{
public:
  template<typename Function>
  std::enable_if_t<std::is_invocable_v<Function, int*>>
  exec(Function&& func, int* numbA)
  {
    std::invoke(std::forward<Function>(func), numbA);
  }
};

template<>
struct someClass<int, LINUX_TYPE>
{
public:
  template<typename Function>
  std::enable_if_t<std::is_invocable_v<Function, int*>>
  exec(Function&& func, int* numbA)
  {
    std::invoke(std::forward<Function>(func), numbA);
  }
};

enum class FilterType
{
  Windows,
  Linux,
  Mac
};

class IFilter {
public:
  virtual ~IFilter() = default;
  virtual FilterType filterType() const = 0;
};

template<size_t N, typename ...Args>
struct filter : public IFilter
{
  FilterType filterType() const override
  {
    return static_cast<FilterType>(N);
  }

};

template <typename T>
void execFunc(T* func, int* numbA)
{
  func->exec([](const int* valuePtr) { std::cout << *valuePtr << '\n'; }, numbA);
}

TEST_CASE("TmpAddTest", "CheckValues")
{
  std::unique_ptr<IFilter> filter(new ::filter<FilterType::Mac>);
  std::cout << filter->filterType() << '\n';

  /* std::unique_ptr<IFilter> filterPtr = std::unique_ptr<IFilter>(filter); */
  /* std::unique_ptr<someClass<int>> ptr = std::make_unique<wrapper>(); */
  /* int value = 1; */
  /* execFunc(ptr.get(), &value); */

  REQUIRE(add(1, 2) == 3);
}
