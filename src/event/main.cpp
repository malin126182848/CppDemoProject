#include "CustomEvent.h"
#include <iostream>

class MediaChanged : public CustomEvent<ICustomEvent::MediaChanged, void, int, bool>
{
public:
  using CustomEvent<ICustomEvent::MediaChanged, void, int, bool>::CustomEvent;
};

int main()
{
  auto event = std::make_shared<MediaChanged>(1, true);
  auto event1 = std::make_shared<MediaChanged>(0, false);

  {
    auto func = [](int val1, bool val2) {
      std::cout << "lambda " << val1 << " " << val2 << '\n';
    };
    auto lis = makeListener<MediaChanged>(func);
    lis->dispatch(event);
    /* auto callor = Callor<MediaChanged::EventArgs, TestClass, decltype(&TestClass::func1)>(&t_obj, &TestClass::func1); */
    /* Listener<MediaChanged, decltype(callor)> lis(callor); */
    /* lis.dispatch(event); */
  }
  {
    /* auto callor = Callor<MediaChanged::EventArgs, TestClass, decltype(&TestClass::func2)>(t_obj.get(), &TestClass::func2); */
    /* Listener<MediaChanged, decltype(callor)> lis(callor); */
    /* lis.dispatch(event1); */
  }
  {
    /* auto callor = Callor<MediaChanged::EventArgs, void, decltype(&TestClass::static_func1)>(nullptr, &TestClass::static_func1); */
    /* Listener<MediaChanged, decltype(callor)> lis(t_obj, callor); */
    /* lis.dispatch(event); */
  }
  {
    /* auto callor = Callor<MediaChanged::EventArgs, void, decltype(&TestClass::static_func2)>(nullptr, &TestClass::static_func2); */
    /* Listener<MediaChanged, decltype(callor)> lis(t_obj, callor); */
    /* lis.dispatch(event1); */
  }
  {
    /* auto func = [](int val1, bool val2) { */
    /*   std::cout << "lambda " << val1 << " " << val2 << '\n'; */
    /* }; */
    /* auto callor = Callor<MediaChanged::EventArgs, void, decltype(func)>(nullptr, func); */
    /* Listener<MediaChanged, decltype(callor)> lis(t_obj, callor); */
    /* lis.dispatch(event); */
  }
  return 0;
}
