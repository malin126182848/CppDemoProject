#include <catch2/catch_test_macros.hpp>

#include "CustomEvent.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <map>
#include <tuple>

class TestClass
{
public:
  std::string func1(int val1, bool val2) const
  {
    std::cout << "func1 " << val1 << " " << (val2 ? "true":"false") << '\n';
    return m_str;
  }

  static bool static_func1(int val1, bool val2)
  {
    std::cout << "static_func1 " << val1 << " " << val2 << '\n';
    return true;
  }

  void func2(int val1, bool val2) const
  {
    std::cout << "func2 " << val1 + val << " " << val2 << '\n';
  }

  static void static_func2(int val1, bool val2)
  {
    std::cout << "static_func2 " << val1 << " " << val2 << '\n';
  }

private:
  std::string m_str = "test";
  int val = 2;
};

#define DECLARE_EVENT(EVENT_NAME, EVENT_TYPE, RETURN_TYPE, ...) \
class EVENT_NAME : public CustomEvent<EVENT_TYPE, RETURN_TYPE, __VA_ARGS__> { \
  public: using CustomEvent::CustomEvent; \
};

DECLARE_EVENT(MediaChanged, ICustomEvent::MediaChanged, std::string, int, bool);
DECLARE_EVENT(MediaChangedNon, ICustomEvent::MediaChangedNon, void, int, bool);


///////////////////////////////////////////////////////////////////////////
/// string to args tuple
///////////////////////////////////////////////////////////////////////////

template <typename T>
T read_value(std::istream& input_stream)
{
  T value = {};
  input_stream >> value;
  std::cout << "value : " << value << '\n';
  return value;
}

template<>
bool read_value<bool>(std::istream& input_stream)
{
  bool value = {};
  input_stream >> std::boolalpha >> value;
  std::cout << "value bool : " << std::boolalpha << value << '\n';
  return value;
}

template <typename T>
std::tuple<T> read_as_tuple(std::istream& input_stream)
{
  return std::forward_as_tuple(read_value<T>(input_stream));
}

template <typename T, typename U, typename... Targs>
std::tuple<T, U, Targs...> read_as_tuple(std::istream& input_stream)
{
  std::tuple<T> first = read_as_tuple<T>(input_stream);
  return std::tuple_cat(first, read_as_tuple<U, Targs...>(input_stream));
}

template <typename... Targs>
std::tuple<Targs...> parse_string_to_tuple(const std::string& inputStr) {
    std::istringstream input(inputStr);
    return read_as_tuple<Targs...>(input);
}
///////////////////////////////////////////////////////////////////////////


TEST_CASE("CustomEvent", "CheckReturnValue")
{
  // register factory by type
  std::map<ICustomEvent::Type, std::function<std::shared_ptr<ICustomEvent>()>> factoryMap;
  factoryMap[MediaChanged::eventID()] = [] { return std::make_shared<MediaChanged>(0, false); };
  factoryMap[MediaChangedNon::eventID()] = [] { return std::make_shared<MediaChangedNon>(0, false); };

  auto event = std::static_pointer_cast<MediaChanged>(factoryMap[MediaChanged::eventID()]());
  event->setArgsTuple(parse_string_to_tuple<int, bool>("1 true"));

  auto del = std::make_shared<TestClass>();
  auto lis = makeListener<MediaChanged>(del, &TestClass::func1);
  lis->dispatch(event);
  REQUIRE(event->retValue() == std::string("test"));
}

TEST_CASE("CustomEvent1", "CheckReturnNonValue")
{
  std::map<ICustomEvent::Type, std::function<std::shared_ptr<ICustomEvent>()>> factoryMap;
  factoryMap[MediaChanged::eventID()] = [] { return std::make_shared<MediaChanged>(0, false); };
  factoryMap[MediaChangedNon::eventID()] = [] { return std::make_shared<MediaChangedNon>(0, false); };

  auto event = std::make_shared<MediaChangedNon>(1, true);
  auto del = std::make_shared<TestClass>();
  auto lis = makeListener<MediaChangedNon>(del, &TestClass::func2);
  lis->dispatch(event);
}
