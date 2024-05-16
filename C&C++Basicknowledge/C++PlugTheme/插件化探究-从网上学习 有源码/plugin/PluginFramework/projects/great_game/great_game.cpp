#ifdef WIN32
#include "stdafx.h"
#endif

#include "plugin_framework/PluginManager.h"
#include "plugin_framework/Path.h"
#include "BattleManager.h"
#include "static_plugin/static_plugin.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;

apr_int32_t DummyInvokeService(const apr_byte_t * serviceName, void * serviceParams)
{
  return 0;
}

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main (int argc, char * argv[])
#endif
{
  cout << "Welcome to the great game!" << endl;

  //检测输入参数的个数
  if (argc != 2)
  {
    cout << "Usage: great_game <plugins dir>" << endl;
    return -1;
  }

  // Initialization

  // apr函数库初始化，是一个第三方的，apr的全称是Apache Portable Runtime Project
  ::apr_initialize();

  //单例函数
  PluginManager & pm = PluginManager::getInstance();

  //赋值一个函数指针 
  pm.getPlatformServices().invokeService = DummyInvokeService;

  //加载动态插件
  pm.loadAll(Path::makeAbsolute(argv[1]));
  //加载静态插件
  PluginManager::initializePlugin(StaticPlugin_InitPlugin);

  // Activate the battle manager
  BattleManager::getInstance().go();

  //停止apr函数库
  ::apr_terminate();

  return 0;
}

