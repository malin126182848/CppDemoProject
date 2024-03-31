#ifndef IPLUGINS_H_
#define IPLUGINS_H_

class IPlugin
{
public:
  IPlugin(const IPlugin &) = delete;
  IPlugin(IPlugin &&) = delete;
  IPlugin &operator=(const IPlugin &) = delete;
  IPlugin &operator=(IPlugin &&) = delete;
  virtual ~IPlugin() = default;

  virtual void init() = 0;
  virtual void run() = 0;
  virtual void shutdown() = 0;
};

extern "C"
{
  int add(int, int);
}

#endif  // IPLUGINS_H_
