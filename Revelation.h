#ifndef REVELATION_H
#define REVELATION_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <shlwapi.h>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using boost::format;

#include "Encrypt.h"

#define VER "1.0"

class CConfig;
class CGame;

class CRevelation
{
public:
  CRevelation(HMODULE hModule);
  ~CRevelation();

  static string GetLogPath();
  static void Log(string text);
  static void Log(boost::basic_format<char> fmt) { Log(fmt.str()); }

  CGame* m_Game;

private:
  static string m_LogFile;
  static bool m_LogInitialized;
};

extern CRevelation* g_Revelation;
#endif