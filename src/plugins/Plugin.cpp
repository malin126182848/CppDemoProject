#include <functional>
#include "IPlugin.h"

class MyPlugin : public IPlugin
{
 public:
  void init() override;
  void run() override;
  void shutdown() override;
};

class PluginManager
{
 public:
  void loadPlugin(IPlugin* plugin);
  void initPlugins();
  void runPlugins();
  void shutdownPlugins();
};

int add(int numbA, int numbB)
{
  return numbA + numbB;
}
