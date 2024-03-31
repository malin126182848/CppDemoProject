#pragma once

#include <tuple>
#include <type_traits>
#include <memory>

class ICustomEvent
{
public:
  enum Type
  {
    MediaChanged,
    MediaChangedNon,
  };
  virtual ~ICustomEvent() = default;
  virtual bool isA(Type) const = 0;
};

template<typename... Args>
class EventArgument
{
public:
  using TupleArgs = std::tuple<Args...>;
  explicit EventArgument(Args&&... args) : m_args(std::forward<Args>(args)...) { }
  const TupleArgs& argsTuple() const { return m_args; }
  void setArgsTuple(const TupleArgs& args) { m_args = args; }

 private:
  TupleArgs m_args;
};

template<size_t N, typename RetT, typename... Args>
class CustomEvent : public EventArgument<Args...>, public ICustomEvent
{
public:
  using EventRet = RetT;
  using EventArgs = EventArgument<Args...>;
  using EventArgument<Args...>::EventArgument;
  static ICustomEvent::Type eventID() { return static_cast<ICustomEvent::Type>(N); }
  bool isA(ICustomEvent::Type _type) const override { return eventID() == _type; }

  RetT retValue() const
  {
    return m_retValue;
  }
  void setRetValue(const RetT& retValue)
  {
    m_retValue = retValue;
  }
private:
  RetT m_retValue;
};

template<size_t N, typename... Args>
class CustomEvent<N, void, Args...> : public EventArgument<Args...>, public ICustomEvent
{
public:
  using EventRet = void;
  using EventArgs = EventArgument<Args...>;
  using EventArgument<Args...>::EventArgument;
  static ICustomEvent::Type eventID() { return static_cast<ICustomEvent::Type>(N); }
  bool isA(ICustomEvent::Type _type) const override { return eventID() == _type; }
};

template<typename, typename, typename>
class Callor;

template<template<typename...> typename T, typename Object, typename Functor, typename... Args>
class Callor<T<Args...>, Object, Functor>
{
public:
  using EventArgument = T<Args...>;
  Callor(Object* object, Functor functor) : m_obj(object), m_functor(functor)
  {
  }
  auto operator()(const EventArgument& argument)
  {
    if constexpr (std::is_member_function_pointer_v<Functor>)
    {
      return std::apply(m_functor, std::tuple_cat(std::forward_as_tuple(m_obj), argument.argsTuple()));
    }
    else
    {
      return std::apply(m_functor, argument.argsTuple());
    }
  }

private:
  Object* m_obj = nullptr;
  Functor m_functor;
};

/////////////////////////
///Listener
/////////////////////////

class IListener
{
public:
  virtual ~IListener() = default;
  virtual void dispatch(const std::shared_ptr<ICustomEvent>& event) = 0;
};

template <typename WeakT>
class WeakObject
{
public:
  WeakObject() = default;
  explicit WeakObject(WeakT weak) : m_weak(std::move(weak)) {}
  bool expired() const { return m_weak.expired(); }
private:
  WeakT m_weak;
};

template <>
class WeakObject<void>
{
public:
  bool expired() const
  {
    return false;
  }
};

template<typename EventType, typename Callor, typename Weak>
class Listener : public IListener
{
public:
  explicit Listener(Weak weak, Callor call)
    : m_weak(std::move(weak))
    , m_callor(std::move(call))
  {
  }
  void dispatch(const std::shared_ptr<ICustomEvent>& event) override
  {
    if (event && event->isA(EventType::eventID()) && !m_weak.expired())
    {
      auto eventPtr = static_cast<EventType*>(event.get());
      if constexpr (std::is_void_v<typename EventType::EventRet>)
      {
        m_callor(*eventPtr);
      }
      else
      {
        static_assert(std::is_same_v<typename EventType::EventRet, std::invoke_result_t<Callor, const EventType&>>,
            "Function return type must be equ EventRet");
        eventPtr->setRetValue(m_callor(*eventPtr));
      }
    }
  }

private:
  Weak m_weak;
  Callor m_callor;
};

template<typename EventType, typename Object, typename Functor>
std::enable_if_t<std::is_invocable_r_v<typename EventType::EventRet, Callor<typename EventType::EventArgs, Object, Functor>, const EventType&>, std::shared_ptr<IListener>>
makeListener(const std::shared_ptr<Object>& obj, Functor&& func)
{
  using CallorT = Callor<typename EventType::EventArgs, Object, Functor>;
  using Weak = WeakObject<std::weak_ptr<Object>>;
  return std::make_shared<Listener<EventType, CallorT, Weak>>(Weak(std::weak_ptr<Object>(obj)), CallorT(obj.get(), std::forward<Functor>(func)));
}

template<typename EventType, typename Functor>
std::enable_if_t<std::is_invocable_r_v<typename EventType::EventRet, Callor<typename EventType::EventArgs, void, Functor>, const EventType&>, std::shared_ptr<IListener>>
makeListener(Functor&& func)
{
  using CallorT = Callor<typename EventType::EventArgs, void, Functor>;
  using Weak = WeakObject<void>;
  return std::make_shared<Listener<EventType, CallorT, Weak>>(Weak(), CallorT(nullptr, std::forward<Functor>(func)));
}

